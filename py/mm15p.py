#!/usr/bin/env pyton3

# 15 palindrom search in ASCII file
# Mmap + multiprocess implementation


import sys, os
from multiprocessing import Pool
import mmap

"""
def find_15_p(_mm, offset, size):
    start_i = 0
    while start_i + 15 <= size:
        if (_mm[start_i] == _mm[start_i + 14] and
           _mm[start_i + 1] == _mm[start_i + 13] and
           _mm[start_i + 2] == _mm[start_i + 12] and
           _mm[start_i + 3] == _mm[start_i + 11] and
           _mm[start_i + 4] == _mm[start_i + 10] and
           _mm[start_i + 5] == _mm[start_i + 9] and
           _mm[start_i + 6] == _mm[start_i + 8]):
            return start_i + offset, _mm[start_i:start_i+15].decode('utf-8')
        start_i += 1
    return None, None
"""

def find_15_p(data, offset):
    start_i = 0
    while start_i + 15 <= len(data):
        if (data[start_i] == data[start_i + 14] and
           data[start_i + 1] == data[start_i + 13] and
           data[start_i + 2] == data[start_i + 12] and
           data[start_i + 3] == data[start_i + 11] and
           data[start_i + 4] == data[start_i + 10] and
           data[start_i + 5] == data[start_i + 9] and
           data[start_i + 6] == data[start_i + 8]):
            return start_i + offset, data[start_i:start_i+15]
        start_i += 1
    return None, None


def run_with_mmap(mm):
    pool = Pool(processes=7)
    results = []

    _input_offset = 0
    batch_size = 1024 * 1024 * 16
    while _input_offset < mm.size():

        results.append(pool.apply_async(find_15_p, (mm[_input_offset:_input_offset+batch_size+14], _input_offset)))

        _input_offset += batch_size

    print('input complete')

    #pool.close()
    #pool.join()

    for r in results:
        p = r.get()
        if p[0] is not None:
            print('SUCCESS\n', p[0], p[1])
            quit(0)

    print('FAIL\n', _input_offset)

with open(sys.argv[1], "r+b") as f:
    mm = mmap.mmap(f.fileno(), 0)
    run_with_mmap(mm)






