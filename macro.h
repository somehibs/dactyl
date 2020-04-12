#ifndef __MACRO_H__
#define __MACRO_H__

#include "matrix.h"

class Macro {
public:
  // Pointer to static char* which is encoded commands
  char* seq;
  void execute();
private:
  void executeMacroStr(char* macro, int len);
};

// example macro encoding format
// r5 kn sd re k78 k78 k78 k78
// r, d and k are reserved
// r5 = repeat next macros five times
// re = repeat end
// kn = press key n - note this must be a single char!
// ds = short delay, 500ms

// reserved but not implemented
// d1 = delay for 1s
// ` = starts and ends a string that should be printed to the remote terminal
// space is skipped - you can safely omit them if you want more dynamic memory space
// khn = hold key n - note this must be a single char!
// kun == unhold key n - note this must be a single char!
// the char gives a command space of 255 and with arguments this should last a simple macro framework for a while

// repeat sets cannot be nested - i.e. r5 r5 k65 re re will not work

#define MACRO_COUNT 1
Macro macros[MACRO_COUNT];

void lentest(char* c) {
  log("Macro len %d macro: %s", strlen(c), c);
}

void init_macros() {
  delay(4000);
  char* duffer = malloc(sizeof(char)*28);
  sprintf(duffer, "r10 k%c ds re r3 k%c ds re k%c", KEY_F11, KEY_DOWN_ARROW, KEY_RETURN);
  lentest(duffer);
  macros[0].seq = duffer;
  macros[0].execute();
}

void Macro::execute() {
  int len = strlen(this->seq);
  this->executeMacroStr(this->seq, len);
}

void Macro::executeMacroStr(char* macro, int len) {
  short repeatModeStart = 0;
  short repeatModeCounter = 0;
  log("execseq %s len %d", macro, len);
  for (short i = 0; i < len; ++i) {
    unsigned char cmd = macro[i];
    if (cmd == ' ') {
      continue;
    }
    unsigned char next = macro[i+1];
    if (cmd == 'r') {
      if (next == 'e') {
        // repeat end, execute repeatModeStart
        log("finishing repeat mode for %d times at position %d", repeatModeCounter, i);
        for (int j = 0; j < repeatModeCounter; ++j) {
          this->executeMacroStr(this->seq+repeatModeStart, i-repeatModeStart);
        }
        i += 1;
        repeatModeStart = 0;
        repeatModeCounter = 0;
        continue;
      } else {
        // repeat start and counter
        repeatModeCounter = atoi(macro+i+1);
        repeatModeStart = i+2;
        if (repeatModeCounter > 9) {
          repeatModeStart += 1;
        }
        if (repeatModeCounter > 99) {
          repeatModeStart += 1;
        }
        i += 1;
        log("entering repeat mode for %d times at position %d", repeatModeCounter, repeatModeStart);
      }
    }
    if (repeatModeStart != 0 && repeatModeCounter != 0) {
      // suppress commands until repeat mode is handled
      continue;
    }
    log("processing command %c (next: %c)", cmd, next);
    if (cmd == 'k') {
      // Push the next char as a key
      log("pushing key %c (d: %d)", next, next);
#ifdef REAL_KEYBOARD
      Keyboard.write(next);
#endif // REAL_KEYBOARD
      i += 1;
    } else if (cmd == 'd') {
      if (next == 's') {
        delay(500);
      }
      i += 1;
    }
  }
}

#endif // __MACRO_H__
