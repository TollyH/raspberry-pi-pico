add_executable(uart_lcd
    uart_lcd.c
)

# pull in common dependencies and additional uart hardware support
target_link_libraries(uart_lcd pico_stdlib hardware_uart)

# enable usb output and uart output
pico_enable_stdio_usb(uart_lcd 1)
pico_enable_stdio_uart(uart_lcd 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(uart_lcd)
