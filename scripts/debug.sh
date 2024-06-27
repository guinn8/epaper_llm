#!/bin/bash

ELF_FILE="$1"
st-util &
arm-none-eabi-gdb "$ELF_FILE" \
    -ex "target extended-remote :4242" \
    -ex "set pagination off" \
    -ex "load" \
    -ex "set pagination on" \
    -ex "monitor reset halt"