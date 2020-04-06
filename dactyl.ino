// caveats
// cannot send 0 because it's used internally as 'no key assigned'

#include <Keyboard.h>
const short MATRIX_COUNT = 4;
#include "matrix.h"
#include "matrix_def.h"

#define RXLED = 17;
#define TXLED = 30;

const char* VERSION = "1.0.0.9";
Matrix matricies[MATRIX_COUNT];

void safetyDelay() {
  Serial.println("Beginning startup safety delay.");
  for (int i = 12; i > 0; --i) {
    Serial.println("Wait a sec...");
    delay(1000);
  }
  Serial.println("Safety delay complete. Starting...");
}

void setup() {
  Serial.begin(9600);
  //safetyDelay();
  while (!ioexp.begin()) {
    // Malfunction starting second half of keyboard
    Serial.println("Could not init left deck");
    delay(750);
  }
  Serial.write("FW: ");
  Serial.println(VERSION);
  #ifdef REAL_KEYBOARD
  delay(500); // windows keyboard corruption startup delay
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
    // configure the read pins as pullup high to prevent floating
    for (int j = 0; j < (m->rowDriven ? m->columnCount : m->rowCount); ++j) {
      short pin = m->rowDriven ? m->columns[j] : m->rows[j];
      if (pin != -1) {
        if (m->remote) {
          ioexp.pullUp(pin, HIGH);
        } else {
          pinMode(pin, INPUT_PULLUP);
          //pullUp(pin, HIGH);
        }
      }
    }
    if (matricies[i].remote) {
      // clear the inversion registers (you might need to edit the adafruit library)
      ioexp.writeRegister(MCP23017_IPOLA, 0x00);
      ioexp.writeRegister(MCP23017_IPOLB, 0x00);
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
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
  delay(100);
}
