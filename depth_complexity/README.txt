Executables:

DEPTH COMPLEXITY 2D (VIEWER)
>> ./depthcomplexity2d

DEPTH COMPLEXITY 3D (VIEWER)
>> ./depthcomplexity3d models/suzanne.obj

DEPTH COMPLEXITY 3D (VIEWER) USING RANDOM SAMPLING
>> ./randomdepthcomplexity3d models/suzanne.obj

OFFLINE 3D

See options:
>> ./depthcomplexity3d_offline -h

RANDOM OFFLINE 3D

See options:
>> ./randomdepthcomplexity3d_offline -h

All the parameters have a default value.

Examples:
Compute histograms and save intersection segments:
>> ./depthcomplexity3d_offline -h -fh "hist.txt" -fr "rays.off"

Save intersection segments with 7 intersections or more,
one set of segments per number of intersections:
>> ./depthcomplexity3d_offline -h -cmr true -it 7

HISTOGRAM PLOTTER:

Generates histograms with maximum, mean and minimum number
of rays found with each number of intersections over runs
using discrete steps between lower_limt and upper_limit.

Usage:
>> plotHistograms.sh model lower_limit upper_limit

DEPTH COMPLEXITY TESTER

Receives path to models and runs DC over them, respecting
given time limit (in seconds).

Usage:
>> doTest.sh model_dir time_limit

