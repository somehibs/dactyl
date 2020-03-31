struct Overlay {
  unsigned char keymap[MATRIX_COUNT][MAX_COLS][MAX_ROWS];
};

#define M_RIGHT_MAIN 0

void clearOverlay(Overlay* o) {
  memset(o, 0, sizeof(Overlay));
}

Overlay* overlay_right(Overlay* a) {
  clearOverlay(a);
  // top row
  a->keymap[M_RIGHT_MAIN][0][0] = KEY_F7;
  a->keymap[M_RIGHT_MAIN][1][0] = KEY_F8;
  a->keymap[M_RIGHT_MAIN][2][0] = KEY_F9;
  a->keymap[M_RIGHT_MAIN][3][0] = KEY_F10;
  a->keymap[M_RIGHT_MAIN][4][0] = KEY_F11;
  a->keymap[M_RIGHT_MAIN][5][0] = KEY_F12;
  // 6 floating keys
  a->keymap[M_RIGHT_MAIN][1][1] = KEY_INSERT;
  a->keymap[M_RIGHT_MAIN][2][1] = KEY_HOME;
  a->keymap[M_RIGHT_MAIN][3][1] = KEY_PAGE_UP;
  a->keymap[M_RIGHT_MAIN][1][2] = KEY_DELETE;
  a->keymap[M_RIGHT_MAIN][2][2] = KEY_END;
  a->keymap[M_RIGHT_MAIN][3][2] = KEY_PAGE_DOWN;
  // reverse arrow keys
  a->keymap[M_RIGHT_MAIN][1][3] = KEY_LEFT_ARROW;
  a->keymap[M_RIGHT_MAIN][2][3] = KEY_DOWN_ARROW;
  a->keymap[M_RIGHT_MAIN][3][3] = KEY_UP_ARROW;
  a->keymap[M_RIGHT_MAIN][4][3] = KEY_RIGHT_ARROW;
  return a;
}
