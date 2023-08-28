#include <stdio.h>
#include "pico/stdlib.h"

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

// Pins 0 and 1 are used for stdin/out UART
#define LCD_RS_PIN 2
#define LCD_E_PIN 3
#define LCD_D0_PIN 4
#define LCD_D1_PIN 5
#define LCD_D2_PIN 6
#define LCD_D3_PIN 7
#define LCD_D4_PIN 8
#define LCD_D5_PIN 9
#define LCD_D6_PIN 10
#define LCD_D7_PIN 11
#define LCD_A_PIN 12

#define INPUT_BUFFER_SIZE 64

#define PROMPT_STR "> "

void lcd_init_gpio(void) {
    gpio_init(LCD_RS_PIN);
    gpio_init(LCD_E_PIN);
    gpio_init(LCD_D0_PIN);
    gpio_init(LCD_D1_PIN);
    gpio_init(LCD_D2_PIN);
    gpio_init(LCD_D3_PIN);
    gpio_init(LCD_D4_PIN);
    gpio_init(LCD_D5_PIN);
    gpio_init(LCD_D6_PIN);
    gpio_init(LCD_D7_PIN);
    gpio_init(LCD_A_PIN);

    gpio_set_dir(LCD_RS_PIN, GPIO_OUT);
    gpio_set_dir(LCD_E_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D0_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D1_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D2_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D3_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D4_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D5_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D6_PIN, GPIO_OUT);
    gpio_set_dir(LCD_D7_PIN, GPIO_OUT);
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
