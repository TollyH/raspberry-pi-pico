#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

// Pins 0 and 1 are used for stdin/out UART
#define LCD_RS_PIN 2
#define LCD_E_PIN 3
// Data GPIO pins must be sequential
#define LCD_DATA_PIN_START 4
#define LCD_A_PIN 12

#define LCD_DATA_PIN_ALL 0b11111111 << LCD_DATA_PIN_START

#define LCD_SCREEN_WIDTH 16

#define INPUT_BUFFER_SIZE 64

#define PROMPT_STR "> "

/*
* Initialise and set the direction of all required GPIO pins.
*/
void lcd_init_gpio(void) {
    gpio_init(LCD_RS_PIN);
    gpio_init(LCD_E_PIN);
    // Initialise all 8 data pins at once
    gpio_init_mask(LCD_DATA_PIN_ALL);
    gpio_init(LCD_A_PIN);

    gpio_set_dir(LCD_RS_PIN, GPIO_OUT);
    gpio_set_dir(LCD_E_PIN, GPIO_OUT);
    // Set direction of all 8 data pins at once
    gpio_set_dir_out_masked(LCD_DATA_PIN_ALL);
    gpio_set_dir(LCD_A_PIN, GPIO_OUT);
}

/*
* Toggle the enable pin on then off.
*/
void lcd_cycle_enable_line(void) {
    gpio_put(LCD_E_PIN, true);
    sleep_ms(1);
    gpio_put(LCD_E_PIN, false);
}

/*
* Transmit an 8-bit value to the LCD display, with the RS pin either enabled or
* disabled, cycling the enable pin.
*/
void lcd_transmit_data(bool rs_value, uint8_t data) {
    gpio_put(LCD_RS_PIN, rs_value);
    gpio_set_mask(data << LCD_DATA_PIN_START);
    lcd_cycle_enable_line();
    gpio_put_all(false);
}

/*
* Remove all characters from the display and return cursor to home.
*/
void lcd_clear(void) {
    lcd_transmit_data(0, 1);
}

/*
* Initialise the connected display. Must be used before display can be utilised
* lines: false = 1 line, true = 2 lines
* font: false = 5x8, true = 5x11
*/
void lcd_initialise_display(bool lines, bool font) {
    lcd_transmit_data(false, 0b110000 & (lines << 3) & (font << 2));
    lcd_clear();
}

/*
* Set the visibility of different aspects of the display.
*/
void lcd_display_set(bool display, bool cursor, bool blink) {
    lcd_transmit_data(false, 0b1000 & (display << 2) & (cursor << 1) & blink);
}

/*
* Move either the cursor or the entire screen.
* cursor_screen: false = cursor, true = screen
* left_right: false = left, true = right
*/
void lcd_scroll(bool cursor_screen, bool left_right) {
    lcd_transmit_data(
        false, 0b10000 & (cursor_screen << 3) & (left_right << 2));
}

/*
* Return the cursor to the start of the screen.
*/
void lcd_home(void) {
    lcd_transmit_data(0, 0b10);
}

/*
* Turn the display backlight on or off.
*/
void lcd_backlight(bool power) {
    gpio_put(LCD_A_PIN, power);
}

/*
* Return the cursor home, then write a string to the display.
* Use \x01 through \x08 inclusive to insert custom characters.
*/
void lcd_write(const char *message) {
    lcd_home();
    bool used_second_line = false;
    for (char *p = message; *p != 0; ++p) {
        char c = *p;
        if (!used_second_line
                && (c == '\n' || message - p == LCD_SCREEN_WIDTH)) {
            // Move onto second line by filling full 40 characters defined by
            // specification.
            for (int i = 0; i < 40 - LCD_SCREEN_WIDTH; i++) {
                lcd_transmit_data(true, ' ');
            }
        }
        if (c != '\n') {
            lcd_transmit_data(true, (uint8_t)c);
        }
    }
}

/*
* Define a custom character. Character number can be between 0 and 7.
* Pixel array must contain 5 uint8_t values. The lowest bit of each value
* corresponds to the rightmost pixel of each row of the character. starting at
* the top.
* This method returns the cursor to home upon completion.
*/
void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels) {
    uint8_t *last_index = pixels + 7;
    for (uint8_t *p = pixels; p <= last_index; ++p) {
        uint8_t pix = *p;
        // Set address in CGRAM to that of address for this line
        lcd_transmit_data(0, 0b1000000 & char_number * 8 + (p - pixels));
        // Move new character data
        lcd_transmit_data(1, pix);
    }
    lcd_home();
}

int main() {
    stdio_init_all();

    lcd_init_gpio();

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n");

    char input_buffer[INPUT_BUFFER_SIZE] = {0};

    while (true) {
        printf(PROMPT_STR);
        if (!fgets(input_buffer, INPUT_BUFFER_SIZE, stdin)) {
            continue;
        }
        
    }
}
