Matrix main_matrix_right() {
  Matrix a;
  clearMatrix(&a);
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
  a.columnCount = MAX_COLS;
  a.rowCount = MAX_ROWS;
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
  a.name = "main_right";
  return a;
}

Matrix thumb_matrix_right() {
  Matrix a;
  clearMatrix(&a);
  a.rows[0] = 16;
  a.rows[1] = 10;
  a.columns[0] = 18;
  a.columns[1] = 15;
  a.columns[2] = 14;
  a.keymap[0][0] = KEY_RIGHT_CTRL;
  a.keymap[0][1] = KEY_OVERLAY_1;
  a.keymap[1][0] = KEY_RIGHT_ALT;
  a.keymap[1][1] = KEY_RETURN;
  a.keymap[2][0] = KEY_RIGHT_SHIFT;
  a.columnCount = 3; 
  a.rowCount = 2;
  a.name = "thumb_right";
  return a;
}

Matrix main_matrix_left() {
  Matrix a;
  clearMatrix(&a);
  a.remote = true;
  a.rows[0] = 0; // TX0
  a.rows[1] = 0; // RX1
  a.rows[2] = 0;
  a.rows[3] = 0;
  a.rows[4] = 0;
  a.columns[0] = 0;
  a.columns[1] = 0;
  a.columns[2] = 0;
  a.columns[3] = 0;
  a.columns[4] = 0;
  a.columns[5] = 0;
  a.columnCount = 0;
  a.rowCount = 0;
  a.keymap[0][0] = '`';
  a.keymap[0][1] = KEY_TAB;
  a.keymap[0][2] = KEY_ESC;
  a.keymap[0][3] = '9';
  a.keymap[0][4] = '9';
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
  a.keymap[5][4] = KEY_LEFT_GUI;
  a.remote = true;
  a.name = "left_main";
  return a;
}

Matrix thumb_matrix_left() { // alt, space, shift, dunno top right three yet
  Matrix a;
  clearMatrix(&a);
  a.remote = true;
  a.rows[0] = 0;
  a.rows[1] = 0;
  a.columns[0] = 0;
  a.columns[1] = 0;
  a.columns[2] = 0;
  a.keymap[0][0] = KEY_LEFT_CTRL;
  a.keymap[0][1] = KEY_OVERLAY_1;
  a.keymap[1][0] = KEY_LEFT_ALT;
  a.keymap[1][1] = KEY_RETURN;
  a.keymap[2][0] = KEY_LEFT_SHIFT;
  a.columnCount = 0;
  a.rowCount = 0;
  a.name = "left_thumb";
  return a;
}
