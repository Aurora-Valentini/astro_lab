//
//  ESERCIZIO EQUAZIONE DELLA DIFFUSIONE
//  Risolvere la seguente funzione du(t,x)/dt= K d^2u/dx^2
//  K= coefficiente della diffusione
//
//  Metodo per risolvere:
//  1. discretizzare la parte spaziale. Discretizzando la derivata seconda
//     d^2u/dx^2 con differenze finite ottengo un sistema di N ODE. 
//  2. Uso Runge-Kutta del 4° ordine per integrare la parte temporale.
//
//
//  Created by Aurora Valentini on 18/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nrutil.h"


// Voglio costruire una griglia di N=100 punti equispaziati tra [0,1] 

float K;
float dx;
int N;

void derivs(float t, float u[], float dudt[]){
    // Dato che uso il Numerical Recipe ricordo che gli indici partono da 1
    // Per il ciclo scelgo di partire da i=2 perche' altrimenti con i=1 --> u[i-1] non esiste
    // Idem per N, devo finire con i=N-1 altrimenti N[i+1] non esiste 
    for (int i = 2; i < N - 1; i++) {  
        dudt[i] = K * (u[i+1] - 2*u[i] + u[i-1]) / (dx * dx);
    }

    // Condizioni al contorno
    dudt[1] = 0.0;
    dudt[N-1] = 0.0;
}

// Definisco la funzione RK4 per l'integrazione temporale
void rk4( float y[], float dydx[], int n, float x, float h, float yout[],
          void (*derivs)(float, float [], float []) );


int main() {
    N = 100;
    // K da definire ogni volta per vedere come varia il risultato
    //K = 0.03; 
    //K= 0.1;  
    K=0.3;
    dx = 1.0 / (N-1);

    float *x = vector(1, N);
    float *u = vector(1, N);
    float *dudt = vector(1, N);
    float *u_out = vector(1, N);

    // Costruisco la griglia
    for (int i = 1; i < N; i++) {
        x[i] = i * dx;
    }

    // Inizializzo tutti a zero
    for (int i = 1; i < N; i++) {
        u[i] = 0.0;
    }

    // Imposto 1 nei 10 punti centrali
    int start = N/2 - 5; // indice del primo punto centrale
    int end   = N/2 + 5; // indice dell'ultimo punto centrale

    for (int i = start; i < end; i++) {
        u[i] = 1.0;
    }
    
    // Condizioni per integrare la parte temporale
    float t = 0.0;
    float dt = 0.0001;
    int NSTEP = 10000;
    
    // Apro il file su cui scrivero' gli output
    char filename[50];
    sprintf(filename, "diffusion_K=%.2f.txt", K);
    FILE *fp = fopen(filename, "w");

    // Ciclo temporale
    for (int i=0; i<=NSTEP; i++) {
        derivs(t, u, dudt);
        rk4(u, dudt, N, t, dt, u_out, derivs);

        for (int j=1; j<N; j++ ){
            u[j] = u_out[j];
        }
        t = t + dt;

        // Voglio salvare la soluzione in vari snapshot temporali (t= i*dt)
        if ( i == 50 || i == 200 || i == 600 || i == 1000 || i == 5000 || i == 9000 ){
            for (int j=1; j<=N; j++ ){
                fprintf(fp, "%f\t%f\n", x[j], u[j]);
            }
            fprintf(fp, "\n");  
        }

    }
    fclose(fp);
    printf("File generato: %s\n", filename);

    // Libero la memoria
    free_vector(x,1,N);
    free_vector(u,1,N);
    free_vector(dudt,1,N);
    free_vector(u_out,1,N);

    return 0;
}

//------------- COMPILAZIONE --------------
// gcc diffusion.c rk4.c nrutil.c -lm -o diffusion.x
// ./diffusion.x  


//------------- COMANDI GNUPLOT --------------
// L'obiettivo è vedere come cambia 
// plot "diffusion_K=0.03.txt"  using 1:2 with lines title "K=0.03"
// plot "diffusion_K=0.10.txt"  using 1:2 with lines title "K=0.1"
// plot "diffusion_K=0.03.txt"  using 1:2 with lines title "K=0.03"

//------------- RISULTATI ATTESI --------------
// Sotto K=0.03 si perde stabilità numerica e sopra K=0.3 gli ultimi
// snapshot erano troppo 'piatti' per esser visti.

// Le curve che si osservano dai grafici sono le soluzioni dell'equazione della 
// diffusione valutate a diversi snapshot temporali. All'aumentare del tempo vediamo che
// le curve si appiattiscono ossia si allargano proprio a causa della diffusione. 

// Dal confronto con i grafici valutati a vari K si nota anche che se K è piccolo
// la diffusione è piu lenta mentre se K è grande le curve si allargano molto più rapidamente. 
