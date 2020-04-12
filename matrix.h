#ifndef __MATRIX_H__
#define __MATRIX_H__
#define MAX_COLS 6
#define MAX_ROWS 5
#define MAGIC_DEBOUNCE_NUMBER 0

// dead keys 28-31
// dead low keys vt to us
#define KEY_OVERLAY_1 1
#define KEY_MOUSE_LEFT 28
#define KEY_DISUSED_1 29
#define KEY_DISUSED_2 30
#define KEY_DISUSED_3 31
// 128-135 used by keyboard for key left and right modifiers
// 136> are sent to the computer by subtracting 136 from them, explaining the odd layout of the key address space
// FREE 137-175
// macros must be contiguous as indexing is based on their keycode
#define KEY_MACRO_ONE 137
#define KEY_MACRO_TWO 138
#define KEY_MACRO_THREE 139
#define KEY_MACRO_FOUR 140
#define KEY_MACRO_FIVE 141
// 176-179 // in use by escape/return/bckspc
// FREE 180-190
// 193-205 // low function keys and esc
// 209-218 // 6 floating keys and arrow keys
// 240-251 high function keys

#include <avr/pgmspace.h>
#include "overlay.h"
#include "macro.h"

Overlay* overlays = 0;
short enableOverlay = 0;

#ifdef IO_EXPANDER
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 ioexp;
#endif // IO_EXPANDER

#ifdef WPM
// resets every 50 days, probably not a concern
unsigned long lastMs = 0;
char keysPressed[60];
unsigned long ms = 0;
unsigned long diff = 0;
char keyIndex = 0;
#define SHIFT_THRESH 1000
void cpmlog() {
  // compute and log the current wpm
  short total = 0;
  short count = 0;
  for (short i = 0; i < 60; ++i) {
    total += keysPressed[i];
    if (total != 0) {
       count += 1;
    }
  }
  char buf[50];
  sprintf_P(buf, PSTR("cpm: %d wpm: %d"), total, total/5);
  Serial.println(buf);
}
void cpmproc() {
  ms = millis();
  diff = ms-lastMs;
  if (lastMs == 0) {
    memset(keysPressed, 0, sizeof(unsigned char)*60);
    lastMs = ms;
  } else if (diff > SHIFT_THRESH) {
    int ki = (diff/1000);
    keyIndex += ki;
    keysPressed[keyIndex] = 0;
    if (keyIndex >= 60) {
      keyIndex = 0;
    }
    lastMs = ms;
  }
}
void cpmincrease() {
  keysPressed[keyIndex] += 1;
  cpmlog();
}
void cpmdecrease() {
  keysPressed[keyIndex] -= 1;
  cpmlog();
}
#endif // WPM

class Matrix {
public:
  void init(char* name, short cols, short rows) {
    memset(this, 0, sizeof(Matrix));
    this->name = name;
    this->rowDriven = true;
    this->columnCount = cols;
    this->rowCount = rows;
    this->columns = malloc(sizeof(char)*cols);
    this->rows = malloc(sizeof(char)*rows);
    memset(this->columns, -1, sizeof(char)*cols);
    memset(this->rows, -1, sizeof(char)*rows);
    this->keyset = malloc(sizeof(char)*rows*cols);
    memset(this->keyset, 0, sizeof(char)*rows*cols);
    this->overlayPressed = malloc(sizeof(char)*rows*cols);
    memset(this->overlayPressed, 0, sizeof(char)*rows*cols);
  }

  void setRemote() {
  #ifdef IO_EXPANDER
    this->remote = true;
  #endif
  }

  #ifdef IO_EXPANDER
  bool isRemote() {
    return this->remote;
  }
  #endif

  short getHighCount() {
    return this->rowDriven ? this->rowCount : this->columnCount;
  }

  short getLowCount() {
    return this->rowDriven ? this->columnCount : this->rowCount;
  }

  short getPos(char col, char row) {
    return (col*this->rowCount)+row;
  }

  // don't edit these at runtime please
  char* columns;
  char* rows;
  bool rowDriven;
  const char* name;
  unsigned char* keyset;
  char* overlayPressed;
  unsigned char keymap[MAX_COLS][MAX_ROWS];

private:
  short columnCount;
  short rowCount;
#ifdef IO_EXPANDER
  // at the other end of MCP23017, use ioexp instead of digitalWrite directly
  bool remote;
#endif // IO_EXPANDER
};

void configure_matrix(Matrix* m) {
  // configure the read pins as pullup high to prevent floating
  for (int i = 0; i < m->getLowCount(); ++i) {
    short pin = m->rowDriven ? m->columns[i] : m->rows[i];
    if (pin != -1) {
#ifdef IO_EXPANDER
      if (m->isRemote()) {
        ioexp.pullUp(pin, HIGH);
      } else {
        pinMode(pin, INPUT_PULLUP);
      }
#else
      pinMode(pin, INPUT_PULLUP);
#endif // IO_EXPANDER
    }
  }
}


#include <Mouse.h>
void pressKeyImpl(unsigned char key) {
  if (key == KEY_MOUSE_LEFT) {
    Mouse.click();
    return;
  }
#ifdef WPM
  if (key == KEY_BACKSPACE) {
    cpmdecrease();
  } else {
    cpmincrease();
  }
#endif // WPM
#ifdef REAL_KEYBOARD
  Keyboard.press(key);
#endif
}

void pressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (enableOverlay != 0) {
    // if this isnt handled, let the original key go through
    unsigned char overlayKey = overlays[enableOverlay - 1].keymap[matrixIndex][col][row];
    if (overlayKey > 0) {
      log(F("overlay: key: %d %d, %d for matrix %s"), overlayKey, col, row, m->name);
      m->overlayPressed[m->getPos(col, row)] = enableOverlay;
      if (overlayKey >= KEY_MACRO_ONE && overlayKey <= KEY_MACRO_FIVE) {
        overlayKey -= KEY_MACRO_ONE;
        log(F("macro key pressed for index %d"), overlayKey);
        macros[overlayKey].execute();
      } else {
        pressKeyImpl(overlayKey);
      }
      return;
    } else {
      log(F("no overlay (%d enabled): %d, %d overlaykey %d for matrix %s"), enableOverlay, col, row, overlayKey, m->name);
      //return; // suppress fallthrough keypresses when the overlay isn't bound to fix confusing keypress issues
    }
  }
  unsigned char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    // enable this overlay
    enableOverlay = key;
    log(F("enable overlay %d"), enableOverlay);
    return;
  }
  pressKeyImpl(key);
}

void unpressKeyImpl(char key) {
  if (key == KEY_MOUSE_LEFT) {
    return;
  }
#ifdef REAL_KEYBOARD
  Keyboard.release(key);
#endif
}


#ifdef WATCHDOG_ENABLED
#include <avr/wdt.h>
#endif
void remoteIoFailed() {
  Serial.write(F("Bad remote IO call reply"));
#ifdef WATCHDOG_ENABLED
  Serial.println(F(" system halt!"));
  wdt_enable(WDTO_15MS);
  while(1) {
    delay(1);
  }
#else
  Serial.println(F(" watchdog disabled - no action"));
#endif
}

void unpressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (m->overlayPressed[m->getPos(col, row)] != 0) {
    // unpress overlay key instead
    short overlay = m->overlayPressed[m->getPos(col, row)];
    unsigned char overlayKey = overlays[overlay - 1].keymap[matrixIndex][col][row];
    unpressKeyImpl(overlayKey);
    m->overlayPressed[m->getPos(col, row)] = 0;
    return;
  }
  unsigned char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    log(F("disable overlay %d"), enableOverlay);
    enableOverlay = 0;
  } else if (key == 0) {
    log(F("key missing %d %d mx %s"), col, row, m->name);
  } else {
    unpressKeyImpl(key);
  }
}

// saves passing an arg around
uint16_t allRows = 0;
inline bool readPin(short pin, Matrix* m) {
#ifdef IO_EXPANDER
  if (m->isRemote()) {
    //return !ioexp.digitalRead(low[i]); // deprecated in favour of faster all-row read method
    return !((allRows >> pin) & 0x1);
  } else {
    return !digitalRead(pin);
  }
#else
    return !digitalRead(pin);
#endif // IO_EXPANDER
}

void processOne(Matrix* m, short matrixIndex, short index) {
  // Point to rows and cols depending on drive type
  char* high = m->rows;
  char* low = m->columns;
  if (!m->rowDriven) {
    high = m->columns;
    low = m->rows;
  }

  // Sanity check
  if (high[index] == -1) {
    log(F("High %d missing from matrix %s"), index, m->name);
    return;
  }

  // Set high
#ifdef IO_EXPANDER
  if (m->isRemote()) {
    bool reply = ioexp.pinMode(high[index], OUTPUT);
    if (!reply) {
      remoteIoFailed();
    }
    //ioexp.digitalWrite(high[index], LOW);
  } else {
    pinMode(high[index], OUTPUT);
    digitalWrite(high[index], LOW);
  }
#else
  pinMode(high[index], OUTPUT);
  digitalWrite(high[index], LOW);
#endif // IO_EXPANDER
  
  // Prepare to read rows and act on col/row lookups
  short column = index;
  short row = index; // will be corrected in loop
#ifdef IO_EXPANDER
  if (m->isRemote()) {
    // read all the pins now
    allRows = ioexp.readGPIOAB();
  }
#endif // IO_EXPANDER
  bool read = false;
  for (short i = 0; i < m->getLowCount(); ++i) {
    if (low[i] == -1) {
      continue;
    }

    // Read the low pin
    read = readPin(low[i], m);

    // Process the read result
    if (m->rowDriven) {
      column = i;
    } else {
      row = i;
    }
    if (read) {
      short keysetPos = m->getPos(column, row);
      // Key is held, add to keyset value
      m->keyset[keysetPos] += 1;
      if (m->keyset[keysetPos] == 254) {
        // nearly at short wraparound, jump back to 2
        m->keyset[keysetPos] = 2;
      }
      if (m->keymap[keysetPos] != 0 && m->keyset[keysetPos] == 1) {
        log(F("keypress: %d, %d allread %u for matrix %s"), column, row, allRows, m->name);
        pressKey(m, matrixIndex, column, row);
      } else if (m->keymap[column][row] == 0) {
        log(F("Could not read key for col %d and row %d for matrix %s"), column, row, m->name);
      } else {
#ifdef VERBOSE
        log(F("pos %d %d c %d mx %s"), column, row, m->keyset[keysetPos], m->name);
#endif
      }
      // Only unpress a key if it's been held long enough (swap this if keys ghost a lot)
    } else if ((m->keyset[m->getPos(column, row)] > MAGIC_DEBOUNCE_NUMBER)
#ifdef IO_EXPANDER
    || (m->keyset[m->getPos(column, row)]>0&&m->isRemote())
#endif // IO_EXPANDER
    ) {
#ifdef DEBUG_RELEASE
      log(F("key release: %d, %d allread %u for matrix %s"), column, row, allRows, m->name);
#endif // DEBUG_RELEASE
      unpressKey(m, matrixIndex, column, row);
      m->keyset[m->getPos(column, row)] = 0;
    }
  }
  // Set column low
#ifdef IO_EXPANDER
  if (m->isRemote()) {
    ioexp.pinMode(high[index], INPUT);
  } else {
    pinMode(high[index], INPUT);
  }
#else
    pinMode(high[index], INPUT);
#endif // IO_EXPANDER
}

// Process each row/column
void process(Matrix* m, short matrixIndex) {
  short matrixMax = m->getHighCount();
  for (short i = 0; i < matrixMax; ++i) {
    processOne(m, matrixIndex, i);
  }
}

#endif // __MATRIX_H__
