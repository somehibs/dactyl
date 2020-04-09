// caveats
// cannot send 0 because it's used internally as 'no key assigned'
#define WATCHDOG_ENABLED

#include <Keyboard.h>

const short MATRIX_COUNT = 4;
#include "matrix.h"
#include "matrix_def.h"

// watchdog related - warning, need to reflash atmega32u4 firmware before enabling
#ifdef WATCHDOG_ENABLED
#include <avr/wdt.h>
#endif

#define RXLED 17
#define TXLED 30

const char* VERSION = "1.0.3.2";
Matrix matricies[MATRIX_COUNT];

void safetyDelay() {
  Serial.println("Beginning startup safety delay.");
  for (int i = 14; i > 0; --i) {
    Serial.write("Wait a sec...");
    Serial.write(" FW: ");
    Serial.println(VERSION);
    delay(1000);
  }
  Serial.println("Safety delay complete. Starting...");
}

void setup() {
#ifdef WATCHDOG_ENABLED
  MCUSR=0;
  wdt_disable();
#endif // WATCHDOG_ENABLED
  Serial.begin(9600);
  //safetyDelay();
  // May be used instead of safety delay to delay startup - useful now that a watchdog timer prevents reflashing otherwise
  while (!ioexp.begin()) {
    Serial.write("Could not init left deck (FW: ");
    Serial.write(VERSION);
    Serial.println(")");
    delay(750);
  }
  Serial.write("FW: ");
  Serial.println(VERSION);
#ifdef REAL_KEYBOARD
  Keyboard.begin();
#endif
  init_matrix();
  init_overlay();
  Serial.println("Startup complete.");
}

void init_matrix() {
  matricies[M_RIGHT_MAIN] = main_matrix_right();
  matricies[1] = thumb_matrix_right();
  matricies[M_LEFT_MAIN] = main_matrix_left();
  matricies[3] = thumb_matrix_left();
  for (int i = 0; i < MATRIX_COUNT; i++) {
    Matrix* m = matricies + i;
    configure_matrix(m);
  }
}

Overlay myOverlays[1];

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
