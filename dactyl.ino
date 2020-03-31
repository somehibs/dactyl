// caveats
// cannot send 0 because it's used internally as 'no key assigned'

#include <Keyboard.h>
const short MATRIX_COUNT = 4;
#include "matrix.h"
#include "matrix_def.h"

#define RXLED = 17;
#define TXLED = 30;

const char* VERSION = "0.0.0.5";
Matrix matricies[MATRIX_COUNT];

void safetyDelay() {
  Serial.println("Beginning startup safety delay.");
  for (int i = 10; i > 0; --i) {
    Serial.println("Wait a sec...");
    delay(1000);
  }
  Serial.println("Safety delay complete. Starting...");
}

void setup() {
  Serial.begin(9600);
  safetyDelay();
  ioexp.begin();
  Serial.write("FW: ");
  Serial.println(VERSION);
  Keyboard.begin();
  init_matrix();
  init_overlay();
  Serial.println("Startup complete.");
}

void init_matrix() {
  matricies[M_RIGHT_MAIN] = main_matrix_right();
  matricies[1] = thumb_matrix_right();
  matricies[2] = main_matrix_left();
  matricies[3] = thumb_matrix_left();
}

Overlay myOverlays[1];

void init_overlay() {
  overlay_right(myOverlays);
  overlays = myOverlays;
}

void loop() {
  for (short xi = 0; xi < MATRIX_COUNT; ++xi) {
    process(matricies+xi, xi);
  }
}
