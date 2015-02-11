set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'time.eps'

set xlabel "{/Helvetica-Oblique ilosc watkow}"
set ylabel "{/Helvetica-Oblique t[s]}"

plot "speadup_plot.dat" u 1:2:3 w yerrorbars ps 0.5 pt 5 t "sredni rzeczywisty czas wykonania obliczen z zaznaczonym odchyleniem std"
