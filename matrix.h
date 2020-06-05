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
// MACROS 137-175
// macros must be contiguous as usage indexes indexing is based on their keycode
#define KEY_MACRO_1 137
#define KEY_MACRO_2 138
#define KEY_MACRO_3 139
#define KEY_MACRO_4 140
#define KEY_MACRO_5 141
#define KEY_MACRO_6 142
#define KEY_MACRO_7 143
#define KEY_MACRO_8 144
#define KEY_MACRO_9 145
#define KEY_MACRO_10 146
#define KEY_MACRO_11 147
#define KEY_MACRO_12 148
#define KEY_MACRO_13 149
#define KEY_MACRO_14 150
#define KEY_MACRO_15 151
#define KEY_MACRO_16 152
#define KEY_MACRO_17 153
#define KEY_MACRO_18 154
#define KEY_MACRO_19 155
#define KEY_MACRO_20 156
#define KEY_MACRO_MAX 156
// 176-179 // in use by escape/return/bckspc
// internal keys 180-190
#define KEY_SHOW_KEYSTROKES 180
#define KEY_SAVE_KEYSTROKES 181
// 193-205 // low function keys and esc
#define KEY_PRINT_SCREEN 206
// 209-218 // 6 floating keys and arrow keys
// 240-251 high function keys

#include <avr/pgmspace.h> // methods and macros for accessing program space / flash memory
#include "overlay.h"
#include "macro.h"

#ifdef LIFETIME_KEYSTROKES
#include <EEPROM.h>
unsigned long keystrokes = 0;
void saveKeystrokes(bool disableSafety) {
  unsigned long eepromKeystrokes = 0;
  EEPROM.get(0, eepromKeystrokes);
  if (keystrokes-eepromKeystrokes >= 5000 || disableSafety) {
    if(Serial)Serial.println(F("Saving keystrokes"));
    EEPROM.put(0, keystrokes);
  } else {
    if(Serial)Serial.println(F("Keystroke save suppressed"));
  }
}
#define HOUR_MS 3600000
unsigned long nextUpdateMs = HOUR_MS;
void savecheck() {
  unsigned long ms = millis();
  if (ms > nextUpdateMs) {
    saveKeystrokes(false);
    nextUpdateMs = ms + HOUR_MS;
    if (nextUpdateMs < HOUR_MS) { // wraparound check
      nextUpdateMs = HOUR_MS;
    }
  }
}
#endif // LIFETIME_KEYSTROKES

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
  sprintf_P(buf, PSTR("cpm: %d wpm: %d"), total, total / 5);
  if(Serial)Serial.println(buf);
}
void cpmproc() {
  ms = millis();
  diff = ms - lastMs;
  if (lastMs == 0) {
    memset(keysPressed, 0, sizeof(unsigned char) * 60);
    lastMs = ms;
  } else if (diff > SHIFT_THRESH) {
    int ki = (diff / 1000);
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
      this->columns = malloc(sizeof(char) * cols);
      this->rows = malloc(sizeof(char) * rows);
      memset(this->columns, -1, sizeof(char)*cols);
      memset(this->rows, -1, sizeof(char)*rows);
      this->keyset = malloc(sizeof(char) * rows * cols);
      memset(this->keyset, 0, sizeof(char)*rows * cols);
      this->overlayPressed = malloc(sizeof(char) * rows * cols);
      memset(this->overlayPressed, 0, sizeof(char)*rows * cols);
    }

    void setRemote() {
#ifdef IO_EXPANDER
      this->remote = true;
#endif
    }

    bool isRemote() {
#ifdef IO_EXPANDER
      return this->remote;
      #else
      return false;
#endif
    }

    short getHighCount() {
      return this->rowDriven ? this->rowCount : this->columnCount;
    }

    short getLowCount() {
      return this->rowDriven ? this->columnCount : this->rowCount;
    }

    short getPos(char col, char row) {
      return (col * this->rowCount) + row;
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
// handles special overrides for keys. overlays modifying keys is handled in pressKey
void pressKeyImpl(unsigned char key) {
#ifdef LIFETIME_KEYSTROKES
  keystrokes += 1;
  if (key == KEY_SHOW_KEYSTROKES) {
    if(Serial)Serial.print(F("lifetime keystrokes: "));
    if(Serial)Serial.print(keystrokes, DEC);
    if(Serial)Serial.println("");
    return;
  } else if (key == KEY_SAVE_KEYSTROKES) {
    saveKeystrokes(true);
    return;
  }
#endif // LIFETIME_KEYSTROKES
#ifdef WPM
  if (key == KEY_BACKSPACE) {
    cpmdecrease();
  } else {
    cpmincrease();
  }
#endif // WPM
  if (key == KEY_MOUSE_LEFT) {
    Mouse.click();
    return;
  }
  if (key >= KEY_MACRO_1 && key <= KEY_MACRO_MAX) {
    key -= KEY_MACRO_1;
    log(F("macro key pressed for index %d"), key);
    macros[key].execute();
    return;
  }
  if (key == KEY_OVERLAY_1) {
    // enable this overlay
    enableOverlay = key;
    log(F("enable overlay %d"), enableOverlay);
    return;
  }
#ifdef REAL_KEYBOARD
  Keyboard.press(key);
#endif
}
bool hottoggle = true;
void hotproc(short col, short row);
void hotprocup(short col, short row);
void hotprocdown(short col, short row);
void pressKey(Matrix* m, short matrixIndex, short col, short row) {
  hotprocup(col, row);
  if (hottoggle) {
    hotproc(col, row);
  }
  if (enableOverlay != 0) {
    // if this isnt handled, let the original key go through
    unsigned char overlayKey = overlays[enableOverlay - 1].keymap[matrixIndex][col][row];
    if (overlayKey > 0) {
      log(F("overlay: key: %d %d, %d for matrix %s"), overlayKey, col, row, m->name);
      m->overlayPressed[m->getPos(col, row)] = enableOverlay;
      pressKeyImpl(overlayKey);
      return;
    } else {
      log(F("no overlay (%d enabled): %d, %d overlaykey %d for matrix %s"), enableOverlay, col, row, overlayKey, m->name);
      //return; // suppress fallthrough keypresses when the overlay isn't bound to fix confusing keypress issues
    }
  }
  pressKeyImpl(m->keymap[col][row]);
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
#ifdef LIFETIME_KEYSTROKES
  saveKeystrokes(false);
#endif // LIFETIME_KEYSTROKES
  if(Serial)Serial.write(F("Bad remote IO call reply"));
#ifdef WATCHDOG_ENABLED
  if(Serial)Serial.println(F(" system halt!"));
  wdt_enable(WDTO_15MS);
  while (1) {
    delay(1);
  }
#else
  if(Serial)Serial.println(F(" watchdog disabled - no action"));
#endif
}

void unpressKey(Matrix* m, short matrixIndex, short col, short row) {
  hotprocdown(col, row);
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
        log(F("pos %d %d c %d mx %s r %d d %d"), column, row, m->keyset[keysetPos], m->name, low[i], high[index]);
#endif
      }
      // Only unpress a key if it's been held long enough (swap this if keys ghost a lot)
    } else if ((m->keyset[m->getPos(column, row)] > MAGIC_DEBOUNCE_NUMBER)
#ifdef IO_EXPANDER
               || (m->keyset[m->getPos(column, row)] > 0 && m->isRemote())
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













char* newline = "/ ";
char* sp = " ";

#define NAME_MAX 15
class Keyswitch {
  char* manufacturer;
  char* type;
  char name[NAME_MAX];
  char pressure;
  char activation;

  public:
  Keyswitch() {
    
  }
  Keyswitch(char* manufacturer, char* type, const __FlashStringHelper* name, char pressure, char activationNm) {
    this->type = type;
    this->manufacturer = manufacturer;
    vsnprintf_P(this->name, NAME_MAX, (PGM_P)name, 0);
    this->pressure = pressure;
    this->activation = activationNm;
  }
  void write() {
    keywrite("\r\n");
    keywrite(manufacturer);
    keywrite(sp);
    keywrite(this->name);
    keywrite(sp);
    keywrite(" - ");
    keywrite(this->type);
    keywrite(sp);
    keywrite(newline);
    
    keywrite("Force: ");
    char buf[25];
    sprintf(buf, "%d", pressure);
    keywrite(buf);
    keywrite("cN");
    keywrite(sp);
    keywrite(newline);

    keywrite("Activation: ");
    dtostrf(this->activation/10.0f, 2, 1, buf);
    keywrite(buf);
    keywrite("mm");
    keywrite(sp);
    keywrite(newline);
    
    keywrite("Key: ");
  }
  void keywrite(char* s) {
    Keyboard.write(s, strlen(s));
  }
};

Keyswitch look[5][5];
char* gateron = "Gateron";
char* kailh = "Kailh";
char* cherry = "Cherry MX";
char* linear = "Linear";
char* tactile = "Tactile";
char* clicky = "Clicky";


bool initswitched=false;
void initswitch() {
  look[0][0] = Keyswitch(gateron, linear, F("Clear"), 35, 20);
  look[1][0] = Keyswitch(kailh, linear, F("Box Red"), 45, 18);
  look[2][0] = Keyswitch(gateron, linear, F("Red"), 45, 20);
  look[3][0] = Keyswitch(cherry, linear, F("Red"), 45, 20);
  look[4][0] = Keyswitch(gateron, linear, F("Silent Black"), 50, 20);
  look[0][1] = Keyswitch(kailh, linear, F("Box Black"), 60, 18);
  look[1][1] = Keyswitch(cherry, linear, F("Silent Black"), 60, 19);
  //look[2][1] = Keyswitch(gateron, linear, F("Red"), 35, 2);
  look[3][1] = Keyswitch(kailh, tactile, F("Box Brown"), 45, 18);
  look[4][1] = Keyswitch(gateron, tactile, F("Brown"), 45, 20);
  look[0][2] = Keyswitch(kailh, tactile, F("Purple"), 50, 17);
  look[1][2] = Keyswitch(cherry, tactile, F("Brown"), 55, 20);
  look[2][2] = Keyswitch(cherry, tactile, F("Clear"), 65, 20);
  look[3][2] = Keyswitch(kailh, tactile, F("Speed Copper"), 40, 11);
  look[4][2] = Keyswitch(cherry, tactile, F("Grey"), 80, 20);
  look[0][3] = Keyswitch(kailh, clicky, F("Box White"), 45, 18);
  look[1][3] = Keyswitch(cherry, clicky, F("Blue"), 60, 22);
  look[2][3] = Keyswitch(gateron, clicky, F("Blue"), 60, 23);
  look[3][3] = Keyswitch(cherry, clicky, F("Green"), 80, 22);
  look[4][3] = Keyswitch(gateron, clicky, F("Green"), 80, 24);
  look[0][4] = Keyswitch(cherry, clicky, F("White"), 80, 22);
  look[2][4] = Keyswitch(cherry, tactile, F("Grey"), 80, 20);
  look[3][4] = Keyswitch(kailh, linear, F("Speed Silver"), 40, 11);
  look[4][4] = Keyswitch(kailh, tactile, F("Speed Copper"), 40, 11);
  initswitched=true;
}

bool en1 = false;
bool en2 = false;
void hotprocdown(short col, short row) {
  if (col == 0 && row == 4) {
    en1 = false;
  }
  if (col == 4 && row == 0) {
    en2 = false;
  }
}

void hotprocup(short col, short row) {
  if (col == 0 && row == 4) {
    en1 = true;
  }
  if (col == 4 && row == 0) {
    en2 = true;
  }
  if (en1 && en2) {
    hottoggle = !hottoggle;
  }
}

void hotproc(short col, short row) {
  if (!initswitched) {
    initswitch();
  }
  look[col][row].write();
}

#endif // __MATRIX_H__
