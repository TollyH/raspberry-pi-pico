#include "pico/stdlib.h"

#include "lcd_controller.h"

void _lcd_cycle_enable_line(void) {
    gpio_put(LCD_E_PIN, true);
    sleep_us(1);
    gpio_put(LCD_E_PIN, false);
}

void _lcd_set_data_direction(bool out) {
    // RW pin is 0 for write, 1 for read
    gpio_put(LCD_RW_PIN, !out);
    // Set direction of all 8 data pins at once
    if (out) {
        gpio_set_dir_out_masked(LCD_DATA_PIN_ALL);
    } else {
        gpio_set_dir_in_masked(LCD_DATA_PIN_ALL);
    }
}

uint8_t _lcd_get_address(void) {
    return lcd_receive_data(false, true) & 0b1111111;
}

void _lcd_set_ddram_address(uint8_t address) {
    lcd_transmit_data(false, 0b10000000 | address);
}

void _lcd_set_cgram_address(uint8_t address) {
    lcd_transmit_data(false, 0b1000000 | address);
}

void lcd_init_gpio(void) {
    gpio_init(LCD_RS_PIN);
    gpio_init(LCD_RW_PIN);
    gpio_init(LCD_E_PIN);
    // Initialise all 8 data pins at once
    gpio_init_mask(LCD_DATA_PIN_ALL);
    gpio_init(LCD_A_PIN);
    gpio_init(LCD_LED_PIN);

    gpio_set_dir(LCD_RS_PIN, GPIO_OUT);
    gpio_set_dir(LCD_RW_PIN, GPIO_OUT);
    gpio_set_dir(LCD_E_PIN, GPIO_OUT);
    gpio_set_dir(LCD_A_PIN, GPIO_OUT);
    gpio_set_dir(LCD_LED_PIN, GPIO_OUT);
}

bool lcd_is_busy(void) {
    return (lcd_receive_data(false, false) & 0b10000000) >> 7;
}

uint8_t lcd_receive_data(bool rs_value, bool wait_for_not_busy) {
    while (wait_for_not_busy && lcd_is_busy()) { }

    gpio_put(LCD_LED_PIN, true);

    _lcd_set_data_direction(GPIO_IN);
    gpio_put(LCD_RS_PIN, rs_value);

    gpio_put(LCD_E_PIN, true);
    sleep_us(1);
    uint8_t data = (gpio_get_all() & LCD_DATA_PIN_ALL) >> LCD_DATA_PIN_START;
    gpio_put(LCD_E_PIN, false);

    sleep_us(LCD_SHORT_SLEEP_US);
    gpio_put(LCD_LED_PIN, false);

    return data;
}

struct LCDPosition lcd_get_cursor_position(struct LCDSize size) {
    uint8_t address = _lcd_get_address();

    uint8_t mod_second_line = address % LCD_SECOND_LINE_DDRAM;
    uint8_t line;
    if (mod_second_line >= size.width) {
        line = address >= LCD_SECOND_LINE_DDRAM ? 3 : 2;
    } else {
        line = address >= LCD_SECOND_LINE_DDRAM ? 1 : 0;
    }

    return (struct LCDPosition){
        .line = line,
        .offset = mod_second_line % size.width
    };
}

void lcd_read(struct LCDSize size, char *string) {
    // Store old DDRAM address to return to later
    uint8_t old_address = _lcd_get_address();

    int characters_per_line = size.width + 1;

    for (int y = 0; y < size.height; y++) {
        lcd_set_cursor_position(size,
            (struct LCDPosition){.line = y, .offset = 0});
        for (int x = 0; x < size.width; x++) {
            // Get character data
            // (display will automatically move to next character)
            uint8_t data = lcd_receive_data(true, true);
            if (data <= 7) {
                // Convert 0-indexed custom character to 1-indexed
                ++data;
            }
            string[y * characters_per_line + x] = data;
        }
        string[y * characters_per_line + size.width] = '\n';
    }
    // Ensure string is null terminated
    string[size.height * characters_per_line - 1] = '\0';

    // Restore DDRAM address
    _lcd_set_ddram_address(old_address);
}

void lcd_get_custom_char(uint8_t char_number, uint8_t *pixels) {
    // Store old DDRAM address to return to later
    uint8_t old_address = _lcd_get_address();

    // Set address in CGRAM to that of address for this character
    _lcd_set_cgram_address(char_number * 8);
    for (int i = 0; i < 8; i++) {
        // Get character line data
        // (display will automatically move to next line in character)
        pixels[i] = lcd_receive_data(true, true);
    }

    // Restore DDRAM address
    _lcd_set_ddram_address(old_address);
}

void lcd_transmit_data(bool rs_value, uint8_t data) {
    while (lcd_is_busy()) { }

    gpio_put(LCD_LED_PIN, true);

    _lcd_set_data_direction(GPIO_OUT);
    uint32_t gpio_data = (uint32_t)data << LCD_DATA_PIN_START;
    gpio_put(LCD_RS_PIN, rs_value);
    gpio_put_masked(LCD_DATA_PIN_ALL, gpio_data);
    _lcd_cycle_enable_line();

    sleep_us(LCD_SHORT_SLEEP_US);
    gpio_put(LCD_LED_PIN, false);
}

void lcd_clear(void) {
    lcd_transmit_data(false, 1);
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
    lcd_transmit_data(false, 0b10);

    gpio_put(LCD_LED_PIN, true);
    sleep_ms(LCD_LONG_SLEEP_MS);
    gpio_put(LCD_LED_PIN, false);
}

void lcd_backlight(bool power) {
    gpio_put(LCD_A_PIN, power);
}

void lcd_set_cursor_position(struct LCDSize size, struct LCDPosition position) {
    if (position.line % 2 != 0) {
        position.offset += LCD_SECOND_LINE_DDRAM;
    }
    if (position.line >= 2) {
        position.offset += size.width;
    }
    _lcd_set_ddram_address(position.offset);
}

void lcd_write(struct LCDSize size, const char *message) {
    uint8_t last_line = lcd_get_cursor_position(size).line;
    for (const char *p = message; *p != 0; p++) {
        char c = *p;
        if (c >= '\x01' && c <= '\x08') {
            // Character should be a custom character.
            // Convert 1-based index to 0-based.
            --c;
        }
        if (c != '\n') {
            lcd_transmit_data(true, (uint8_t)c);
        }
        struct LCDPosition position = lcd_get_cursor_position(size);
        if (c == '\n' || position.offset >= size.width || position.line != last_line) {
            // Move to first character of next line
            lcd_set_cursor_position(size,
                (struct LCDPosition){
                    .line = ++last_line % size.height,
                    .offset = 0
                }
            );
        }
    }
}

void lcd_define_custom_char(uint8_t char_number, const uint8_t *pixels) {
    // Store old DDRAM address to return to later
    // (setting character data requires moving cursor into CGRAM)
    uint8_t old_address = _lcd_get_address();

    // Set address in CGRAM to that of address for this character
    _lcd_set_cgram_address(char_number * 8);
    for (int i = 0; i < 8; i++) {
        // Set character line data
        // (display will automatically move to next line in character)
        lcd_transmit_data(true, pixels[i] & 0b11111);
    }

    // Restore DDRAM address
    _lcd_set_ddram_address(old_address);
}
