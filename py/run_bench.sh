#!/bin/sh

set +x

time python3 ./prime.py | pypy3 base_36.py | pv | pypy3 15p.py


# pv < /dev/zero > /dev/null
# cat /dev/zero | pv > /dev/null

# pypy3 pipe.py < /dev/zero | pv > /dev/null
# cat /dev/zero | pypy3 pipe.py | pv > /dev/null

# time python3 ./prime.py | pv > /dev/null

# time cat ../xxx.txt | pv | pypy3 15p.py

# time python3 ./prime.py | pypy3 base_36.py | pv > /dev/null



