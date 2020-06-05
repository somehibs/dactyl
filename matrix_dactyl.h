Matrix test_board_matrix() {
  Matrix a;
  a.init("test_board", 5, 5);
  short offone = 1;
  a.rows[0] = 2; // TX0
  a.rows[1] = 3; // RX1
  a.rows[2] = 4;
  a.rows[3] = 5;
  a.rows[4] = 6;
  a.columns[0] = 10;
  a.columns[1] = 14;
  a.columns[2] = 15;
  a.columns[3] = 9;
  a.columns[4] = 8;
  a.keymap[0][0] = 'a';
  a.keymap[1][0] = 'b';
  a.keymap[2][0] = 'c';
  a.keymap[3][0] = 'd';
  a.keymap[4][0] = 'e'; // actually the space bar
  a.keymap[0][1] = 'f';
  a.keymap[1][1] = 'g';
  a.keymap[2][1] = ' ';
  a.keymap[3][1] = 'i'-offone;
  a.keymap[4][1] = 'j'-offone; // actually the space bar
  a.keymap[0][2] = 'k'-offone;
  a.keymap[1][2] = 'l'-offone;
  a.keymap[2][2] = 'm'-offone;
  a.keymap[3][2] = 'n'-offone;
  a.keymap[4][2] = 'o'-offone; // actually the space bar
  a.keymap[0][3] = 'p'-offone;
  a.keymap[1][3] = 'q'-offone;
  a.keymap[2][3] = 'r'-offone;
  a.keymap[3][3] = 's'-offone;
  a.keymap[4][3] = 't'-offone; // actually the space bar
  a.keymap[0][4] = 'u'-offone;
  a.keymap[1][4] = 'v'-offone-offone;
  a.keymap[2][4] = 'x'-offone-offone-offone;
  a.keymap[3][4] = 'y'-offone-offone-offone;
  a.keymap[4][4] = 'z'-offone-offone-offone; // actually the space bar
  return a;
}


Matrix main_matrix_right() {
  Matrix a;
  a.init("main_right", 6, 5);
  a.rows[0] = 1; // TX0
  a.rows[1] = 0; // RX1
  a.rows[2] = 4;
  a.rows[3] = 20;
  a.rows[4] = 19;
  a.columns[0] = 21;
  a.columns[1] = 5;
  a.columns[2] = 6;
  a.columns[3] = 7;
  a.columns[4] = 8;
  a.columns[5] = 9;
  a.keymap[0][0] = '6';
  a.keymap[0][1] = 'y';
  a.keymap[0][2] = 'h';
  a.keymap[0][3] = 'n';
  a.keymap[0][4] = ' '; // actually the space bar
  a.keymap[1][0] = '7';
  a.keymap[1][1] = 'u';
  a.keymap[1][2] = 'j';
  a.keymap[1][3] = 'm';
  a.keymap[1][4] = '[';
  a.keymap[2][0] = '8';
  a.keymap[2][1] = 'i';
  a.keymap[2][2] = 'k';
  a.keymap[2][3] = ',';
  a.keymap[2][4] = ']';
  a.keymap[3][0] = '9';
  a.keymap[3][1] = 'o';
  a.keymap[3][2] = 'l';
  a.keymap[3][3] = '.';
  a.keymap[3][4] = '=';
  a.keymap[4][0] = '0';
  a.keymap[4][1] = 'p';
  a.keymap[4][2] = ';';
  a.keymap[4][3] = '/';
  a.keymap[4][4] = KEY_PAGE_UP;
  a.keymap[5][0] = '-';
  a.keymap[5][1] = '\\';
  a.keymap[5][2] = '\'';
  a.keymap[5][3] = KEY_BACKSPACE;
  a.keymap[5][4] = KEY_PAGE_DOWN;
  return a;
}

Matrix thumb_matrix_right() {
  Matrix a;
  a.init("thumb_right", 3, 2);
  a.rows[0] = 16;
  a.rows[1] = 10;
  a.columns[0] = 18;
  a.columns[1] = 15;
  a.columns[2] = 14;
  a.keymap[0][0] = KEY_LEFT_GUI;//KEY_RIGHT_ALT;
  a.keymap[0][1] = KEY_OVERLAY_1;
  a.keymap[1][0] = KEY_RIGHT_SHIFT;
  a.keymap[1][1] = KEY_RETURN;
  a.keymap[2][0] = KEY_RIGHT_CTRL;
  return a;
}

Matrix main_matrix_left() {
  Matrix a;
  a.init("left_main", 6, 5);
  a.setRemote();
  a.rows[0] = 14;
  a.rows[1] = 13;
  a.rows[2] = 12;
  a.rows[3] = 11;
  a.rows[4] = 10;
  a.columns[0] = 0;
  a.columns[1] = 1;
  a.columns[2] = 2;
  a.columns[3] = 3;
  a.columns[4] = 4;
  a.columns[5] = 15;
  a.keymap[0][0] = '`';
  a.keymap[0][1] = KEY_TAB;
  a.keymap[0][2] = KEY_ESC;
  a.keymap[0][3] = KEY_LEFT_ALT;//KEY_LEFT_SHIFT;
  a.keymap[0][4] = KEY_LEFT_GUI;
  a.keymap[1][0] = '1';
  a.keymap[1][1] = 'q';
  a.keymap[1][2] = 'a';
  a.keymap[1][3] = 'z';
  a.keymap[1][4] = KEY_LEFT_ARROW;
  a.keymap[2][0] = '2';
  a.keymap[2][1] = 'w';
  a.keymap[2][2] = 's';
  a.keymap[2][3] = 'x';
  a.keymap[2][4] = KEY_DOWN_ARROW;
  a.keymap[3][0] = '3';
  a.keymap[3][1] = 'e';
  a.keymap[3][2] = 'd';
  a.keymap[3][3] = 'c';
  a.keymap[3][4] = KEY_UP_ARROW;
  a.keymap[4][0] = '4';
  a.keymap[4][1] = 'r';
  a.keymap[4][2] = 'f';
  a.keymap[4][3] = 'v';
  a.keymap[4][4] = KEY_RIGHT_ARROW;
  a.keymap[5][0] = '5';
  a.keymap[5][1] = 't';
  a.keymap[5][2] = 'g';
  a.keymap[5][3] = 'b';
  a.keymap[5][4] = ' ';
  return a;
}

Matrix thumb_matrix_left() { // alt, space, shift, dunno top right three yet
  Matrix a;
  a.init("left_thumb", 3, 2);
  a.setRemote();
  a.rows[0] = 9;
  a.rows[1] = 8;
  a.columns[0] = 5;
  a.columns[1] = 6;
  a.columns[2] = 7;
  a.keymap[1][0] = KEY_LEFT_SHIFT;
  a.keymap[0][1] = KEY_OVERLAY_1;
  a.keymap[0][0] = KEY_LEFT_GUI;//;
  a.keymap[1][1] = KEY_BACKSPACE;
  a.keymap[2][0] = KEY_LEFT_CTRL;
  a.keymap[2][1] = ' ';
  return a;
}
