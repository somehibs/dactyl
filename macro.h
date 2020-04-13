#ifndef __MACRO_H__
#define __MACRO_H__

#include "matrix.h"

// example macro encoding format
// r5 kn sd re k78 k78 k78 k78
// r, h, u, d, ` and k are reserved
// still a big address space left!
// r5 = repeat next macros five times
// re = repeat end
// kn = press key n - note this must be a single char!
// ds = short delay, 500ms
// ` = starts and ends a string that should be printed to the remote terminal
// hn = hold key n - note this must be a single char!
// un == release key n - note this must be a single char! - should be improved to remove next character and pop the release key off the stack
// space is skipped - you can safely omit them if you want more dynamic memory space

// reserved but not implemented
// d1 = delay for 1s - warning, change wdt if fixing this, consider a millis() based solution

// this code is unsafe - incorrect macro sequences may cause your microcontroller to crash doing an illegal operation
// repeat sets cannot be nested - i.e. r5 r5 k65 re re       will not work
// repeat sets can be repeated - i.e. r5 k65 re r5 k65 re    will repeat 65 10 times ok

class Macro {
public:
  // Pointer to static char* which is encoded commands
  char* seq;
  void execute();
private:
  short resumePosition;
  short delayLength;
  void executeMacroStr(char* macro, int len);
};

#define MACRO_COUNT 2
Macro macros[MACRO_COUNT];

void lentest(Macro* m) {
  log(F("Macro len %d macro: %s"), strlen(m->seq)+1, m->seq);
}

//#include <avr/pgmspace.h>
//const char fst[] PROGMEM = ;
//const char *const string_table[] PROGMEM = {fst};
//  char* fduff = malloc(sizeof(char)*28);
//  strcpy_P(fduff, (char *)pgm_read_word(&(string_table[0])));  // Necessary casts and dereferencing, just copy.

void init_macros() {
  delay(4000);
  memset(macros, 0, sizeof(Macro)*MACRO_COUNT);
  macros[0].seq = malloc(sizeof(char)*28);
  sprintf_P(macros[0].seq, PSTR("r35 k%c ds re r2 k%c ds re k%c"), KEY_F11, KEY_DOWN_ARROW, KEY_RETURN);
  //lentest(macros);
  macros[1].seq = malloc(sizeof(char)*20);
  sprintf_P(macros[1].seq, PSTR("h%c k%c u%c"), KEY_LEFT_ALT, KEY_F2, KEY_LEFT_ALT);
  //lentest(macros+1);
}

void Macro::execute() {
  int len = strlen(this->seq);
  this->executeMacroStr(this->seq, len);
}

int countUntil(char* sectionStart, int len, char sectionChar) {
  for (int i = 0; i < len; ++i) {
    if (sectionStart[i] == sectionChar) {
      return i;
    }
  }
  return 1;
}

void Macro::executeMacroStr(char* macro, int len) {
  short repeatModeStart = 0;
  short repeatModeCounter = 0;
  for (short i = 0; i < len; ++i) {
#ifdef WATCHDOG_ENABLED
  wdt_enable(WDTO_1S); // single loop shouldn't be longer than 1s
#endif // WATCHDOG_ENABLED
    unsigned char cmd = macro[i];
    if (cmd == ' ') {
      continue;
    }
    unsigned char next = macro[i+1];
    if (cmd == 'r') {
      if (next == 'e') {
        // repeat end, execute repeatModeStart
        log(F("finishing repeat mode for %d times at position %d"), repeatModeCounter, i);
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
        log(F("entering repeat mode for %d times at position %d"), repeatModeCounter, repeatModeStart);
      }
    }
    if (repeatModeStart != 0 && repeatModeCounter != 0) {
      // suppress commands until repeat mode is handled
      continue;
    }
    if (cmd == 'k') {
      // Push the next char as a key
      log(F("pushing key %c (d: %d)"), next, next);
#ifdef REAL_KEYBOARD
      Keyboard.write(next);
#endif // REAL_KEYBOARD
      i += 1;
    } else if (cmd == 'd') {
      if (next == 's') {
        delay(250);
      }
      i += 1;
    } else if (cmd == 'h') {
      log(F("hold key %c (d: %d)"), next, next);
#ifdef REAL_KEYBOARD
      Keyboard.press(next);
#endif // REAL_KEYBOARD
      i += 1;
    } else if (cmd == 'u') {
      log(F("release key %c (d: %d)"), next, next);
#ifdef REAL_KEYBOARD
      Keyboard.release(next);
#endif // REAL_KEYBOARD
      i += 1;
    } else if (cmd == '`') {
      int seqLen = countUntil(macro+i+1, len-1-i, '`');
      log(F("Sequence should be %d chars"), seqLen);
      Keyboard.write(macro+i+1, seqLen);
      i = i+seqLen+1;
    } else {
      log(F("unprocessed command %c (next: %c)"), cmd, next);
    }
  }
}

#endif // __MACRO_H__
