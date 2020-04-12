
// wdt_enable in loop() will prevent the system from being reprogrammed without a reset
// this is useful for production applications but can be annoying when debugging
// when enabled, removing the connection between two boards when the io expander is enabled can be used to halt the board in a startup loop
// 
//#define WATCHDOG_ENABLED

// useful if you have more than one board.
// intentionally defined to allow users to use only the arduino and remove any code/memory usage from using mcp io expander
#define IO_EXPANDER

// debug flag, call log() as you would sprintf serial.println
// when debug is undefined, log() will turn into an empty inline method and the compiler will remove 3kb of debug code+strings from memory
//#define DEBUG true
//#define DEBUG_RELEASE

// enable wpm counting and logging - has a separate buffer to debug
#define WPM

// comment out to disable keyboard actions. keyboard header still necessary for various key definitions
#define REAL_KEYBOARD

// max matrixes to include
const short MATRIX_COUNT = 4;

// current firmware version
const char* VERSION = "1.0.3.4";

#ifdef WATCHDOG_ENABLED
  #include <avr/wdt.h>
#endif
#include <Keyboard.h>

// shared debug code
#ifdef DEBUG
char* debugBuffer = new char[64];
void log(const __FlashStringHelper* fmt, ...) {
  if (!fmt) return;
  va_list argp;
  va_start(argp, fmt);
  vsprintf(debugBuffer, (PGM_P)fmt, argp);
  va_end(argp);
  Serial.println(debugBuffer);
}
#else
inline void log(const __FlashStringHelper* _, ...) {
  // Do nothing
}
#endif

#ifdef WPM
void writeWpm(char* fmt, ...) {
  char* debugBuffer = new char[64];
  va_list argp;
  va_start(argp, fmt);
  vsprintf_P(debugBuffer, fmt, argp);
  va_end(argp);
  Serial.println(debugBuffer);
}
#endif

#include "macro.h"
#include "matrix.h"
#include "matrix_dactyl.h"

Matrix matricies[MATRIX_COUNT];
Overlay myOverlays[1];

void setup() {
#ifdef WATCHDOG_ENABLED
  MCUSR=0;
  wdt_disable();
#endif // WATCHDOG_ENABLED
  Serial.begin(9600);
#ifdef IO_EXPANDER
  while (!ioexp.begin()) {
    Serial.write("Could not init left deck (FW: ");
    Serial.write(VERSION);
    Serial.println(")");
    delay(750);
  }
#else
#ifdef WATCHDOG_ENABLED
  delay(10000); // wait ten seconds instead
#endif // WATCHDOG_ENABLED
#endif // IO_EXPANDER
  Serial.write("FW: ");
  Serial.println(VERSION);
#ifdef REAL_KEYBOARD
  Keyboard.begin();
#endif // REAL_KEYBOARD
  init_matrix();
  init_overlay();
  init_macros();
  Serial.println("Startup complete.");
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

void loop() {
#ifdef WATCHDOG_ENABLED
  wdt_enable(WDTO_250MS); // col/row process shouldn't take longer than 250ms
#endif // WATCHDOG_ENABLED
  for (short xi = 0; xi < MATRIX_COUNT; ++xi) {
    process(matricies+xi, xi);
  }
}
