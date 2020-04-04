struct Overlay {
  unsigned char keymap[MATRIX_COUNT][MAX_COLS][MAX_ROWS];
};

#define M_RIGHT_MAIN 0
#define M_LEFT_MAIN 2

void clearOverlay(Overlay* o) {
  memset(o, 0, sizeof(Overlay));
}

Overlay* overlay_main(Overlay* a) {
  clearOverlay(a);
  // top row
  a->keymap[M_RIGHT_MAIN][0][0] = KEY_F6;
  a->keymap[M_RIGHT_MAIN][1][0] = KEY_F7;
  a->keymap[M_RIGHT_MAIN][2][0] = KEY_F8;
  a->keymap[M_RIGHT_MAIN][3][0] = KEY_F9;
  a->keymap[M_RIGHT_MAIN][4][0] = KEY_F10;
  a->keymap[M_RIGHT_MAIN][5][0] = KEY_F11;
  a->keymap[M_RIGHT_MAIN][5][1] = KEY_F12;
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
  // top row
  a->keymap[M_LEFT_MAIN][0][0] = '`';
  a->keymap[M_LEFT_MAIN][1][0] = KEY_F1;
  a->keymap[M_LEFT_MAIN][2][0] = KEY_F2;
  a->keymap[M_LEFT_MAIN][3][0] = KEY_F3;
  a->keymap[M_LEFT_MAIN][4][0] = KEY_F4;
  a->keymap[M_LEFT_MAIN][5][0] = KEY_F5;
  a->keymap[M_LEFT_MAIN][5][1] = '`';
  // 6 floating keys
  a->keymap[M_LEFT_MAIN][2][1] = KEY_INSERT;
  a->keymap[M_LEFT_MAIN][3][1] = KEY_HOME;
  a->keymap[M_LEFT_MAIN][4][1] = KEY_PAGE_UP;
  a->keymap[M_LEFT_MAIN][2][2] = KEY_DELETE;
  a->keymap[M_LEFT_MAIN][3][2] = KEY_END;
  a->keymap[M_LEFT_MAIN][4][2] = KEY_PAGE_DOWN;
  // reverse arrow keys
  a->keymap[M_LEFT_MAIN][2][3] = KEY_LEFT_ARROW;
  a->keymap[M_LEFT_MAIN][3][3] = KEY_DOWN_ARROW;
  a->keymap[M_LEFT_MAIN][4][3] = KEY_UP_ARROW;
  a->keymap[M_LEFT_MAIN][5][3] = KEY_RIGHT_ARROW;
  return a;
}
