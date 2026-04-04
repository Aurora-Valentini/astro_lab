Per compilare questo esercizio è necessario scegliere il metodo che si vuole 
utilizzare per calcolare la densita' sulla griglia. 

make ngp
make cic
make tsc

Si noti che se si compila solo con make di default il sistema utilizzera' 
il metodo NGP. 

In seguito avviare il programma con:
./main_pm parameters.dat

Verranno mostrati su terminale dei messaggi per monitorare la creazione dei files e la fine dell'esecuzione. 

Si possono eseguire due simulazioni leggermente diverse con lo stesso programma. Nel file parameters.dat si puo' impostare dt_flag=1 per timestep adattivo o dt_flag=0 per timestep fisso.

Di conseguenza si possono visualizzare i grafici relativi alle tre fasi salienti della simulazione: Regime lineare (iniziale), Shell Crossing e Regime non lineare (finale). Si utilizzi a seconda del metodo scelto una delle seguenti per il plotting:
gnuplot animation_adapt.plt 
gnuplot animation.plt

Dopo ogni run eliminare i file generati tramite: make clean
