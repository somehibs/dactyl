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
  a.keymap[0][0] = '7';
  a.keymap[0][1] = 'y';
  a.keymap[0][2] = 'h';
  a.keymap[0][3] = 'n';
  a.keymap[0][4] = KEY_RETURN;
  a.keymap[1][0] = '8';
  a.keymap[1][1] = 'u';
  a.keymap[1][2] = 'j';
  a.keymap[1][3] = 'm';
  a.keymap[1][4] = '[';
  a.keymap[2][0] = '9';
  a.keymap[2][1] = 'i';
  a.keymap[2][2] = 'k';
  a.keymap[2][3] = ',';
  a.keymap[2][4] = ']';
  a.keymap[3][0] = '0';
  a.keymap[3][1] = 'o';
  a.keymap[3][2] = 'l';
  a.keymap[3][3] = '.';
  a.keymap[3][4] = '2';
  a.keymap[4][0] = '-';
  a.keymap[4][1] = 'p';
  a.keymap[4][2] = ';';
  a.keymap[4][3] = '/';
  a.keymap[4][4] = KEY_PAGE_UP;
  a.keymap[5][0] = '=';
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
  a.keymap[0][0] = KEY_HOME;
  a.keymap[0][1] = KEY_OVERLAY_1;
  a.keymap[1][0] = KEY_END;
  a.keymap[1][1] = ' '; // actu ally a space bar, dont remove it
  a.keymap[2][0] = KEY_RIGHT_SHIFT;
  a.columnCount = 3;
  a.rowCount = 2;
  a.name = "thumb_right";
  return a;
}

Matrix main_matrix_left() {// 1, `, tab, unk, ctrl
  Matrix a;
  clearMatrix(&a);
  a.remote = true;
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
  a.columnCount = 0;
  a.rowCount = 0;
  a.keymap[0][0] = '7';
  a.keymap[0][1] = 'y';
  a.keymap[0][2] = 'h';
  a.keymap[0][3] = 'n';
  a.keymap[0][4] = ' ';
  a.keymap[1][0] = '8';
  a.keymap[1][1] = 'u';
  a.keymap[1][2] = 'j';
  a.keymap[1][3] = 'm';
  a.keymap[1][4] = ' ';
  a.keymap[2][0] = '9';
  a.keymap[2][1] = 'i';
  a.keymap[2][2] = 'k';
  a.keymap[2][3] = ',';
  a.keymap[2][4] = '[';
  a.keymap[3][0] = '0';
  a.keymap[3][1] = 'o';
  a.keymap[3][2] = 'l';
  a.keymap[3][3] = '.';
  a.keymap[3][4] = ']';
  a.keymap[4][0] = '-';
  a.keymap[4][1] = 'p';
  a.keymap[4][2] = ';';
  a.keymap[4][3] = '/';
  a.keymap[4][4] = ' ';
  a.keymap[5][0] = '=';
  a.keymap[5][1] = '\\';
  a.keymap[5][2] = '\'';
  a.keymap[5][3] = '?';
  a.keymap[5][4] = ' ';
  a.remote = true;
  a.name = "main";
  return a;
}

Matrix thumb_matrix_left() { // alt, space, shift, dunno top right three yet
  Matrix a;
  clearMatrix(&a);
  a.remote = true;
  a.rows[0] = 16;
  a.rows[1] = 10;
  a.columns[0] = 18;
  a.columns[1] = 15;
  a.columns[2] = 14;
  a.columnCount = 0;
  a.rowCount = 0;
  a.name = "thumb";
  return a;
}
