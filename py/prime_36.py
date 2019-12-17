#!/usr/bin/env pyton3

import os
import numpy as np
from primesieve.numpy import *


a = primes(5*1000*1000*1000)


base36_table = b'0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

def base36_encode_bytes(number):
    global base36_table
    base36 = b''

    while number:
        number, i = divmod(number, 36)
        base36 = bytes([base36_table[i]]) + base36

    return base36 or b'0'

for v in a:
    os.write(1, base36_encode_bytes(v))





