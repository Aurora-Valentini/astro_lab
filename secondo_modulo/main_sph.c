//
//  ESERCIZIO SPH - Smoothed Particles Hydrodynamics
//
//  Si realizzi una simulazione SPH unidimensionale per il caso
//  test dello shock-tube. A differenza delle implementazioni 
//  standard a raggio fisso, si richiede una lunghezza di smoothing
//  adattiva h_i, variabile per ogni particella in base alla densita' locale
//  e l'ottimizzazione della ricerca dei vicini secondo due metodi: quicksort e cell linked list.
//
//  Created by Aurora Valentini on 16/03/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include "physic.h"


int main() {
    Parameter param;

    // -------------- CONDIZIONI INIZIALI -----------
    
    // Leggo i parametri dal file .dat
    leggi_param("parameters.dat", &param);
    
    // Calcolo i parametri aggiuntivi 
    calcola_param(&param); 
    // Per Controllare che i parametri letti e calcolati siano corretti
    // printf("DEBUG: n_totale = %d\n", param.n_totale);

    // Alloco la memoria per l'array di particelle.
    Particle *part = (Particle *)malloc(param.n_totale * sizeof(Particle));

    // Imposto le condizioni iniziali (pos, rho, P, u, vel)
    init_cond(part, &param);
    
    // Salvo il risultato per il plot
    salva_evol(part, &param, "IC_plot.txt");

    printf("Checkpoint 1: Condizioni iniziali generate.\n");
    printf("Per verificare usare Gnuplot: \n");
    printf("plot \"IC_plot.txt\" using 1:2 with p ps 0.1 -> rho(x) vs x\n");
    //printf("plot \"IC_plot.txt\" using 1:3 with p ps 0.1 -> p(x) vs x\n");
    //printf("plot \"IC_plot.txt\" using 1:4 with p ps 0.1 -> u(x) vs x\n");

    // --- RICERCA DEI VICINI con QUICKSORT, CALCOLO DI DENSITA, PRESSIONE e FORZA ---
    
    
    // Ordino l'array di particelle lungo l'asse x
    qsort(part, param.n_totale, sizeof(Particle), compare_part);

    // Calcolo la Smoothing Length h_i
    update_h(part, &param);

    param.head = (Particle **)malloc(param.n_cell * sizeof(Particle *));

    //CHECK
    if (part == NULL || param.head == NULL) {
        fprintf(stderr, "Errore: Memoria insufficiente!\n");
        return 1;
    }

    // Calcolo la densità e la pressione usando il Kernel_W 
    density_press_qs(part, &param);
    accel_qs(part, &param);
    salva_evol(part, &param, "quicksort_plot.txt");
    printf("Checkpoint 2: Ricerca dei vicini tramite quicksort.\n");
    printf("Per verificare usare Gnuplot: \n");
    printf("plot \"quicksort_plot.txt\" using 1:2 with p ps 0.1 -> rho(x) vs x\n");
    

    // --- RICERCA DEI VICINI con CELL-LINKED LIST, CALCOLO DI DENSITA, PRESSIONE e FORZA ---
    qsort(part, param.n_totale, sizeof(Particle), compare_part);
    update_h(part, &param);
    linked_list(part, &param);
    density_press_ll(part, &param);
    accel_ll(part, &param);
    salva_evol(part, &param, "linked_list_plot.txt");
    printf("Checkpoint 3: Ricerca dei vicini tramite linked list.\n");
    printf("Per verificare usare Gnuplot: \n");
    printf("plot \"linked_list_plot.txt\" using 1:2 with p ps 0.1 -> rho(x) vs x\n");

    // --------------- EVOLUZIONE TEMPORALE (LEAPFROG) ---------------------

    double t = 0.0;                    // Tempo iniziale
    double t_end = param.t_max;       // Tempo finale 

    printf("Inizio simulazione...\n");
    clock_t start_qs = clock(); // Avvio il cronometro qui per simulazione con quicksorting
    double output_interval = 0.05; // Intervalli per salvare gli snapshot temporali
    double next_output_time = output_interval;

    int snapshot_id = 0;

    while (t < t_end) {       // Finché non arrivo al tempo finale...
        
        // Calcolo quanto deve essere lungo il passo
        double dt = timestep_comp(part, &param); 
    
        // Se l'ultimo passo mi fa superare t_end, lo accorcio
        if (t + dt > t_end) dt = t_end - t;

        leapfrog(part, &param, dt, 0); // Passo metodo ==0 -> USO QS
        t += dt;                       // Aggiorno il tempo 

        if (t >= next_output_time) {
            char filename[100];
            sprintf(filename, "snapshot_qs_%03d.txt", snapshot_id); // Salvo gli snapshot temporali per il plotting
        
            salva_evol(part, &param, filename);
        
            next_output_time += output_interval;
            snapshot_id++;
        }

    }

    clock_t end_qs = clock();
    double time_qs = (double)(end_qs - start_qs) / CLOCKS_PER_SEC;

    printf("Simulazione 'Quicksort' completata in %f secondi.\n", time_qs);

    // --- SECONDA SIMULAZIONE: LINKED LIST ---
    init_cond(part, &param); // Inizializzo tutto di nuovo 
    t = 0.0;
    clock_t start_ll = clock();


    double next_output_time_ll = output_interval;
    int snapshot_id_ll = 0;
    
    while (t < param.t_max) {
        double dt = timestep_comp(part, &param);
        if (t + dt > param.t_max) dt = param.t_max - t;

        leapfrog(part, &param, dt, 1); // Passo 1 -> USO LL
        t += dt;

        if (t >= next_output_time_ll) {
            char filename[100];
            sprintf(filename, "snapshot_ll_%03d.txt", snapshot_id_ll);
        
            salva_evol(part, &param, filename);
        
            next_output_time_ll += output_interval;
            snapshot_id_ll++;
        }
        
        
    }
    clock_t end_ll = clock();
    double time_ll = (double)(end_ll - start_ll) / CLOCKS_PER_SEC;
    printf("Simulazione 'Linked list' completata in %f secondi.\n", time_ll);

    // --- CONFRONTO FINALE ---
    printf("Speedup: %.2f\n", time_qs / time_ll);
    
    // Libero la memoria
    free(part);
    free(param.head);
    
    printf("Fine simulazione...\n");

    return 0;
}
