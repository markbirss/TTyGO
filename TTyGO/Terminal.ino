/* Terminal emulator

   Implements a simple ANSI escape code terminal interpreter using TTGO OLED ESP8622 SH1106 as platform.
   Uses U8g2lib graphical library, so it is compatible with a lot of displays https://github.com/olikraus/u8g2/wiki
*/

char charsets[4] = {'B','B','B','B'};
char current_charset = 0;
Cursor current_cursor, saved_cursor;
unsigned char terminal_tab_stops[TERMINAL_TABS_COUNT];

ScrollRegion scroll_region;
int char_height, char_width,
    terminal_width, terminal_height, display_height_offset, display_width_offset; // This is all set by the terminal_setup based on current font and display size
Glyph terminal_buffer[TERMINAL_COUNT];
unsigned char terminal_buffer_dirty[TERMINAL_DIRTY_COUNT];

void terminal_cursor_move_to_tab(int next) {
  if (next) {
    for (current_cursor.x++; current_cursor.x <= terminal_width; current_cursor.x++) {
      if (BIT(terminal_tab_stops, current_cursor.x)) break;
    }
  } else {
    for (current_cursor.x--; current_cursor.x >= 1; current_cursor.x--) {
      if (BIT(terminal_tab_stops, current_cursor.x)) break;
    }    
  }
}

void terminal_scroll(int start, int end, int up) {
  int top = max(scroll_region.upper, start);
  int bottom = min(scroll_region.lower, end);

  if (up == 1) {
    for (int y = top; y <= bottom; y++) {
      for (int x = 1; x <= terminal_width; x++) {
        TERM_SET(x, y, TERM(x, y + 1));
      }
    }
  } else {
    for (int y = bottom; y >= top; y--) {
      for (int x = 1; x <= terminal_width; x++) {
        TERM_SET(x, y, TERM(x, y - 1));
      }
    }
  }
}

void terminal_scroll_line(int y, int start, int end, int direction_left) {
  int left = max(1, start);
  int right = min(terminal_width, end);

  if (direction_left == 1) {
    for (int x = left; x <= right; x++) {
      TERM_SET(x, y, TERM(x + 1, y));
    }
  } else {
    for (int x = right; x >= left; x--) {
      TERM_SET(x, y, TERM(x - 1, y));
    }
  }
}

void terminal_clear_line(int x, int y, int mode) {
  switch (mode)
  {
    case 0:
      for (int i = x; i <= terminal_width; i++) {
        TERM_SET(i, y, {' '});
      }
      break;
    case 1:
      for (int i = 1; i <= x; i++) {
        TERM_SET(i, y, {' '});
      }
      break;
    case 2:
      for (int i = 1; i <= terminal_width; i++) {
        TERM_SET(i, y, {' '});
      }
      break;
  }
}

void terminal_cursor_save() {
  saved_cursor = current_cursor;
}

void terminal_cursor_restore() {
  current_cursor = saved_cursor;
}

// Clears the char buffer
void terminal_clear(int mode)
{
  int start = 1;
  int end = terminal_height;

  switch (mode)
  {
    case 0:
      start = current_cursor.y;
      break;
    case 1:
      end = current_cursor.y;
      break;
    default:
      break;
  }

  for (int y = start; y <= end; y++)
  {
    for (int x = 1; x <= terminal_width; x++)
    {
      TERM_SET(x, y, {NUL});
    }
  }

  if (mode >= 2) // Reposition cursor to the top left
  {
    current_cursor.x = 1;
    current_cursor.y = 1;
  }
}

void handle_scroll() {
  newline_eating_mode = 0;
  if (current_cursor.x > terminal_width) {
    current_cursor.x = 1;
    current_cursor.y++;
    newline_eating_mode = 1;
  }
  if (current_cursor.y > scroll_region.lower) {
    current_cursor.y = scroll_region.lower;
    terminal_scroll(0, scroll_region.lower, 1);
    terminal_clear_line(0, scroll_region.lower, 2);    
  }
}

// Inserts a char into the char buffer
int newline_eating_mode = 0;
void terminal_backspace()
{
  if (current_cursor.x > 1) {
    current_cursor.x -= 1;
  }
  TERM_SET(current_cursor.x, current_cursor.y, {' '});
  handle_scroll();
}

void terminal_newline()
{
  if (!newline_eating_mode) {
    current_cursor.x = 1; current_cursor.y++; // Line feed
  }
  handle_scroll();
}

void terminal_put_glyph(Glyph g) {
  TERM_SET(current_cursor.x, current_cursor.y, g);
  ++current_cursor.x;
  handle_scroll();
}

void terminal_reset()
{
   scroll_region.upper = 1;
   scroll_region.lower = terminal_height;

   for (int i = 1; i <= terminal_width; i++) {
     BIT_SET(terminal_tab_stops, i, (i - 1) % 8 == 0);
   }
}