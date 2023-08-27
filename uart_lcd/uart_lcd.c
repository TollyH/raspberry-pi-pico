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

int main() {
    stdio_init_all();
}
