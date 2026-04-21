Questo esercizio si propone di risolvere l'integrazione delle orbite per il sistema Solare interno e per il sistema Trappist-1.
Per compliare occorre utilizzare due comandi diversi a seconda del sistema scelto:

make run_solare
make run_trappist

Da terminale si puo' tenere traccia dei processi che stanno avvenendo e dei file che vengono generati.

In seguito per visualizzare i grafici rispettivamente del Sistema Solare o del sistema Trappist-1 si digiti da terminale:

gnuplot plot_orbite_sun.plt
gnuplot plot_orbite_trap.plt


Si aprira' una finestra che mostrera' le varie orbite.
Al termine dell'esecuzione eliminare i file generati con:

make clean
