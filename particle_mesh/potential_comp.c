#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "physic.h"
#include <fftw3.h>

// In questo file scrivo le funzioni che servono per calcolare il potenziale.
// Passo nello spazio di Fourier dove sfrutto il fatto che la derivata seconda
// diventa semplicemente una moltiplicazione per (-k^2). Quindi devo usare FFT. 
// Per tornare allo spazio reale infine faccio l'antitrasformata. 

void compute_potential(double *rho, double *pot, Parametri *p) {
    int N = p->n_grid;
    
    // 1. Allocazione del buffer FFTW 
    fftw_complex *kDensity = (fftw_complex*) fftw_alloc_complex(N/2 + 1);
    fftw_complex *kPot     = (fftw_complex*) fftw_alloc_complex(N/2 + 1);

    // 2. Creazione dei piani 
    fftw_plan fft_fwd = fftw_plan_dft_r2c_1d(N, rho, kDensity, FFTW_ESTIMATE); // fft forward = trasformata diretta
    fftw_plan fft_bck = fftw_plan_dft_c2r_1d(N, kPot, pot, FFTW_ESTIMATE);     // fft back = trasformata inversa = antitrasformata

    // 3. Eseguo la trasformata di Fourier: Rho(x) -> kDensity(k)
    fftw_execute(fft_fwd);
    
    // Il modo k=0 va gestito a parte per evitare la divisione per zero
    kPot[0][0] = 0.0; 
    kPot[0][1] = 0.0;

    // Ci sarebbe anche un prefattore moltiplicativo (normalizzazione). 
    // Per non portarmi dietro errori di arrotondamento lo trascuro
    // double factor = 4.0 * PI * p->G_norm;

    for (int i = 1; i < N/2 + 1; i++) {
        double k = ( i * 2.0 * PI )/ p->box_length;;
        double k2 = k * k;

        // Formula: Phi(k) = - Rho(k) / k^2
        // [0] è la parte reale, [1] è la parte immaginaria
        kPot[i][0] = -( kDensity[i][0]) / k2;
        kPot[i][1] = -( kDensity[i][1]) / k2;
    }

    // 5. Antitrasformata di Fourier: kPot(k) -> Pot(x)
    fftw_execute(fft_bck);

    // 6. Normalizzazione (FFTW non normalizza automaticamente), in 1D devo dividere per N
    double final_norm = 1.0 / (double)N; 
    for (int i = 0; i < N; i++) {
        pot[i] *= final_norm;
    }

    // 7. Pulizia memoria 
    fftw_destroy_plan(fft_fwd);
    fftw_destroy_plan(fft_bck);
    fftw_free(kDensity);
    fftw_free(kPot);
}

// Funzione che serve per fare un check e confrontare il potenziale calcolato con fft e quello analitico. 
void comparison(double *pot_fft, Parametri *p, const char *filename) {
    FILE *f = fopen(filename, "w");
    double A = p->a_delta; // L'ampiezza della sinusoide iniziale 
    double k = 2.0 * PI / p->box_length; // Assumo n=1 

    for (int j = 0; j < p->n_grid; j++) {
        double x = (j + 0.5) * p->dx;
        double val_pot_fft = pot_fft[j];
        
        // Data delta = -A * cos(k*x), integro due volte (a mano) e trovo:
        double pot_analitic = (A / (k * k)) * cos(k * x);


        fprintf(f, "%f\t%f\t%f\n", x, val_pot_fft, pot_analitic);
    }
    fclose(f);
    printf("Confronto salvato in %s\n", filename);
}

// Per ottenere la forza e di conseguenza l'accelerazione devo differenziare il potenziale.
// a=F/m ma lavorando in unita' interne m=1

void compute_acc(double *pot, double *acc, Parametri *p){

    int N = p->n_grid;

    for (int j = 0; j < N; j++) {
        // Gestione della Periodicità 
        // jp: indice successivo (j+1), se j è l'ultimo torna a 0
        // jm: indice precedente (j-1), se j è 0 va all'ultimo
        int jp = (j + 1) % N;
        int jm = (j - 1 + N) % N;

        // Uso le differenze finite per esprimere la derivata del potenziale
        acc[j] = -(pot[jp] - pot[jm]) / (2.0 * p->dx);
    }

}

// A partire dall'accelerazione calcolata sulla griglia, questa funzione la assegna ad
// una determinata particella in base alla sua posizione secondo i 3 metodi.
void assign_acc(Particle *parts, double *grid_acc, Parametri *p) {
    int N = p->n_grid;
    double dx = p->dx;

    for (int i = 0; i < p->n_points; i++) {
        // Trovo la posizione della particella in unità di cella
        double x_cell = parts[i].x / dx;

#ifdef NGP
        // NEAREST GRID POINT: Prendo il nodo più vicino
        int j_nearest = (int)round(x_cell);
        j_nearest = (j_nearest + N) % N; // Periodicità
        parts[i].ax = grid_acc[j_nearest];

#elif defined(CIC)
        int j = (int)floor(x_cell);
        
        // CLOUD IN CELL: Interpolazione lineare tra i due nodi adiacenti
        double weight_right = x_cell - (double)j;
        double weight_left  = 1.0 - weight_right;

        int j_left  = (j + N) % N;
        int j_right = (j + 1 + N) % N;

        // L'accelerazione della particella è la media pesata dei due nodi
        parts[i].ax = weight_left * grid_acc[j_left] +  weight_right * grid_acc[j_right];

#elif defined(TSC)
        // La particella influenza 3 nodi: j-1, j, j+1
        double d = x_cell - (double)j; // Distanza dal nodo centrale (-0.5 < d < 0.5)
        
        // Pesi TSC (Funzioni quadratiche)
        double w_central = 0.75 - d * d;
        double w_left    = 0.5 * (0.5 - d) * (0.5 - d);
        double w_right   = 0.5 * (0.5 + d) * (0.5 + d);

        int jl = (j - 1 + N) % N; // left
        int jc = (j + N) % N;     // center
        int jr = (j + 1 + N) % N; // right

        parts[i].ax = w_left * grid_acc[jl] + 
                      w_central * grid_acc[jc] + 
                      w_right * grid_acc[jr];

#endif
    }
}