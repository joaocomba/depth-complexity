#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Usage:
# this should be called by plotHistograms.sh

import sys
import os
import re
import numpy
from collections import defaultdict

model, lower, upper, exec_name = sys.argv[1:5]

lower = int(lower)
upper = int(upper)

histogram = defaultdict(lambda: [0] * (upper + 1 - lower))

for i in range(lower, upper + 1):
    os.system('{:s} -f {:s} -dsteps {:d} -fh histogram.txt'.format(exec_name, model, i))
    total = 0
    for line in open('histogram.txt'):
        found = re.findall(r'\d+',line)
        if found:
            index = int(found[0])
            val = numpy.float64(found[1])
            total += val
            histogram[index][i-lower] = val
    for k in histogram:
        histogram[k][i-lower] /= total

with open('toPlot.txt','w') as f:
    for k, v in sorted(histogram.items()):
        f.write('{:d} {:s}\n'.format(k, ' '.join(str(numpy.percentile(v, i / 4.0)) for i in range(5))))

exit(max(histogram))
