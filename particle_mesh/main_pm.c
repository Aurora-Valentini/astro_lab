//
//  ESERCIZIO PARTICLE MESH
//
//  Si esegua una simulazione cosmologica a N-body implementando il metodo del particle mesh.
//  Si risolva l'equazione di Poisson per la gravita' in un sistema N-body
//  utilizzando una griglia fissa. Grazie alla griglia si abbatte il costo computazionale:
//  si passa da un N^2 a NlogN.
//
//  Created by Aurora Valentini on 24/11/25.
//



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "physic.h"

int main() {
    Parametri param;

    //--------------------------- Condizioni iniziali -----------------------------

    // 1. Leggo i dati  dal file parameters.dat
    leggi_param("parameters.dat", &param);

    // 2. Calcolo le unita' di misura interne da utilizzare nei calcoli (G e unit_time)
    //    e anche la geometria della griglia
    calcola_param(&param);

    // 3. Alloco e genero le IC
    Particle *part = (Particle *)malloc(param.n_points * sizeof(Particle));
    sinusoidal_ic(part, &param); 

    // 4. Faccio l'istogramma per fare un check
    histogram(part, &param, param.n_grid);

    // Uso gnuplot per vedere il grafico
    // plot "istogramma.txt" using 1:2 with lines

    conversion("init_cond.bin", "init_cond.txt");

    printf("\n--- Checkpoint 1: IC generate ---\n");

    //---------------------- Assegnazione della massa ---------------------------------

    // 5. Inizializzo a zero la griglia della densita'
    double *rho = (double *)calloc(param.n_grid, sizeof(double));

    // 6. Chiamo la funzione di assegnazione (NGP, CIC o TSC a seconda del Make)
    assign_density(part, rho, &param);

    printf("\n--- Checkpoint 2: Densita' assegnata  ---\n");

    //---------------------- FFT e calcolo del potenziale ---------------------------

    // Alloco l'array per il potenziale (stessa dimensione della griglia)
    double *pot = (double *)malloc(param.n_grid * sizeof(double));
    
    // Chiamo la funzione FFT che abbiamo scritto prima
    compute_potential(rho, pot, &param);

    // Salvo il potenziale direttamente in txt plottare il confronto 
    // tra risultato del potenziale numerico (con fft) e analitico (calcolato a mano).

    // comparison(pot, &param, "comparison_test.txt");
    // plot "istogramma.txt" using 1:2 with lines

    printf("\n--- Checkpoint 3: Potenziale calcolato via FFT ---\n");
    
    //---------------------- Calcolo delle accelerazioni  ---------------------------

    // Alloco l'array per le accelerazioni (stessa dimensione della griglia)
    double *acc_grid = (double *)malloc(param.n_grid * sizeof(double));

    // Calcolo l'accelerazione sulla griglia
    compute_acc(pot, acc_grid, &param);

    // Assegno le accelerazioni alle particelle
    assign_acc(part, acc_grid, &param);

    //---------------------- Integrazione secondo Leapfrog  ---------------------------

    char filename_rho[64];
    char filename_phase[64];

    printf("\n--- Inizio evoluzione (Timestep adattivo: %s) ---\n", param.dt_flag ? "ON" : "OFF");


    double t = 0.0;
    double dt_attuale = param.dt_fisso; // Valore iniziale
    int step = 0;
    int n_skip_attuale;

    if (param.dt_flag == 0) {
        n_skip_attuale = param.n_skip_fix;
    } else {
        n_skip_attuale = param.n_skip_adapt;
    }

    while (t < param.t_final) {

        // --- CALCOLO TIMESTEP ---
        // Se param.dt_flag == 1 usa la funzione adattiva, altrimenti resta dt_fisso
        dt_attuale = calcola_dt_adapt(part, &param);

        // --- LEAPFROG ---
        // Primo kick (mezzo passo)
        kick(part, &param, dt_attuale); 

        // Drift (passo intero)
        drift(part, &param, dt_attuale); 

        // Aggiornamento Forze
        for (int i = 0; i < param.n_grid; i++) rho[i] = 0.0;
        assign_density(part, rho, &param);
        compute_potential(rho, pot, &param);
        compute_acc(pot, acc_grid, &param);
        assign_acc(part, acc_grid, &param);

        // Secondo kick (mezzo passo)
        kick(part, &param, dt_attuale * 0.5);

        // Aggiorno il tempo
        t += dt_attuale;
        step++;

        // --- SALVATAGGIO ---
        // Salvo ogni n_skip per non creare troppi file.
        // Nota: ho scelto n_skip diversi per caso adattivo e fisso al fine di 
        // visualizzare meglio la fase di shell crossing. 
        if (step % n_skip_attuale == 0) {
            sprintf(filename_phase, "phase_step_%04d.txt", step);
            sprintf(filename_rho, "rho_step_%04d.txt", step);
            save_phase_space(part, &param, filename_phase);
            save_density(rho, &param, filename_rho);
            printf("t = %.3f | Step %d | dt = %.5f\n", t, step, dt_attuale);
        }

        /*
        // DEBUG: per fare un check sull'andamento dell'evoluzione
        if (step < 400) {
            printf("DEBUG: step %d, dt_usato = %.10f, dt_param = %.10f\n", 
            step, dt_attuale, param.dt_fisso);
        }
        */
    }

    printf("\n--- Evoluzione completata. Tempo finale raggiunto: %.3f ---\n", t);

    //---------------------- Pulizia finale parametri allocati --------------------
    free(part);
    free(rho);
    free(pot);
    free(acc_grid);

    return 0;
}