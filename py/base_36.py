#!/usr/bin/env pyton3

import sys
#import numpy as np
#from numba import jit
#import string, math
#import base36

"""
for line in sys.stdin:
    print(np.base_repr(int(line), 36), end='')


        static const char jump_table[36] = {
                '0','1','2','3','4','5','6','7','8','9',
                'A','B','C','D','E','F','G','H','I','J',
                'K','L','M','N','O','P','Q','R','S','T',
                'U','V','W','X','Y','Z'
        };

        char *ptr_end = dest;
        size_t remainder;
        size_t num = fast_strtol(src);

        while (num > 36) {
                remainder = num % 36;
                *ptr_end = jump_table[remainder];
                ptr_end++;
                num = num / 36;
        }

        remainder = num % 36;
        *ptr_end = jump_table[remainder];

        if (num == 36) {
                ptr_end++;
                *ptr_end = jump_table[num / 36];
        }

        ptr_end++;
        *ptr_end = '\0';

        strrev(dest);

"""

jump_table = [  '0','1','2','3','4','5','6','7','8','9',
                'A','B','C','D','E','F','G','H','I','J',
                'K','L','M','N','O','P','Q','R','S','T',
                'U','V','W','X','Y','Z']

def base_10_to_36(num):
    global jump_table
    r = ''
    while num > 36:
        remainder = num % 36
        v = jump_table[remainder]
        r = v + r
        num = int(num / 36)
    r = jump_table[num % 36] + r
    if num == 36:
        r = jump_table[int(num / 36)] + r
    return r

"""
int2base = lambda a, b: ''.join(
    [(string.digits +
      string.ascii_uppercase)[(a // b ** i) % b]
     for i in range(int(math.log(a, b)), -1, -1)]
)
"""

#@jit(nopython=True)
def convert(s):
    #s1 = np.base_repr(int(line), 36)
    #s2 = base_10_to_36(line)
    #s3 = int2base(int(line), 36)
    #s3 = base36.dumps(int(line))
    #if s2 != s3:
    #    print(s2, s3)
    #    break
    #return np.base_repr(int(s), 36)
    return base_10_to_36(int(s))

def main():
    for line in sys.stdin:
        print(convert(line), end='')
        #print(line, end='')

main()
