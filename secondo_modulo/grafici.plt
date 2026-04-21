reset
set grid
set key outside

# Non considero i grafici delle condizioni iniziali 
n_snap = 4
dt = 0.05

set xlabel "x"
set ylabel "rho"

# ------------------------------
# 1. CONDIZIONI INIZIALI
# ------------------------------
set title "Condizioni iniziali"
plot "IC_plot.txt" using 1:2 with points pt 7 ps 0.3 title "ρ(x)"


print "Condizioni iniziali -> premere INVIO per continuare"
pause -1


# ------------------------------
# 2. RICERCA DEI VICINI (QS)
# ------------------------------
set title "Ricerca dei vicini (Quicksort)"
plot "quicksort_plot.txt" using 1:2 with points pt 7 ps 0.3 title "ρ(x)"

print "Ricerca dei vicini (Quicksort)-> premere INVIO per continuare"
pause -1


# ------------------------------
# 4. EVOLUZIONE TEMPORALE (QS)
# ------------------------------
do for [i=0:n_snap-1] {

    filename = sprintf("snapshot_qs_%03d.txt", i)
    time = (i+1)*dt

    set title sprintf("SPH 1D - t = %.2f", time)

    plot \
        filename using 1:2 with linespoints lw 1 pt 7 ps 0.3 title "densità (ρ)", \
        filename using 1:3 with linespoints lw 1 pt 7 ps 0.3 title "pressione (P)", \
        filename using 1:5 with linespoints lw 1 pt 7 ps 0.3 title "velocità (v)"

    print sprintf("Snapshot QS (t=%.2f) -> premere INVIO per continuare", time)
    pause -1
}

# ------------------------------
# 3. DOPO NEIGHBOR SEARCH (LL)
# ------------------------------
set title "Ricerca dei vicini (Linked List)"
plot "linked_list_plot.txt" using 1:2 with points pt 7 ps 0.3 title "ρ(x)"

print "Ricerca dei vicini (Linked List)-> premere INVIO per continuare"
pause -1


# ------------------------------
# 5. EVOLUZIONE TEMPORALE (LL)
# ------------------------------
do for [i=0:n_snap-1] {

    filename = sprintf("snapshot_ll_%03d.txt", i)
    time = (i+1)*dt

    set title sprintf("SPH 1D - t = %.2f", time)

    plot \
        filename using 1:2 with linespoints lw 1 pt 7 ps 0.3 title "densità (ρ)", \
        filename using 1:3 with linespoints lw 1 pt 7 ps 0.3 title "pressione (P)", \
        filename using 1:5 with linespoints lw 1 pt 7 ps 0.3 title "velocità (v)"

    print sprintf("Snapshot LL (t=%.2f) -> premere INVIO per continuare", time)
    pause -1
}

# ------------------------------
# 6. CONFRONTO FINALE
# ------------------------------
set title "Confronto finale"
plot "final_qsort.txt" using 1:2 with points pt 7 ps 0.3 title "QS", \
     "final_linked_list.txt" using 1:2 with points pt 5 ps 0.3 title "LL"

print "Confronto finale -> premere INVIO per chiudere"
pause -1

print "Fine della simulazione."