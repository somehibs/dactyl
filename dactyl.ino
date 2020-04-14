// useful if you have more than one board.
// intentionally defined to allow users to use only the arduino and remove any code/memory usage from using mcp io expander
#define IO_EXPANDER

// debug flag, call log() as you would serial.printf
// when debug is undefined, log() will turn into an empty inline method and the compiler will remove 3kb of debug code+strings from memory
//#define DEBUG true
#ifdef DEBUG
// features that depend on debug to work anyway
// key releases
#define DEBUG_RELEASE
// for debugging how long it takes to run each loop
#define LOOPTIMER
#endif // DEBUG

// wdt_enable in loop() will prevent the system from being reprogrammed without a reset
// this is useful for production applications but can be annoying when debugging
// when enabled, removing the connection between two boards when the io expander is enabled can be used to halt the board in a startup loop
// debug disables this to make it easy to program
// remember not to spend more than 250ms thinking in the processing loop
#ifndef DEBUG
#define WATCHDOG_ENABLED
#endif // DEBUG

// enable wpm counting and logging - has a separate buffer to debug
//#define WPM

// comment out to disable keyboard actions. keyboard header still necessary for various key definitions
#define REAL_KEYBOARD

// max matrixes to include
const short MATRIX_COUNT = 4;
// current firmware version
const char* VERSION = "1.1.0.4";

#ifdef WATCHDOG_ENABLED
#include <avr/wdt.h>
#endif
#include <Keyboard.h>

// shared debug code
#ifdef DEBUG
char* debugBuffer = new char[128];
void log(const __FlashStringHelper* fmt, ...) {
  if (!fmt) return;
  va_list argp;
  va_start(argp, fmt);
  vsnprintf_P(debugBuffer, 128, (PGM_P)fmt, argp);
  va_end(argp);
  Serial.println(debugBuffer);
}
// memory layout on this is a little more predictable
int freeMemory() {
  char* heapStack = malloc(8);
  int f = (int)&heapStack - (int)heapStack;
  free(heapStack);
  log(F("free: %d"), f);
  return f;
}
#else
inline void log(const __FlashStringHelper* _, ...) {}
#endif

#include "matrix.h"
#include "macro.h"
#include "matrix_dactyl.h"

Matrix matricies[MATRIX_COUNT];
Overlay myOverlays[1];

void setup() {
#ifdef WATCHDOG_ENABLED
  MCUSR = 0;
  wdt_disable();
#endif // WATCHDOG_ENABLED
#ifdef IO_EXPANDER
  Serial.begin(9600);
  while (!ioexp.begin()) {
    Serial.print(F("Could not init left deck (FW: "));
    Serial.write(VERSION);
    Serial.println(")");
    delay(750);
  }
#else
#ifdef WATCHDOG_ENABLED
  delay(10000); // wait ten seconds instead
#endif // WATCHDOG_ENABLED
#endif // IO_EXPANDER
#ifdef REAL_KEYBOARD
  Keyboard.begin();
#endif // REAL_KEYBOARD
  init_matrix();
  init_overlay();
  init_macros();
  Serial.println(F("Startup complete."));
}

void init_matrix() {
  matricies[M_RIGHT_MAIN] = main_matrix_right();
  matricies[M_RIGHT_THUMB] = thumb_matrix_right();
  matricies[M_LEFT_MAIN] = main_matrix_left();
  matricies[M_LEFT_THUMB] = thumb_matrix_left();
  for (int i = 0; i < MATRIX_COUNT; i++) {
    Matrix* m = matricies + i;
    configure_matrix(m);
  }
}

void init_overlay() {
  overlay_main(myOverlays);
  overlays = myOverlays;
}

void init_macros() {
  memset(macros, 0, sizeof(Macro)*MACRO_COUNT);
  macros[0].seq = malloc(sizeof(char)*22);
  sprintf_P(macros[0].seq, PSTR("r35k%cdsre r2k%cdsre k%c"), KEY_F11, KEY_DOWN_ARROW, KEY_RETURN);
  //lentest(macros);
  macros[1].seq = malloc(sizeof(char)*7);
  sprintf_P(macros[1].seq, PSTR("h%ck%cu%c"), KEY_LEFT_ALT, KEY_F2, KEY_LEFT_ALT);
  //lentest(macros+1);
}

#ifdef LOOPTIMER
unsigned long lastLoop = 0;
unsigned long lastReport = 0;
void idlecheck() {
  unsigned long ms = millis();
  if (ms - lastReport > 1000) {
    lastReport = ms;
    freeMemory();
    log(F("loop: %d"), (ms - lastLoop));
  }
  lastLoop = ms;
}
#endif // LOOPTIMER

void loop() {
#ifdef WATCHDOG_ENABLED
  wdt_enable(WDTO_250MS); // col/row process shouldn't take longer than 250ms
#endif // WATCHDOG_ENABLED
  for (short xi = 0; xi < MATRIX_COUNT; ++xi) {
    process(matricies + xi, xi);
  }
#ifdef LOOPTIMER
  idlecheck();
#endif // LOOPTIMER
#ifdef WPM
  cpmproc();
#endif // WPM
}
