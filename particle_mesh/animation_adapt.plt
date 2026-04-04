# Questo script serve per plottare i risultati con dt_flag=1 ossia per timestep adattivo.

set terminal wxt size 1200,600
# Parametri fissi
L = 50.0 

# --- STEP 0010 ---
set multiplot layout 1,2 title "{/*1.2 Regime lineare}"
    # Grafico 1: Spazio delle Fasi
    set size square
    set title "Spazio delle Fasi "
    set xlabel "Posizione (x)"
    set ylabel "Velocita' (v)"
    set xrange [0:L]; set yrange [*:*]; set grid
    plot NaN with lines lc rgb "blue" lw 2 title "v(x)", \
        "phase_step_0010.txt" every 10 u 1:2 with lines lc rgb "blue" notitle

    # Grafico 2: Densita'
    set size square
    set title "Profilo di Densita'"
    set xlabel "Posizione (x)"
    set ylabel "Contrasto di densita' (delta)"
    set xrange [0:L]; set yrange [*:*]
    plot "rho_step_0010.txt" u 1:2 with lines lc rgb "red" title "delta(x)"
unset multiplot

print "Premere INVIO nel terminale per lo step successivo..."
pause -1 

# --- STEP 0050 ---
set multiplot layout 1,2 title "{/*1.2 Shell Crossing}"
    # Grafico 1: Spazio delle Fasi
    set size square
    set title "Spazio delle Fasi"
    set xlabel "Posizione (x)"
    set ylabel "Velocita' (v)"
    set xrange [0:L]; set yrange [*:*]
    plot NaN with lines lc rgb "blue" title "v(x)", \
        "phase_step_0050.txt" every 10 u 1:2 with dots lc rgb "blue" notitle

    # Grafico 2: Densita'
    set size square
    set title "Profilo di Densita'"
    set xlabel "Posizione (x)"
    set ylabel "Contrasto di densita' (delta)"
    set xrange [0:L]; set yrange [*:*]
    plot "rho_step_0050.txt" u 1:2 with lines lc rgb "red" title "delta(x)"
unset multiplot

print "Premere INVIO nel terminale per lo step successivo..."
pause -1

# --- STEP 0150 ---
set multiplot layout 1,2 title "{/*1.2 Regime non lineare}"
    # Grafico 1: Spazio delle Fasi
    set size square
    set title "Spazio delle Fasi"
    set xlabel "Posizione (x)"
    set ylabel "Velocita' (v)"
    set xrange [0:L]; set yrange [*:*]
    plot NaN with lines lc rgb "blue" title "v(x)", \
        "phase_step_0150.txt" every 10 u 1:2 with dots lc rgb "blue" notitle

    # Grafico 2: Densita'
    set size square
    set title "Profilo di Densita'"
    set xlabel "Posizione (x)"
    set ylabel "Contrasto di densita' (delta)"
    set xrange [0:L]; set yrange [-1:*]
    plot "rho_step_0150.txt" u 1:2 with lines lc rgb "red" title "delta(x)"
unset multiplot

pause -1 "Premere INVIO per chiudere."