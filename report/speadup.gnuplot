set terminal postscript eps enhanced color font 'Helvetica,10' size 5, 1.5
set output 'speadup.eps'

set xlabel "{/Helvetica-Oblique ilosc watkow}"
set ylabel "{/Helvetica-Oblique przyspieszenie}"

set yrange [0:7]

set size ratio -1

plot "speadup_plot.dat" u 1:4 w l t "przyspieszenie rzeczywiste", f(x)=x, f(x) t "przyspieszenie teoretyczne"
