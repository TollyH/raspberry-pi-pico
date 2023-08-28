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

#define INPUT_BUFFER_SIZE 64

#define PROMPT_STR "> "

void lcd_init_gpio(void) {
    gpio_init(LCD_RS_PIN);
    gpio_init(LCD_E_PIN);
    // Initialise all 8 data pins at once
    gpio_init_mask(0b11111111 << LCD_DATA_PIN_START);
    gpio_init(LCD_A_PIN);

    gpio_set_dir(LCD_RS_PIN, GPIO_OUT);
    gpio_set_dir(LCD_E_PIN, GPIO_OUT);
    // Set direction of all 8 data pins at once
    gpio_set_dir_out_masked(0b11111111 << LCD_DATA_PIN_START);
    gpio_set_dir(LCD_A_PIN, GPIO_OUT);
}

int main() {
    stdio_init_all();

    lcd_init_gpio();

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n");

    char input_buffer[INPUT_BUFFER_SIZE] = {0};

    while (1) {
        printf(PROMPT_STR);
        if (!fgets(input_buffer, INPUT_BUFFER_SIZE, stdin)) {
            continue;
        }
        
    }
}
