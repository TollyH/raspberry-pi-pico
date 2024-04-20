#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include <lcd_controller.h>

#define INPUT_BUFFER_SIZE 128
#define MAX_ARGS 16

#define PROMPT_STR "\n> "

static void command_help(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 0) {
        printf("The #help command takes no arguments.\n");
        return;
    }

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n"
        "Write any text not prefixed with # to write it to the display\n"
        "\nList of commands:\n"
        "    #set_size [1-%d] [1-%d] - Set the number of lines and columns the display has\n"
        "    #init 1/2 8/11 - Initialise the screen in (1)/(2) line mode with 5x(8) or 5x(11) font\n"
        "    #set 0/1 0/1 0/1 - Set whether the display, cursor, and blinking are on (1) or off (0)\n"
        "    #clear - Clear the screen of all characters and return the cursor to the start position\n"
        "    #home - Return the cursor to the start position\n"
        "    #scroll c/s l/r - Scroll the (c)ursor/(s)creen (l)eft/(r)ight\n"
        "    #backlight 0/1 - Set the screen backlight on (1) or off (0)\n"
        "    #def_custom [0-7] <char> - Define a custom character at index 0-7\n"
        "        <char> is 8, 5-bit binary numbers separated by spaces\n"
        "    #write_custom [0-7] - Write the custom character at index 0-7\n"
        "    #read_custom [0-7] - Get the pixel data of the custom character at index 0-7\n"
        "    #newline - Move the cursor to the start of the next line\n"
        "    #setpos [1-%d] [0-%d] - Set the position of the cursor to a given line, at a 0-based offset\n"
        "    #getpos - Get the position of the cursor\n"
        "    #read - Read the text currently on the screen\n"
        "    #raw_tx 0/1 <data> - (ADVANCED) Transmit raw data to the LCD module, with RS pin on (1) or off (0)\n"
        "        <data> is an 8-bit binary number, going from D7-D0\n"
        "    #raw_rx 0/1 - (ADVANCED) Receive raw data from the LCD module, with RS pin on (1) or off (0)\n",
        LCD_SCREEN_MAX_HEIGHT, LCD_SCREEN_MAX_WIDTH, size->height, size->width - 1
    );
}

static void command_set_size(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 2) {
        printf("The #set_size command requires two arguments.\n");
        return;
    }

    uint8_t height;
    if (strlen(argv[0]) == 1) {
        // Convert ASCII digit to integer
        height = argv[0][0] - '0';
    } else {
        printf("The first argument to the #set_size command must be 1 digit long.\n");
        return;
    }
    if (height < 1 || height > 4) {
        printf("The first argument to the #set_size command must be between 1 and 4.\n");
        return;
    }

    uint8_t width;
    if (strlen(argv[1]) == 1) {
        // Convert ASCII digit to integer
        width = argv[1][0] - '0';
    } else if (strlen(argv[1]) == 2) {
        // Convert ASCII digits to integer
        width = ((argv[1][0] - '0') * 10) + argv[1][1] - '0';
    } else {
        printf("The second argument to the #set_size command must be 1 or 2 digits long.\n");
        return;
    }
    if (width < 1 || width > 20) {
        printf("The second argument to the #set_size command must be between 1 and 20.\n");
        return;
    }

    size->height = height;
    size->width = width;
}

static void command_init(int argc, char *argv[]) {
    if (argc != 2) {
        printf("The #init command requires two arguments.\n");
        return;
    }

    bool lines;
    if (strcmp("1", argv[0]) == 0) {
        lines = false;
    } else if (strcmp("2", argv[0]) == 0) {
        lines = true;
    } else {
        printf("The first argument to the #init command must be 1 or 2.\n");
        return;
    }

    bool font;
    if (strcmp("8", argv[1]) == 0) {
        font = false;
    } else if (strcmp("11", argv[1]) == 0) {
        font = true;
    } else {
        printf("The second argument to the #init command must be 8 or 11.\n");
        return;
    }

    lcd_initialise_display(lines, font);
}

static void command_set(int argc, char *argv[]) {
    if (argc != 3) {
        printf("The #set command requires three arguments.\n");
        return;
    }

    bool display;
    if (strcmp("0", argv[0]) == 0) {
        display = false;
    } else if (strcmp("1", argv[0]) == 0) {
        display = true;
    } else {
        printf("The first argument to the #set command must be 0 or 1.\n");
        return;
    }

    bool cursor;
    if (strcmp("0", argv[1]) == 0) {
        cursor = false;
    } else if (strcmp("1", argv[1]) == 0) {
        cursor = true;
    } else {
        printf("The second argument to the #set command must be 0 or 1.\n");
        return;
    }

    bool blink;
    if (strcmp("0", argv[2]) == 0) {
        blink = false;
    } else if (strcmp("1", argv[2]) == 0) {
        blink = true;
    } else {
        printf("The third argument to the #set command must be 0 or 1.\n");
        return;
    }

    lcd_display_set(display, cursor, blink);
}

static void command_clear(int argc, char *argv[]) {
    if (argc != 0) {
        printf("The #clear command takes no arguments.\n");
        return;
    }

    lcd_clear();
}

static void command_home(int argc, char *argv[]) {
    if (argc != 0) {
        printf("The #home command takes no arguments.\n");
        return;
    }

    lcd_home();
}

static void command_scroll(int argc, char *argv[]) {
    if (argc != 2) {
        printf("The #scroll command requires two arguments.\n");
        return;
    }

    bool cursor_screen;
    if (strcmp("c", argv[0]) == 0) {
        cursor_screen = false;
    } else if (strcmp("s", argv[0]) == 0) {
        cursor_screen = true;
    } else {
        printf("The first argument to the #scroll command must be 1 or 2.\n");
        return;
    }

    bool left_right;
    if (strcmp("l", argv[1]) == 0) {
        left_right = false;
    } else if (strcmp("r", argv[1]) == 0) {
        left_right = true;
    } else {
        printf("The second argument to the #scroll command must be 8 or 11.\n");
        return;
    }

    lcd_scroll(cursor_screen, left_right);
}

static void command_backlight(int argc, char *argv[]) {
    if (argc != 1) {
        printf("The #backlight command requires one argument.\n");
        return;
    }

    bool backlight;
    if (strcmp("0", argv[0]) == 0) {
        backlight = false;
    } else if (strcmp("1", argv[0]) == 0) {
        backlight = true;
    } else {
        printf("The first argument to the #backlight command must be 0 or 1.\n");
        return;
    }

    lcd_backlight(backlight);
}

static void command_def_custom(int argc, char *argv[]) {
    if (argc != 9) {
        printf("The #def_custom command requires nine arguments.\n");
        return;
    }

    if (strlen(argv[0]) != 1) {
        printf("The first argument to the #def_custom command must be a single digit.\n");
        return;
    }
    // Convert ASCII digit to integer
    char character_index = argv[0][0] - '0';
    if (character_index < 0 || character_index > 7) {
        printf("The first argument to the #def_custom command must be between 0 and 7.\n");
        return;
    }

    uint8_t pixels[8];
    for (int i = 0; i < 8; i++) {
        char *binary = argv[i + 1];
        if (strlen(binary) != 5) {
            printf("Each binary argument to the #def_custom command must be five digits long.\n");
            return;
        }

        uint8_t row = 0;
        for (int j = 0; j < 5; j++) {
            if (binary[j] != '0' && binary[j] != '1') {
                printf("Each binary digit to the #def_custom command must be either 0 or 1.\n");
                return;
            }
            // Repeatedly add string binary digits to integer value
            row <<= 1;
            if (binary[j] == '1') {
                row |= 1;
            }
        }

        pixels[i] = row;
    }

    lcd_define_custom_char((uint8_t)character_index, pixels);
}

static void command_write_custom(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 1) {
        printf("The #write_custom command requires one argument.\n");
        return;
    }

    if (strlen(argv[0]) != 1) {
        printf("The first argument to the #write_custom command must be a single digit.\n");
        return;
    }
    // Convert ASCII digit to integer
    char character_index = argv[0][0] - '0';
    if (character_index < 0 || character_index > 7) {
        printf("The first argument to the #write_custom command must be between 0 and 7.\n");
        return;
    }

    char str_to_write[2] = {0};
    str_to_write[0] = character_index + 1;  // lcd_write uses 1-based indexing
    lcd_write(*size, str_to_write);
}

static void command_read_custom(int argc, char *argv[]) {
    if (argc != 1) {
        printf("The #read_custom command requires one argument.\n");
        return;
    }

    if (strlen(argv[0]) != 1) {
        printf("The first argument to the #read_custom command must be a single digit.\n");
        return;
    }
    // Convert ASCII digit to integer
    char character_index = argv[0][0] - '0';
    if (character_index < 0 || character_index > 7) {
        printf("The first argument to the #read_custom command must be between 0 and 7.\n");
        return;
    }

    uint8_t pixels[8];
    lcd_get_custom_char(character_index, pixels);
    for (int i = 0; i < 8; i++) {
        printf("%05b ", pixels[i]);
    }
    putchar('\n');
}

static void command_newline(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 0) {
        printf("The #newline command takes no arguments.\n");
        return;
    }

    lcd_write(*size, "\n");
}

static void command_setpos(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 2) {
        printf("The #setpos command requires two arguments.\n");
        return;
    }

    uint8_t line;
    if (strlen(argv[0]) == 1) {
        // Convert ASCII digit to integer
        line = argv[0][0] - '0';
    } else {
        printf("The first argument to the #setpos command must be 1 digit long.\n");
        return;
    }
    if (line < 1 || line > size->height) {
        printf("The first argument to the #setpos command must be between 1 and %d.\n", size->height);
        return;
    }
    // Convert to 0-indexed
    line--;

    uint8_t offset;
    if (strlen(argv[1]) == 1) {
        // Convert ASCII digit to integer
        offset = argv[1][0] - '0';
    } else if (strlen(argv[1]) == 2) {
        // Convert ASCII digits to integer
        offset = ((argv[1][0] - '0') * 10) + argv[1][1] - '0';
    } else {
        printf("The second argument to the #setpos command must be 1 or 2 digits long.\n");
        return;
    }
    if (offset < 0 || offset > size->width - 1) {
        printf("The second argument to the #setpos command must be between 0 and %d.\n", size->width - 1);
        return;
    }

    lcd_set_cursor_position(
        (struct LCDPosition){.line = line, .offset = offset});
}

static void command_getpos(int argc, char *argv[]) {
    if (argc != 0) {
        printf("The #getpos command takes no arguments.\n");
        return;
    }

    struct LCDPosition position = lcd_get_cursor_position();
    printf("line: %d, offset: %d\n", position.line + 1, position.offset);
}

static void command_read(int argc, char *argv[], struct LCDSize *size) {
    if (argc != 0) {
        printf("The #read command takes no arguments.\n");
        return;
    }

    char string[LCD_STRING_MAX_CHARS];
    lcd_read(*size, string);
    for (int i = 0; i < strlen(string); i++) {
        char c = string[i];
        if (c >= 1 && c <= 8) {
            // Convert custom character to printable placeholder
            c = '#';
        }
        putchar(c);
    }
    putchar('\n');
}

static void command_raw_tx(int argc, char *argv[]) {
    if (argc != 2) {
        printf("The #raw_tx command requires two arguments.\n");
        return;
    }

    bool rs_pin;
    if (strcmp("0", argv[0]) == 0) {
        rs_pin = false;
    } else if (strcmp("1", argv[0]) == 0) {
        rs_pin = true;
    } else {
        printf("The first argument to the #raw_tx command must be 0 or 1.\n");
        return;
    }

    if (strlen(argv[1]) != 8) {
        printf("The second argument to the #raw_tx command must be eight digits long.\n");
        return;
    }
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        if (argv[1][i] != '0' && argv[1][i] != '1') {
            printf("Each binary digit to the #raw_tx command must be either 0 or 1.\n");
            return;
        }
        // Repeatedly add string binary digits to integer value
        data <<= 1;
        if (argv[1][i] == '1') {
            data |= 1;
        }
    }

    lcd_transmit_data(rs_pin, data);
}

static void command_raw_rx(int argc, char *argv[]) {
    if (argc != 1) {
        printf("The #raw_rx command requires one argument.\n");
        return;
    }

    bool rs_pin;
    if (strcmp("0", argv[0]) == 0) {
        rs_pin = false;
    } else if (strcmp("1", argv[0]) == 0) {
        rs_pin = true;
    } else {
        printf("The first argument to the #raw_rx command must be 0 or 1.\n");
        return;
    }

    uint8_t data = lcd_receive_data(rs_pin, true);
    printf("%08b (0x%02x) (%d)\n", data, data, data);
}

int main() {
    stdio_init_all();

    lcd_init_gpio();

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n");

    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    char *buffer_end = input_buffer + INPUT_BUFFER_SIZE - 1;

    struct LCDSize lcd_size = (struct LCDSize){.width = 16, .height = 2};

    while (true) {
        printf(PROMPT_STR);

        char *buffer_ptr = input_buffer;
        while (true) {
            char c = getchar();

            if (c == '\x7f' || c == '\b') {
                // '\x7f' is ASCII delete - user pressed backspace key.
                // Decrement buffer so it is overwritten by next keypress.
                if (buffer_ptr > input_buffer) {
                    --buffer_ptr;
                    putchar(c);
                }
                continue;
            }

            // Echo typed character so user can see what they're typing
            putchar(c);

            if (c == '\r' || c == '\n' || buffer_ptr == buffer_end) {
                // '\r' or '\n' represents user pressing Enter key
                // - stop taking input and process what we have
                putchar('\n');
                break;
            }

            *buffer_ptr++ = c;
        }
        // Null terminate the input string
        *buffer_ptr = '\0';

        if (strnlen(input_buffer, INPUT_BUFFER_SIZE) == 0) {
            printf("You must enter either a command or text to write to the screen.\n");
            continue;
        }
        
        if (input_buffer[0] == '#') {
            // Command
            // Split command into individual components
            char *command = strtok(input_buffer, " ");
            char *argv[MAX_ARGS];
            int argc = 0;
            char *arg = command;
            while (argc < MAX_ARGS && (arg = strtok(NULL, " ")) != NULL) {
                argv[argc++] = arg;
            }

            if (strcmp(command, "#help") == 0) {
                command_help(argc, argv, &lcd_size);
            } else if (strcmp(command, "#set_size") == 0) {
                command_set_size(argc, argv, &lcd_size);
            } else if (strcmp(command, "#init") == 0) {
                command_init(argc, argv);
            } else if (strcmp(command, "#set") == 0) {
                command_set(argc, argv);
            } else if (strcmp(command, "#clear") == 0) {
                command_clear(argc, argv);
            } else if (strcmp(command, "#home") == 0) {
                command_home(argc, argv);
            } else if (strcmp(command, "#scroll") == 0) {
                command_scroll(argc, argv);
            } else if (strcmp(command, "#backlight") == 0) {
                command_backlight(argc, argv);
            } else if (strcmp(command, "#def_custom") == 0) {
                command_def_custom(argc, argv);
            } else if (strcmp(command, "#write_custom") == 0) {
                command_write_custom(argc, argv, &lcd_size);
            } else if (strcmp(command, "#read_custom") == 0) {
                command_read_custom(argc, argv);
            } else if (strcmp(command, "#newline") == 0) {
                command_newline(argc, argv, &lcd_size);
            } else if (strcmp(command, "#setpos") == 0) {
                command_setpos(argc, argv, &lcd_size);
            } else if (strcmp(command, "#getpos") == 0) {
                command_getpos(argc, argv);
            } else if (strcmp(command, "#read") == 0) {
                command_read(argc, argv, &lcd_size);
            } else if (strcmp(command, "#raw_tx") == 0) {
                command_raw_tx(argc, argv);
            } else if (strcmp(command, "#raw_rx") == 0) {
                command_raw_rx(argc, argv);
            } else {
                printf("\"%s\" is not a recognised command. Run #help to see all available commands.\n", command);
            }
        } else {
            // Text
            lcd_write(lcd_size, input_buffer);
        }
    }
}
