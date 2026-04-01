//
//  ESERCIZIO PARTICLE MESH
//
//  Si esegua una simulazione cosmologica a N-body implementando il metodo del particle mesh.
//  Si risolva l'equazione di Poisson per la gravita' in un sistema N-body
//  utilizzando una griglia fissa. Grazie ala griglia si abbatte il costo computazionale
//  passando da un N^2 a NlogN poiche' non calcolo la forza  tra ogni particella ma uso la griglia
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


    // 3. Alloca e genera le IC
    Particle *part = (Particle *)malloc(param.n_points * sizeof(Particle));
    sinusoidal_ic(part, &param); 

    // 4. faccio l'istogramma per fare un check
    histogram(part, &param, param.n_grid);
    // Usa gnuplot per vedere il grafico
    // plot "istogramma.txt" using 1:2 with lines

    conversion("init_cond.bin", "init_cond.txt");

    printf("\n--- Checkpoint 1: IC generate ---\n");

    //---------------------- Assegnazione della massa ---------------------------------

    // 5. Inizializzo a zero la griglia della densita'
    double *rho = (double *)calloc(param.n_grid, sizeof(double));

    // 6. Chiamo la funzione di assegnazione (NGP, CIC o TSC a seconda del Makefile)
    assign_density(part, rho, &param);

    save_density(rho, &param, "density_plot.txt");

    // Meglio e faccio direttamente un file txt perche' dopo mi serve per il loop del leapfrog 
    //conversion("density_plot.bin", "density_plot.txt");

    printf("\n--- Checkpoint 2: Densita' assegnata  ---\n");

    //---------------------- FFT e calcolo del potenziale ---------------------------

    // Alloco l'array per il potenziale (stessa dimensione della griglia)
    double *pot = (double *)malloc(param.n_grid * sizeof(double));
    
    // Chiamo la funzione FFT che abbiamo scritto prima
    compute_potential(rho, pot, &param);

    // Salvo il potenziale direttamente in txt plottare il confronto 
    // tra risultato del potenziale numerico (con fft) e analitico (calcolato a mano).

    comparison(pot, &param, "comparison_test.txt");

    printf("\n--- Checkpoint: Potenziale calcolato via FFT ---\n");
    
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

    for (int step = 0; step < param.n_step; step++) {

        // Primo kick di mezzo passo per aggiornare le velocita' di dt/2
        kick(part, &param); 

        // Drift di un passo intero per aggiornare le posizioni
        drift(part, &param); 

        // Ora ricalcolo la densità -> il potenziale -> la forza (accelerazione)
        // Inizializzo di nuovo a zero la griglia della densita' (senza riallocare la memoria)
        for (int i = 0; i < param.n_grid; i++) {
            rho[i] = 0.0;
        }

        assign_density(part, rho, &param);
        compute_potential(rho, pot, &param);
        compute_acc(pot, acc_grid, &param);
        assign_acc(part, acc_grid, &param);

        // Secondo kick di mezzo passo per aggiornare le velocita' dell'altro dt/2
        kick(part, &param);

        // Salvo su file i risultati per il plotting ogni n_step
        if (step % param.n_skip == 0) {
            sprintf(filename_phase, "phase_step_%04d.txt", step);
            sprintf(filename_rho, "rho_step_%04d.txt", step);
            save_phase_space(part, &param, filename_phase);
            save_density(rho, &param, filename_rho);
            printf("Salvataggio step %d completato.\n", step);
        }
        
    }

    



    //---------------------- Pulizia finale parametri allocati --------------------
    free(part);
    free(rho);
    free(pot);
    free(acc_grid);

    return 0;
}