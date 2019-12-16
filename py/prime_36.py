#!/usr/bin/env pyton3


import primesieve
import numpy as np



it = primesieve.Iterator()
prime = it.next_prime()

_s = ''
_global_offset = 0

while prime < 2500000000:
    prime36 = np.base_repr(prime, 36)
    print(prime36)
    prime = it.next_prime()





