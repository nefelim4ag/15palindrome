#!/usr/bin/env pyton3

import sys, os
import array


base36_table = b'0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

def base36_encode_bytes(number):
    base36 = b''

    while number:
        number, i = divmod(number, 36)
        base36 = bytes([base36_table[i]]) + base36

    return base36 or b'0'


base36p2_table = b''
for i in range(36*36):
    if i < 36:
        base36p2_table += b'0'
    base36p2_table += base36_encode_bytes(i)


def base36p2_encode_bytes(number):
    base36 = b''

    if number >= 36:
        while number:
            number, i = divmod(number, 36*36)
            if number > 0 or i >= 36:
                base36 = base36p2_table[i*2:i*2+2] + base36
            else:
                base36 = bytes([base36_table[i]]) + base36
        return base36
    else:
        return bytes([base36_table[number]])


def base36p2_encode_bytes_r(number):
    number, i = divmod(number, 36*36)

    if number == 0:
        if i >= 36:
            return base36p2_table[i*2:i*2+2]
        else:
            return bytes([base36_table[i]])
    else:
        return base36p2_encode_bytes_r(number) + base36p2_table[i*2:i*2+2]


#for line in sys.stdin:
#    os.write(1, base36_encode_bytes(int(line)))
#quit(0)

read_buffer_size = 1024 * 64

while True:
    data = os.read(0, read_buffer_size)
    if len(data) == 0:
        break
    
    output = b''
    a = array.array('Q', data)
    #print(sum(a))

    for v in a:
        """
        if base36p2_encode_bytes_r(v) != base36_encode_bytes(v):
            print(v)
            print(base36_encode_bytes(v))
            print(base36p2_encode_bytes_r(v))
            quit(1)
        """

        output += base36p2_encode_bytes_r(v)
    try:
        os.write(1, output)
    except:
        break


