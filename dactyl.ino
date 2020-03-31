#include <Keyboard.h>
const short MATRIX_COUNT = 4;
#include "matrix.h"
#include "matrix_def.h"

#define RXLED = 17;
#define TXLED = 30;

Matrix matricies[MATRIX_COUNT];

void setup() {
  delay(10000);
  ioexp.begin();
  Serial.begin(9600);
  Serial.write("FW: ");
  Serial.println(VERSION);
  Keyboard.begin();
  init_matrix();
  init_overlay();
  Serial.println("Startup complete.");
  delay(1000);
}

void init_matrix() {
  matricies[0] = main_matrix_right();
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
    process(matricies+xi);
  }
}
