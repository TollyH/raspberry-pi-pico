#include "pico/stdlib.h"

#include "lcd_controller.h"

// Keeps track of how many characters we've already written to the screen
// to keep consistent line wrapping behaviour
static int current_chars_written = 0;

void _lcd_cycle_enable_line(void) {
    gpio_put(LCD_E_PIN, true);
    sleep_ms(2);
    gpio_put(LCD_E_PIN, false);
}

void _lcd_set_ddram_address(uint8_t address) {
    lcd_transmit_data(false, 0b10000000 | address);
    current_chars_written = address % LCD_SECOND_LINE_DDRAM;
}

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

void lcd_transmit_data(bool rs_value, uint8_t data) {
    uint32_t mask = (uint32_t)data << LCD_DATA_PIN_START;
    gpio_put(LCD_RS_PIN, rs_value);
    gpio_set_mask(mask);
    _lcd_cycle_enable_line();
    gpio_clr_mask(mask);
}

void lcd_clear(void) {
    lcd_transmit_data(0, 1);
    current_chars_written = 0;
}

void lcd_initialise_display(bool lines, bool font) {
    lcd_transmit_data(false, 0b110000 | (lines << 3) | (font << 2));
    lcd_clear();
    current_chars_written = 0;
}

void lcd_display_set(bool display, bool cursor, bool blink) {
    lcd_transmit_data(false, 0b1000 | (display << 2) | (cursor << 1) | blink);
}

void lcd_scroll(bool cursor_screen, bool left_right) {
    lcd_transmit_data(
        false, 0b10000 | (cursor_screen << 3) | (left_right << 2));
    if (!cursor_screen) {
        current_chars_written += left_right ? -1 : 1;
    }
}

void lcd_home(void) {
    lcd_transmit_data(0, 0b10);
    current_chars_written = 0;
}

void lcd_backlight(bool power) {
    gpio_put(LCD_A_PIN, power);
}

void lcd_set_cursor_position(bool line, uint8_t offset) {
    if (line) {
        offset += LCD_SECOND_LINE_DDRAM;
    }
    _lcd_set_ddram_address(offset);
}

void lcd_write(const char *message) {
    for (const char *p = message; *p != 0; ++p) {
        char c = *p;
        if (c >= '\x01' && c <= '\x08') {
            // Character should be a custom character.
            // Convert 1-based index to 0-based.
            --c;
        }
        if (c != '\n') {
            lcd_transmit_data(true, (uint8_t)c);
        }
        if (c == '\n' || ++current_chars_written == LCD_SCREEN_WIDTH) {
            // Move to first character of second line
            lcd_set_cursor_position(true, 0);
            current_chars_written = 0;
            continue;
        }
    }
}

void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels) {
    // Set address in CGRAM to that of address for this character
    lcd_transmit_data(0, 0b1000000 | char_number * 8);
    for (int i = 0; i < 8; ++i) {
        // Set character line data
        // (display will automatically move to next line in character)
        lcd_transmit_data(1, pixels[i] & 0b11111);
    }
    lcd_home();
}
