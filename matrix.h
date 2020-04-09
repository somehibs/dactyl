#define REAL_KEYBOARD

const short MAX_COLS = 6;
const short MAX_ROWS = 5;
const short MAGIC_DEBOUNCE_NUMBER = 0;

const short KEY_OVERLAY_1 = 1;

//#define DEBUG true
char* debugBuffer = new char[64];

#include "overlay.h"

struct Matrix {
  char columns[MAX_COLS];
  char rows[MAX_ROWS];
  unsigned char keymap[MAX_COLS][MAX_ROWS];
  short keyset[MAX_COLS][MAX_ROWS];
  char overlayPressed[MAX_COLS][MAX_ROWS];
  short columnCount;
  short rowCount;
  const char* name;
  bool rowDriven;
  // at the other end of MCP23017, use ioexp instead of digitalWrite directly
  bool remote;
};

Overlay* overlays = 0;
short enableOverlay = 0;

#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 ioexp;

void clearMatrix(Matrix* a) {
  memset(a, 0, sizeof(Matrix));
  memset(a->columns, -1, sizeof(short)*MAX_COLS);
  memset(a->rows, -1, sizeof(short)*MAX_ROWS);
  // row driven by default
  a->rowDriven = true;
}

void configure_matrix(Matrix* m) {
  // configure the read pins as pullup high to prevent floating
  for (int i = 0; i < (m->rowDriven ? m->columnCount : m->rowCount); ++i) {
    short pin = m->rowDriven ? m->columns[i] : m->rows[i];
    if (pin != -1) {
      if (m->remote) {
        ioexp.pullUp(pin, HIGH);
      } else {
        pinMode(pin, INPUT_PULLUP);
      }
    }
  }
}

#ifdef DEBUG
void log(char* fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vsprintf(debugBuffer, fmt, argp);
  va_end(argp);
  Serial.println(debugBuffer);
}
#else
inline void log(char* _, ...) {
  // Do nothing
}
#endif


void pressKeyImpl(char key) {
#ifdef REAL_KEYBOARD
  Keyboard.press(key);
#else
  log("Key pressed: %c (%d)", key, key);
#endif
}

void pressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (enableOverlay != 0) {
    // if this isnt handled, let the original key go through
    unsigned char overlayKey = overlays[enableOverlay - 1].keymap[matrixIndex][col][row];
    if (overlayKey > 0) {
      log("overlay: key: %d %d, %d for matrix %s", overlayKey, col, row, m->name);
      m->overlayPressed[col][row] = enableOverlay;
      pressKeyImpl(overlayKey);
      return;
    } else {
      log("no overlay (%d enabled): %d, %d overlaykey %d for matrix %s", enableOverlay, col, row, overlayKey, m->name);
      //return; // suppress fallthrough keypresses when the overlay isn't bound to fix confusing keypress issues
    }
  }
  unsigned char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    // enable this overlay
    enableOverlay = key;
    log("enable overlay %d", enableOverlay);
    return;
  }
  pressKeyImpl(key);
}

void unpressKeyImpl(char key) {
#ifdef REAL_KEYBOARD
  Keyboard.release(key);
#else
  Serial.write("Key unpressed: ");
  Serial.println(key);
#endif
}


#ifdef WATCHDOG_ENABLED
#include <avr/wdt.h>
#endif
void remoteIoFailed() {
  Serial.write("Bad remote IO call reply");
#ifdef WATCHDOG_ENABLED
  Serial.println(" system halt!");
  wdt_enable(WDTO_15MS);
  while(1) {
    delay(1);
  }
#else
  Serial.println(" watchdog disabled - no action");
#endif
}

void unpressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (m->overlayPressed[col][row] != 0) {
    // unpress overlay key instead
    short overlay = m->overlayPressed[col][row];
    unsigned char overlayKey = overlays[overlay - 1].keymap[matrixIndex][col][row];
    unpressKeyImpl(overlayKey);
    m->overlayPressed[col][row] = 0;
    m->keyset[col][row] = 0;
    return;
  }
  unsigned char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    log("disable overlay %d", enableOverlay);
    enableOverlay = 0;
  } else if (key == 0) {
    log("key missing %d %d mx %s", col, row, m->name);
  } else {
    unpressKeyImpl(key);
  }
  m->keyset[col][row] = 0;
}

// saves passing an arg around
uint16_t allRows = 0;
inline bool readPin(short pin, bool remote) {
  if (!remote) {
    //return !ioexp.digitalRead(low[i]); // deprecated in favour of faster all-row read method
    return !((allRows >> pin) & 0x1);
  } else {
    return !digitalRead(pin);
  }
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
    log("High %d missing from matrix %s", index, m->name);
    return;
  }

  // Set high
  if (!m->remote) {
    pinMode(high[index], OUTPUT);
    digitalWrite(high[index], LOW);
  } else {
    bool reply = ioexp.pinMode(high[index], OUTPUT);
    if (!reply) {
      remoteIoFailed();
    }
    ioexp.digitalWrite(high[index], LOW);
  }
  
  // Prepare to read rows and act on col/row lookups
  short column = index;
  short row = index; // will be corrected in loop
  if (m->remote) {
    // read all the pins now
    allRows = ioexp.readGPIOAB();
  }
  for (short i = 0; i < (m->rowDriven ? m->columnCount : m->rowCount); ++i) {
    if (low[i] == -1) {
      continue;
    }

    // Read the low pin
    bool read = readPin(low[i], m->remote);

    // Process the read result
    if (m->rowDriven) {
      column = i;
    } else {
      row = i;
    }
    if (read) {
      // Key is held, add to keyset value
      m->keyset[column][row] += 1;
      if (m->keyset[column][row] == 32766) {
        // nearly at short wraparound, jump back to 2
        m->keyset[column][row] = 2;
      }
      if (m->keymap[column][row] != 0 && m->keyset[column][row] == 1) {
        log("keypress: %d, %d allread %u for matrix %s", column, row, allRows, m->name);
        pressKey(m, matrixIndex, column, row);
      } else if (m->keymap[column][row] == 0) {
        log("Could not read key for col %d and row %d for matrix %s", column, row, m->name);
      } else {
#ifdef VERBOSE
        log("pos %d %d c %d mx %s", column, row, m->keyset[column][row], m->name);
#endif
      }
      // Only unpress a key if it's been held long enough (swap this if keys ghost a lot)
    } else if ((m->keyset[column][row]>0&&m->remote) || (m->keyset[column][row] > MAGIC_DEBOUNCE_NUMBER)) {
      unpressKey(m, matrixIndex, column, row);
    }
  }
  // Set column low
  if (!m->remote) {
    pinMode(high[index], INPUT);
  } else {
    ioexp.pinMode(high[index], INPUT);
  }
}

// Process each row/column
void process(Matrix* m, short matrixIndex) {
  short matrixMax = m->rowDriven ? m->rowCount : m->columnCount;
  for (short i = 0; i < matrixMax; ++i) {
    processOne(m, matrixIndex, i);
  }
}
