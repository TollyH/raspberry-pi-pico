#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include <lcd_controller.h>

#define INPUT_BUFFER_SIZE 64
#define MAX_ARGS 32

#define PROMPT_STR "\n> "

static void command_help(int argc, char *argv[]) {
    if (argc != 0) {
        printf("The #help command takes no arguments.\n");
        return;
    }

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n"
        "Write any text not prefixed with # to write it to the display\n"
        "Use \\0 through \\7 to type a custom character, and \\n to type a newline\n"
        "\nList of commands:\n"
        "    #init 1/2 8/11 - Initialise the screen in (1)/(2) line mode with 5x(8) or 5x(11) font\n"
        "    #set 0/1 0/1 0/1 - Set whether the display, cursor, and blinking are on (1) or off (0)\n"
        "    #clear - Clear the screen of all characters and return the cursor to the start position\n"
        "    #home - Return the cursor to the start position\n"
        "    #scroll c/s l/r - Scroll the (c)ursor/(s)creen (l)eft/(r)ight\n"
        "    #backlight 0/1 - Set the screen backlight on (1) or off (0)\n"
        "    #custom [0-7] <char> - Define a custom character at index 0-7\n"
        "        <char> is 5, 8-bit binary numbers separated by spaces\n");
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
    } else{
        printf("The first argument to the #init command must be 1 or 2.\n");
        return;
    }

    bool font;
    if (strcmp("8", argv[1]) == 0) {
        font = false;
    } else if (strcmp("11", argv[1]) == 0) {
        font = true;
    } else{
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
    } else{
        printf("The first argument to the #set command must be 0 or 1.\n");
        return;
    }

    bool cursor;
    if (strcmp("0", argv[1]) == 0) {
        cursor = false;
    } else if (strcmp("1", argv[1]) == 0) {
        cursor = true;
    } else{
        printf("The second argument to the #set command must be 0 or 1.\n");
        return;
    }

    bool blink;
    if (strcmp("0", argv[2]) == 0) {
        blink = false;
    } else if (strcmp("1", argv[2]) == 0) {
        blink = true;
    } else{
        printf("The third argument to the #set command must be 0 or 1.\n");
        return;
    }

    lcd_display_set(display, cursor, blink);
}

static void command_clear(int argc, char *argv[]) {
    if (argc != 0) {
        printf("The #help command takes no arguments.\n");
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
    } else{
        printf("The first argument to the #scroll command must be 1 or 2.\n");
        return;
    }

    bool left_right;
    if (strcmp("l", argv[1]) == 0) {
        left_right = false;
    } else if (strcmp("r", argv[1]) == 0) {
        left_right = true;
    } else{
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

static void command_custom(int argc, char *argv[]) {
    if (argc != 6) {
        printf("The #custom command requires six arguments.\n");
        return;
    }

    if (strlen(argv[0]) != 1) {
        printf("The first argument to the #custom command must be a single digit.");
        return;
    }
    // Convert ASCII digit to integer
    uint8_t character_index = argv[0][0] - '0';
    if (character_index < 0 || character_index > 7) {
        printf("The first argument to the #custom command must be between 0 and 7.");
        return;
    }

    uint8_t pixels[5];
    for (int i = 1; i <= 5; i++) {
        const char *binary = argv[i];
        if (strlen(binary) != 8) {
            printf("Each binary argument to the #custom command must be eight digits long.");
            return;
        }

        uint8_t row = 0;
        for (int j = 0; j < 8; j++) {
            if (binary[j] != '0' && binary[j] != '1') {
                printf("Each binary digit to the #custom command must be either 0 or 1.");
                return;
            }
            // Repeatedly add string binary digits to integer value
            if (binary[j] == '1') {
                ++row;
            }
            row <<= 1;
        }

        pixels[i] = row;
    }

    lcd_define_custom_char(character_index, pixels);
}

int main() {
    stdio_init_all();

    lcd_init_gpio();

    printf("LCD <-> UART Controller. Commands start with #, i.e. \"#help\"\n");

    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    char *buffer_end = input_buffer + INPUT_BUFFER_SIZE - 1;

    while (true) {
        printf(PROMPT_STR);

        char *buffer_ptr = input_buffer;
        while (true) {
            char c = getchar();

            if (c == '\x7f') {
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

            if (c == '\r' || buffer_ptr == buffer_end) {
                // '\r' represents user pressing Enter key
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
            while ((arg = strtok(NULL, " ")) != NULL) {
                argv[argc++] = arg;
            }

            if (strcmp(command, "#help") == 0) {
                command_help(argc, argv);
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
            } else if (strcmp(command, "#custom") == 0) {
                command_custom(argc, argv);
            } else {
                printf("\"%s\" is not a recognised command. Run #help to see all available commands.\n", command);
            }
        } else {
            // Text
            lcd_write(input_buffer);
        }
    }
}