#define REAL_KEYBOARD

// if memory becomes a constraint, make the cols, rows, keymap and keyset dynamic to reclaim space from small matrixes
const short MAX_COLS = 6;
const short MAX_ROWS = 5;
const short MAGIC_DEBOUNCE_NUMBER = 3;

const short KEY_OVERLAY_1 = 1;

#define DEBUG true
 #ifdef DEBUG
char* debugBuffer = new char[512];
#else
char* debugBuffer = 0;
#endif

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

void log(char* logLine) {
  if (!DEBUG) {
    return;
  }
  Serial.println(logLine);
}


void pressKeyImpl(char key) {
  #ifdef REAL_KEYBOARD
    Keyboard.press(key);
  #else
    Serial.write("Key pressed: ");
    Serial.println(key);
  #endif
}

void pressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (enableOverlay != 0) {
    // if this isnt handled, let the original key go through
    unsigned char overlayKey = overlays[enableOverlay-1].keymap[matrixIndex][col][row];
    if (overlayKey > 0) {
#ifdef DEBUG
sprintf(debugBuffer, "overlay: key: %d %d, %d for matrix %s", overlayKey, col, row, m->name);
log(debugBuffer);
#endif
      m->overlayPressed[col][row] = enableOverlay;
      pressKeyImpl(overlayKey);
      return;
    } else {
#ifdef DEBUG
sprintf(debugBuffer, "no overlay (%d enabled): %d, %d overlaykey %d for matrix %s", enableOverlay, col, row, overlayKey, m->name);
log(debugBuffer);
#endif
      return; // suppress fallthrough keypresses when the overlay isn't bound to fix confusing keypress issues
    }
  }
  char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    // enable this overlay
    enableOverlay = key;
#ifdef DEBUG
sprintf(debugBuffer, "enable overlay %d", enableOverlay);
log(debugBuffer);
#endif
    return;
  }
  pressKeyImpl(key);
}

void unpressKeyImpl(char key) {
  #ifdef REAL_KEYBOARD
    Keyboard.release(key);
  #else
    Serial.write("Key pressed: ");
    Serial.println(key);
  #endif
}

void unpressKey(Matrix* m, short matrixIndex, short col, short row) {
  if (m->overlayPressed[col][row] != 0) {
    // unpress overlay key instead
    short overlay = m->overlayPressed[col][row];
    unsigned char overlayKey = overlays[overlay-1].keymap[matrixIndex][col][row];
    unpressKeyImpl(overlayKey);
    m->overlayPressed[col][row] = 0;
    m->keyset[col][row] = 0;
    return;
  }
  unsigned char key = m->keymap[col][row];
  if (key == KEY_OVERLAY_1) {
    // disable this overlay
#ifdef DEBUG
sprintf(debugBuffer, "disable overlay %d", enableOverlay);
log(debugBuffer);
#endif
    enableOverlay = 0;
  } else if (key == 0) {
#ifdef DEBUG
sprintf(debugBuffer, "key missing %d %d mx %s", col, row, m->name);
log(debugBuffer);
#endif
  } else {
    unpressKeyImpl(key);
  }
  m->keyset[col][row] = 0;
}

void processOne(Matrix* m, short matrixIndex, short index) {
  char* high = m->rows;
  char* low = m->columns;
  if (!m->rowDriven) {
    high = m->columns;
    low = m->rows;
  }
  
  if (high[index] == -1) {
#ifdef DEBUG
sprintf(debugBuffer, "High %d missing from matrix %s", index, m->name);
log(debugBuffer);
#endif
    return;
  }

  if (!m->remote) {
    pinMode(high[index], OUTPUT);
    digitalWrite(high[index], LOW);
  } else {
    ioexp.pinMode(high[index], OUTPUT);
    ioexp.digitalWrite(high[index], LOW);
  }
  // Read rows
  short column = index;
  short row = index; // will be corrected in loop
  for (short i = 0; i < (m->rowDriven ? m->columnCount : m->rowCount); ++i) {
    if (low[i] == -1) {
      continue;
    }

    // Read the low pin
    bool read = false;
    if (!m->remote) {
      pinMode(low[i], INPUT_PULLUP);
      bool notRead = digitalRead(low[i]);
      read = !notRead;
      pinMode(low[i], INPUT);
    } else {
      ioexp.pullUp(low[i], HIGH);
      bool notRead = ioexp.digitalRead(low[i]);
      read = !notRead;
      ioexp.pullUp(low[i], LOW);
    }

    // Process the read result
    if (m->rowDriven) {
      column = i;
    } else {
      row = i;
    }
    if (read) {
      m->keyset[column][row] += 1;
      if (m->keyset[column][row] == 32766) {
        // nearly at short wraparound, jump back to 2
        m->keyset[column][row] = 2;
      }
      if (m->keymap[column][row] != 0 && m->keyset[column][row] == 1) {
#ifdef DEBUG
sprintf(debugBuffer, "keypress: %d, %d for matrix %s", column, row, m->name);
log(debugBuffer);
        pressKey(m, matrixIndex, column, row);
#endif
      } else if (m->keymap[column][row] == 0) {
#ifdef DEBUG
sprintf(debugBuffer, "Could not read key for high %d and low %d for matrix %s", i, index, m->name);
log(debugBuffer);
#endif
      } else {
#ifdef VERBOSE
sprintf(debugBuffer, "pos %d %d c %d mx %s", column, row, m->keyset[column][row], m->name);
log(debugBuffer);
#endif
      }
    // Only unpress a key if it's been held long enough (swap this if keys ghost a lot)
    } else if (m->keyset[column][row] > MAGIC_DEBOUNCE_NUMBER) {
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

void process(Matrix* m, short matrixIndex) {
  short matrixMax = m->rowDriven ? m->rowCount : m->columnCount;
  for (short i = 0; i < matrixMax; ++i) {
    processOne(m, matrixIndex, i);
  }
}
