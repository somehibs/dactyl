struct Overlay {
  char keymap[MATRIX_COUNT][MAX_COLS][MAX_ROWS];
};

#define M_RIGHT_MAIN 0

void clearOverlay(Overlay* o) {
  memset(o, 0, sizeof(Overlay));
}

Overlay* overlay_right(Overlay* a) {
  clearOverlay(a);
  a->keymap[M_RIGHT_MAIN][0][0] = KEY_F2;
  return a;
}
