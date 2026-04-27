Questo esercizio implementa una simulazione SPH in 1 dimensione che prende il nome di problema di Sod.
Per compilare utilizzare il comando:

make run_sph

Da terminale si puo' tenere traccia dei processi che stanno avvenendo e dei file che vengono creati.

In seguito per vedere i grafici relativi si digiti da terminale:

gnuplot grafici.plt

Si aprira' una finestra interattiva che a partire dalle condizioni iniziali mostrera' vari snapshot della simulazione per quantità quali densita', pressione e velocita' in funzione della posizione. Occorre premere 'Invio' per passare alla visualizzazione del grafico successivo. 

Al termine dell'esecuzione eliminare i file generati con:

make clean
