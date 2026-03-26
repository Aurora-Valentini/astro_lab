# Digitando da terminale "gnuplot plot_orbite_sun.plt" non occorre
# riscrivere tutto ogni volta per visualizzare le orbite del sistema solare. 

# Impostazioni grafiche
set title "Simulazione Orbitale - Sistema solare interno"
set xlabel "x [AU]"
set ylabel "y [AU]"
set grid
set size ratio -1
set terminal wxt size 800,800

# Limiti degli assi (per centrare il Sole)
set xrange [-2.:2.]
set yrange [-2.:2.]

# Disegna il Sole al centro
set object 1 circle at 0,0 size 0.05 fillcolor rgb "yellow" fillstyle solid noborder 

# Plot dei dati
# Colonna 2 = x, Colonna 3 = y
plot "Solar_Mercury.txt" u 2:3 w l title "Mercurio" lc rgb "gray", \
     "Solar_Venus.txt"   u 2:3 w l title "Venere"   lc rgb "blue", \
     "Solar_Earth.txt"   u 2:3 w l title "Terra"    lc rgb "green", \
     "Solar_Mars.txt"    u 2:3 w l title "Marte"    lc rgb "red"

# Pausa per tenere aperta la finestra su alcuni sistemi
pause -1 "Premi Invio per chiudere"