#include "pico/stdlib.h"

#include "lcd_controller.h"

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

void lcd_cycle_enable_line(void) {
    gpio_put(LCD_E_PIN, true);
    sleep_ms(2);
    gpio_put(LCD_E_PIN, false);
}

void lcd_transmit_data(bool rs_value, uint8_t data) {
    uint32_t mask = (uint32_t)data << LCD_DATA_PIN_START;
    gpio_put(LCD_RS_PIN, rs_value);
    gpio_set_mask(mask);
    lcd_cycle_enable_line();
    gpio_clr_mask(mask);
}

void lcd_clear(void) {
    lcd_transmit_data(0, 1);
}

void lcd_initialise_display(bool lines, bool font) {
    lcd_transmit_data(false, 0b110000 | (lines << 3) | (font << 2));
    lcd_clear();
}

void lcd_display_set(bool display, bool cursor, bool blink) {
    lcd_transmit_data(false, 0b1000 | (display << 2) | (cursor << 1) | blink);
}

void lcd_scroll(bool cursor_screen, bool left_right) {
    lcd_transmit_data(
        false, 0b10000 | (cursor_screen << 3) | (left_right << 2));
}

void lcd_home(void) {
    lcd_transmit_data(0, 0b10);
}

void lcd_backlight(bool power) {
    gpio_put(LCD_A_PIN, power);
}

void lcd_write(const char *message) {
    lcd_home();
    bool used_second_line = false;
    for (const char *p = message; *p != 0; ++p) {
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

void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels) {
    const uint8_t *last_index = pixels + 7;
    for (const uint8_t *p = pixels; p <= last_index; ++p) {
        uint8_t pix = *p;
        // Set address in CGRAM to that of address for this line
        lcd_transmit_data(0, 0b1000000 | (char_number * 8 + (p - pixels)));
        // Move new character data
        lcd_transmit_data(1, pix);
    }
    lcd_home();
}