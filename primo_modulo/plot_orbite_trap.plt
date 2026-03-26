# Digitando da terminale "gnuplot plot_orbite_trap.plt" non occorre
# riscrivere tutto ogni volta per visualizzare le orbite del sistema solare. 

# Impostazioni grafiche
set title "Simulazione Orbitale - Sistema Trappist-1"
set xlabel "x [AU]"
set ylabel "y [AU]"
set grid
set size ratio -1
set terminal wxt size 800,800

# Limiti degli assi (per centrare il pianeta a)
set xrange [-0.05:0.05]
set yrange [-0.05:0.05]

# Disegna il pianeta a al centro
set object 1 circle at 0,0 size 0.001 fillcolor rgb "black" fillstyle solid noborder 

# Plot dei dati
# Colonna 2 = x, Colonna 3 = y
plot "Trappist_Planet_b.txt" u 2:3 w l title "Pianeta b" lc rgb "yellow", \
     "Trappist_Planet_c.txt" u 2:3 w l title "Pianeta c"   lc rgb "orange", \
     "Trappist_Planet_d.txt" u 2:3 w l title "Pianeta d"    lc rgb "red", \
     "Trappist_Planet_e.txt" u 2:3 w l title "Pianeta e"    lc rgb "purple", \
     "Trappist_Planet_f.txt" u 2:3 w l title "Pianeta f"    lc rgb "blue"

# Pausa per tenere aperta la finestra su alcuni sistemi
pause -1 "Premi Invio per chiudere"