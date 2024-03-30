#include <stdbool.h>
#include <stdint.h>

// Pins 0 and 1 are used for stdin/out UART
#define LCD_RS_PIN 2
#define LCD_RW_PIN 13
#define LCD_E_PIN 3
// Data GPIO pins must be sequential
#define LCD_DATA_PIN_START 4
#define LCD_A_PIN 12

#define LCD_DATA_PIN_ALL 0b11111111 << LCD_DATA_PIN_START

#define LCD_SCREEN_WIDTH 16
#define LCD_SECOND_LINE_DDRAM 0x40
#define LCD_STRING_TOTAL_CHARS (LCD_SCREEN_WIDTH * 2) + 2

struct LCDPosition {
    bool line;
    uint8_t offset;
};

// INTERNAL METHODS

/*
* Toggle the enable pin on then off.
*/
void _lcd_cycle_enable_line(void);

/*
* Set the GPIO direction of the data pins. Also sets the RW pin.
*/
void _lcd_set_data_direction(bool out);

/*
* Get the current address counter from the LCD.
*/
uint8_t _lcd_get_address(void);

/*
* Set the current DDRAM (display data) address of the display.
*/
void _lcd_set_ddram_address(uint8_t address);

/*
* Set the current CGRAM (character generator) address of the display.
* Note: The LCD display only has one address register. Moving it into CGRAM
* will prevent you from displaying characters, as these are in DDRAM.
* You should always call _lcd_set_ddram_address or lcd_set_cursor_position
* once you have finished working in CGRAM.
*/
void _lcd_set_cgram_address(uint8_t address);

// GPIO INIT METHOD

/*
* Initialise and set the direction of all required GPIO pins.
*/
void lcd_init_gpio(void);

// RX METHODS

/*
* Determine whether or not the LCD is currently busy and unable to respond to instructions.
*/
bool lcd_is_busy(void);

/*
* Receive an 8-bit value from the LCD display, with the RS pin either enabled or
* disabled, cycling the enable pin.
* rs_value should be true to read CGRAM/DDRAM data.
* rs_value of false gets current address and busy flag, which should instead be
* retrieved with the lcd_is_busy and lcd_get_address methods.
* wait_for_not_busy should be true unless the purpose of the call is to check the busy flag.
*/
uint8_t lcd_receive_data(bool rs_value, bool wait_for_not_busy);

/*
* Get the position of the cursor relative to either line 1 or 2.
* line: false = 1, true = 2
* offset: 0-based position index between 0 and 39
*/
struct LCDPosition lcd_get_cursor_position(void);

/*
* Read the text currently on the screen as a C string.
* String must have enough capacity for LCD_STRING_TOTAL_CHARS.
* Custom characters are represented by \x01 through \x08 inclusive.
* Lines are separated by \n.
*/
void lcd_read(char *string);

/*
* Retrieve pixels for a defined custom character. Character number can be between 0 and 7.
* Pixel array must have capacity for 8 uint8_t values. They will be no greater than 0b11111 each.
* The lowest bit of each value corresponds to the rightmost pixel
* of each row of the character, starting at the top.
*/
void lcd_get_custom_char(uint8_t char_number, uint8_t *pixels);

// TX METHODS

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
void lcd_set_cursor_position(struct LCDPosition position);

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
*/
void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels);
