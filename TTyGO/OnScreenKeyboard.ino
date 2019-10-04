#include "utilities.h"
bool osk_visible = false;

typedef struct {
  char label[4];
  char code[8];
} Key;

const Key osk_keyboard[] = { {"A", "A"}, {"B", "B"}, {"C", "C"}, {"D", "D"}, {"E", "E"}, {"F", "F"}, {"G", "G"}, {"H", "H"}, {"I", "I"}, {"J", "J"}, {"K", "K"},
  {"L", "L"}, {"M", "M"}, {"N", "N"}, {"O", "O"}, {"P", "P"}, {"Q", "Q"}, {"R", "R"}, {"S", "S"}, {"T", "T"}, {"U", "U"}, {"V", "V"}, {"W", "W"}, {"X", "X"},
  {"Y", "Y"}, {"Z", "Z"}, {"0", "0"}, {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"}, {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"}, {"9", "9"}, {"a", "a"},
  {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"}, {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"}, {"m", "m"}, {"n", "n"},
  {"o", "o"}, {"p", "p"}, {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"}, {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"}, {"y", "y"}, {"z", "z"}, {"[", "["},
  {"]", "]"}, {"(", "("}, {")", ")"}, {".", "."}, {",", ","}, {":", ":"}, {";", ";"}, {"+", "+"}, {"-", "-"}, {"*", "*"}, {"/", "/"}, {"!", "!"}, {""", """},
  {"#", "#"}, {"%", "%"}, {"&", "&"}, {"?", "?"}, {"'", "'"}, {"*", "*"}, {"|", "|"}, {"_", "_"}, {"<", "<"}, {">", ">"}, {"ESC", ESC}, {"RET", "\n"}, {"TAB", "\t"},
};

const int osk_keyboard_length = ArrayLength(osk_keyboard);
const int osk_default_selection = 0;
const int osk_largejump = 10; // When holding a key
int osk_current_selection;

// Draws the on screen keyboard to the lcd
void osk_draw()
{
  u8g2.drawRBox(10, 10, 17, 10, 3);
  u8g2.setCursor(12, 17);
  u8g2.setDrawColor(0);
  u8g2.print(osk_keyboard[osk_current_selection].label);
  u8g2.setDrawColor(1);
}

// Returns true if it was out of bounds
bool osk_update_check_bounds()
{
  // Loop boundaries
  if (osk_current_selection < 0)
    osk_current_selection = osk_keyboard_length + osk_current_selection;
  else if (osk_current_selection >= osk_keyboard_length)
    osk_current_selection = osk_current_selection - osk_keyboard_length;
  else
    return false;
  return true;
}

// Select a key
bool osk_update_selection(int v)
{
  lcd_dirty = true;
  osk_current_selection += v;
  while (osk_update_check_bounds());
}

void button_osk_left_click()
{
  osk_update_selection(-1);
}

void button_osk_left_hold()
{
  osk_update_selection(-osk_largejump);
}

void button_osk_middle_click()
{
  Serial.print(osk_keyboard[osk_current_selection].code);
}

void button_osk_middle_hold()
{
  osk_hide();
}

void button_osk_right_click()
{
  osk_update_selection(1);
}

void button_osk_right_hold()
{
  osk_update_selection(osk_largejump);
}

void attach_osk_buttons()
{
  // Set buttons (all supported modes: https://github.com/mathertel/OneButton/blob/master/examples/TwoButtons/TwoButtons.ino )
  button_left.attachClick(button_osk_left_click);
  button_left.attachLongPressStart(button_osk_left_hold);
  button_middle.attachClick(button_osk_middle_click);
  button_middle.attachLongPressStart(button_osk_middle_hold);
  button_right.attachClick(button_osk_right_click);
  button_right.attachLongPressStart(button_osk_right_hold);
}

void osk_show()
{
  osk_current_selection = osk_default_selection;
  osk_visible = true;
  attach_osk_buttons();
}

void osk_hide()
{
  osk_visible = false;
  attach_buttons();
}
