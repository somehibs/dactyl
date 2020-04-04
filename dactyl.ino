
// caveats
// cannot send 0 because it's used internally as 'no key assigned'

#include <Keyboard.h>
const short MATRIX_COUNT = 4;
#include "matrix.h"
#include "matrix_def.h"

#define RXLED = 17;
#define TXLED = 30;

const char* VERSION = "1.0.0.6";
Matrix matricies[MATRIX_COUNT];

void safetyDelay() {
  Serial.println("Beginning startup safety delay.");
  for (int i = 15; i > 0; --i) {
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
    if (matricies[i].remote) {
      Matrix* m = matricies + i;
      // clear the inversion registers (you might need to edit the adafruit library)
      ioexp.writeRegister(MCP23017_IPOLA, 0x0);
      ioexp.writeRegister(MCP23017_IPOLB, 0x0);
      for (int j = 0; j < m->rowCount; ++j) {
        
      }
    }
  }
}

Overlay myOverlays[1];

void init_overlay() {
  overlay_main(myOverlays);
  overlays = myOverlays;
}

void loop() {
  for (short xi = 0; xi < MATRIX_COUNT; ++xi) {
    process(matricies+xi, xi);
    //halter();
  }
}

void halter() {
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
}
