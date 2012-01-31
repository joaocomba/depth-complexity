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

ABOUT TESTS FOLDER

Before using or creating results, beware that the current results are
available in:
vgchead.poly.edu:/home/vgc/share/scratch/depth-complexity/Tests.tar.gz
and should be extracted into depth-complexity/ (creating the folder
Tests) for use.

HISTOGRAM PLOTTER:

Generates histograms with maximum, mean and minimum number
of rays found with each number of intersections over runs
using discrete steps between lower_limt and upper_limit. If
a last argument is given, than the random test will also be
executed.

Usage:
>> plotHistograms.sh model lower_limit upper_limit [test_random]

DEPTH COMPLEXITY TESTER

Receives path to models and runs DC over them, respecting
given time limit (in seconds).

Usage:
>> doTest.sh model_dir time_limit

This generates the folder "Tests". Inside it, there is one folder per model and time limit used (the folders are named "model_time"). Inside the model_time folder are two folders, one for the tests using our approach, and one for the ones using the random approach, besides the "comp.txt" file. Inside those are all the tests made using the respective approach, model and time limit. Each test's results are inside a folder named with the number of discrete steps utilized to run the test. The results include a histogram of all found rays, and .off files with the rays with a high number of intersections.
The "comp.txt" file states the most intersections found using each approach.

.OFF FILE MERGER

Receives two files, both in the OFF format, and joins them
into one file.

Usage:
>> mergeOFF first_file second_file [output_file]

TEST ANALYZER

Receives a test folder (model_time) and produces histograms
comparing runs with different number of steps and algorithms.
If no_key is given, then there should be no key in the plot.
If only a certain range of discretization steps should be used,
this can be informed by editing the LIMIT variables inside the
script.
The scale of colors can be defined within the script, as well
as the size of the plot.
The output is in model_time_u.pdf and model_time_r.pdf.

Usage:
>> analyzeTests.sh model_time [no_key]
Example:
>> ./analyzeTests.sh hammer.obj_400 1


