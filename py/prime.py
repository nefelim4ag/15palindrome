#!/usr/bin/env pyton3


import os
import numpy as np
from primesieve.numpy import *


a = primes(5*1000*1000*1000)

os.write(1, a.tobytes())


