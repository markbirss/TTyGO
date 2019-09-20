int cursor_left, cursor_top, char_height, char_width;
int terminal_width, terminal_height, display_height_offset, display_width_offset;
char terminal_buffer[80 * 80]; // Just a maximum

/* ASCII control characters recognised
*/
#define CR  '\r'
#define LF  '\n'
#define VT  '\v'
#define BS  '\b'
#define FF  '\f'
#define ESC '\e'
#define NUL '\0'

void terminal_setup()
{
  // init display
  u8g2.begin();
  u8g2.setFont(lcd_font);

  char_height = u8g2.getMaxCharHeight();
  char_width = u8g2.getMaxCharWidth();
  terminal_height = u8g2.getDisplayHeight() / char_height;
  terminal_width = u8g2.getDisplayWidth() / char_width;
  display_height_offset = (u8g2.getDisplayHeight() - (terminal_height * char_height)) / 2;
  display_width_offset = (u8g2.getDisplayWidth() - (terminal_width * char_width)) / 2;

  u8g2.sendF("c", 0x0a7);

  terminal_clear();
}

// Sets the terminal cursor to the proper col and row, based on 1
void terminal_setcursor(int col, int row)
{
  u8g2.setCursor(display_width_offset + char_width * (col - 1), display_height_offset + char_height * row);
}

// Draws terminal to the lcd
void terminal_draw()
{
  u8g2.clearBuffer();

  for (int x = 1; x <= terminal_width; x++)
    for (int y = 1; y <= terminal_height; y++)
    {
      char c = terminal_buffer[(x - 1) + (y - 1) * terminal_width];
      terminal_setcursor(x, y);
      u8g2.print(c);
    }

  u8g2.sendBuffer();
}

// Clears the char buffer
void terminal_clear()
{
  for (int i = 0; i < terminal_width * terminal_height; i++)
    terminal_buffer[i] = NUL;

  cursor_left = 1;
  cursor_top = 1;

  terminal_draw();
}

// Inserts a char into the char buffer
void terminal_put(char c)
{
  switch (c)
  {
    case '\n':
      cursor_left = 1;
      cursor_top++;
      break;

    default:
      terminal_buffer[(cursor_left - 1) + (cursor_top - 1)*terminal_width] = c;
      ++cursor_left;
  }
}

// Main routine
bool is_on_command_mode = false;
void terminal_loop()
{
  bool lcd_dirty = true; // invoke a redraw
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.print("Received: ");
    Serial.print(c,DEC);
    Serial.println();

    switch (c)
    {
      case ESC:
        is_on_command_mode = true;
        lcd_dirty = false;
        break;

      default:
        if (is_on_command_mode)
        {
          switch (c)
          {
            case 'c':
              terminal_clear();
              is_on_command_mode = false; // end of command mode
              break;
          }
        }
        else
        {
          // Just print the character
          terminal_put(c);
        }
        break;
    }

  }

  if (lcd_dirty)
    terminal_draw();
}