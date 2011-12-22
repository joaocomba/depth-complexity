# Usage:
# $ python plotHistograms.py model [lower_limit] upper_limit

import sys
import os
import re
import numpy

exec_name = sys.argv[4]
model = sys.argv[1]
lower = sys.argv[2]
try:
	upper = sys.argv[3];
except:
	upper = lower
	lower = 2
	threshold = 0
	
lower = int(lower)
upper = int(upper)

histogram = dict()

for i in range(lower,upper+1):
	os.system(exec_name+" -f "+model+" -dsteps "+str(i)+" -fh histogram.txt")
	total = 0.0
	for line in open('histogram.txt', 'r'):
		found = re.findall(r'\d+',line)
		if len(found) == 0:
			continue
		index = int(found[0])
		val = numpy.float64(found[1])
		total += val
		try:
			histogram[index].append(val)
		except:
			histogram[index] = list()
			histogram[index].append(val)
	for k in histogram.keys():
		histogram[k][len(histogram[k])-1] /= total

for i in histogram.keys():
	if len(histogram[i]) < upper-lower+1:
		for j in range(upper-lower+1-len(histogram[i])):
			histogram[i].append(0);

f = open('toPlot.txt','w')
for i in sorted(histogram.keys()):
	print >> f, i, numpy.percentile(histogram[i],0.0), numpy.percentile(histogram[i],0.25), numpy.percentile(histogram[i],0.5), numpy.percentile(histogram[i],0.75), numpy.percentile(histogram[i],1.0)
f.close()

exit(max(histogram.keys()))


