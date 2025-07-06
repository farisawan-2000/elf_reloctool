## Considerations
 - Turn off `-ffunction-sections` and `-fdata-sections`.
 The output of `objdump -S your/obj.o` should not contain more PROGBITS sections than `.text`, `.data`, `.rodata`, and `.bss`

## TODO
 - Build a header format for symbols
 - Gracefully handle objects compiled with `-ffunction-sections` (if possible)
