#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "physic.h"

// In questo file scrivo le funzioni che servono per assegnare la massa
// ad ogni particella al fine di interpolare il campo di densita' e sarvare i risultati.

void assign_density(Particle *part, double *rho, Parametri *p) {
    // Inizializzo a zero l'array delle densita'
    for (int j = 0; j < p->n_grid; j++) {
        rho[j] = 0.0;
    }

    double inv_dx = p->inv_dx; // già salvato nella struct contenuta in physic.h

    // Faccio un ciclo su tutte le particelle
    for (int i = 0; i < p->n_points; i++) {
        // Calcolo la posizione della particella in unità di cella
        double x_grid = part[i].x * inv_dx;

        // ------------------ 3 METODI -----------------
        // Definisco le varie funzioni per calcolare la densita'.

        // NGP= Nearest Grid Point
        // In questo metodo cerco la posizione della particella sulla griglia e la approssimo
        // sempre per difetto con floor(). Il problema è la discontinuita'.
        #ifdef NGP
            int idx = (int)floor(x_grid);
            int idx_corr = (idx + p->n_grid) % p->n_grid;  // Boundary conditions periodiche
            rho[idx_corr] += 1.0;                          // Qui assegno la massa, la particella cade in un'unica cella 
        #endif

        // CIC= Cloud In Cell
        // Qui la particella non cade più in una sola cella ma viene 'spalmata' anche alle due
        // celle piu' vicine, a destra e a sinistra. Qui la densità varia in modo continuo.
        #ifdef CIC
            int i_left = (int)floor(x_grid);
            double w_right = x_grid - (double)i_left;  // Peso da dare alla cella destra (piu' la particella si sposta a dx più peso avrà la cella di dx e viceversa)
            double w_left = 1.0 - w_right;              // Peso da dare alla cella sinistra

            // Assegno la massa usando l'operatore modulo (%). In questo modo se ad es. la particella è vicina al bordo destro
            // e l'indice esce dalla scatola, allora l'indice torna a zero ed e' come se rientrasse dall'altra parte.
            rho[i_left % p->n_grid] += w_left;
            rho[(i_left + 1) % p->n_grid] += w_right; 
        #endif

        // TSC= Triangular Shaped Cloud
        // A differenza del del CIC spalma la distribuzione di massa non in 2 celle bensi' in 3 celle
        #ifdef TSC
            // Trovo il nodo della griglia che è piu' vicino
            int i_nearest = (int)floor(x_grid + 0.5);
            // Calcolo la distanza (d) tra la particella e il centro di quel nodo
            double d = x_grid - (double)i_nearest;

            //Calcolo i pesi per le 3 celle coinvolte (sx, 0, dx)
            double w_0 = 0.75 - d * d;
            double w_sx = 0.5 * (0.5 - d)*(0.5 - d);
            double w_dx = 0.5 * (0.5 + d)*(0.5 + d);

            // Assegno la massa con boundary conditions periodiche 
            rho[(i_nearest - 1 + p->n_grid) % p->n_grid] += w_sx;
            rho[(i_nearest + p->n_grid) % p->n_grid]     += w_0;
            rho[(i_nearest + 1 + p->n_grid) % p->n_grid] += w_dx;
        #endif
    }

    // Normalizzazione: trasformo i conteggi in contrasto di densità delta
    double n_medio = (double)p->n_points / p->n_grid;
    for (int j = 0; j < p->n_grid; j++) {
        rho[j] = (rho[j] / n_medio) - 1.0;
    }
}

void save_density(double *rho, Parametri *p, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("Errore: Impossibile creare il file %s\n", filename);
        return;
    }

    for (int j = 0; j < p->n_grid; j++) {
        double x_coord = (j + 0.5) * p->dx;
        // Stampo la coordinata x e il valore del contrasto di densita' delta
        fprintf(f, "%e\t%e\n", x_coord, rho[j]);
    }
    
    printf("Dati salvati in: %s\n", filename);
    fclose(f);
}






