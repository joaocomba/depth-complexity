set terminal svg
set output "plot.svg"
set bars 2.0 front
set style fill solid 0.1 border lt -1
set title "Depth Complexity Histogram"
set xlabel "Depth Complexity" 
set ylabel "Frequency (in %)"
set zero 1e-20
plot "toPlot.txt" using 1:($4*100):($2*100):($6*100):(0.5) every ::var w boxerrorbars title 'Median Frenquency'

