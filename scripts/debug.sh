#!/bin/bash

ELF_FILE="build/nucleo_test.elf"
st-util &
arm-none-eabi-gdb "$ELF_FILE" \
    -ex "target extended-remote :4242" \
    -ex "set pagination off" \
    -ex "load" \
    -ex "monitor reset halt" \
    -ex "c"

