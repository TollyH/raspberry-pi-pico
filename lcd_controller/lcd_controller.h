#include <stdbool.h>
#include <stdint.h>

// Pins 0 and 1 are used for stdin/out UART
#define LCD_RS_PIN 2
#define LCD_E_PIN 3
// Data GPIO pins must be sequential
#define LCD_DATA_PIN_START 4
#define LCD_A_PIN 12

#define LCD_DATA_PIN_ALL 0b11111111 << LCD_DATA_PIN_START

#define LCD_SCREEN_WIDTH 16
#define LCD_SECOND_LINE_DDRAM 0x40

/*
* Toggle the enable pin on then off.
*/
void _lcd_cycle_enable_line(void);

/*
* Set the current DDRAM (cursor) address of the display.
*/
void _lcd_set_ddram_address(uint8_t address);

/*
* Initialise and set the direction of all required GPIO pins.
*/
void lcd_init_gpio(void);

/*
* Transmit an 8-bit value to the LCD display, with the RS pin either enabled or
* disabled, cycling the enable pin.
*/
void lcd_transmit_data(bool rs_value, uint8_t data);

/*
* Remove all characters from the display and return cursor to home.
*/
void lcd_clear(void);

/*
* Initialise the connected display. Must be used before display can be utilised
* lines: false = 1 line, true = 2 lines
* font: false = 5x8, true = 5x11
*/
void lcd_initialise_display(bool lines, bool font);

/*
* Set the visibility of different aspects of the display.
*/
void lcd_display_set(bool display, bool cursor, bool blink);

/*
* Move either the cursor or the entire screen.
* cursor_screen: false = cursor, true = screen
* left_right: false = left, true = right
*/
void lcd_scroll(bool cursor_screen, bool left_right);

/*
* Return the cursor to the start of the screen.
*/
void lcd_home(void);

/*
* Turn the display backlight on or off.
*/
void lcd_backlight(bool power);

/*
* Set the position of the cursor relative to either line 1 or 2.
* line: false = 1, true = 2
* offset: 0-based position index between 0 and 39
*/
void lcd_set_cursor_position(bool line, uint8_t offset);

/*
* Write a string to the display starting at the current cursor position.
* Use \x01 through \x08 inclusive to insert custom characters.
* Use \n to move to the second line.
* Automatic line wrapping is handled by this function.
*/
void lcd_write(const char *message);

/*
* Define a custom character. Character number can be between 0 and 7.
* Pixel array must contain 8 uint8_t values no greater than 0b11111 each.
* The lowest bit of each value corresponds to the rightmost pixel
* of each row of the character, starting at the top.
* This method returns the cursor to home upon completion.
*/
void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels);
