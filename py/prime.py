#!/usr/bin/env pyton3


import primesieve
import numpy as np
import base36



it = primesieve.Iterator()
prime = it.next_prime()

_s = ''
_global_offset = 0

while prime < 5000000000:
    prime36 = np.base_repr(prime, 36)
    #prime36 = base36.dumps(prime)

    _s += prime36
    if len(_s) >= 15:
        start_i = 0
        while start_i + 15 <= len(_s):
            #print(_s[start_i:start_i+15])
            success = (_s[start_i] == _s[start_i + 14] and
                      _s[start_i + 1] == _s[start_i + 13] and
                      _s[start_i + 2] == _s[start_i + 12] and
                      _s[start_i + 3] == _s[start_i + 11] and
                      _s[start_i + 4] == _s[start_i + 10] and
                      _s[start_i + 5] == _s[start_i + 9] and
                      _s[start_i + 6] == _s[start_i + 8])
            if success:
                print('success', _global_offset + start_i, _s[start_i:start_i+15])
                quit(0)
            start_i += 1
        _global_offset += len(_s) - 14
        _s = _s[-14:]

    prime = it.next_prime()

print('fail')
print(prime, prime36, len(_s))





