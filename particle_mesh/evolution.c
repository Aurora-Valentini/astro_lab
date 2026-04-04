#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "physic.h"

// In questo file scrivo le funzioni che mi servono per fare l'integrazione
// vera e propria secondo il metodo del Leapfrog.

// Passo 1: KICK 
// Aggiorno le velocità di dt/2 = dt * 0.5
void kick(Particle *part, Parametri *param, double dt_step) {
    for (int i = 0; i < param->n_points; i++) {
        part[i].v += part[i].ax * (dt_step * 0.5);
    }
}

// Passo 2: DRIFT
// Aggiorno le posizioni di un passo intero
void drift(Particle *part, Parametri *param, double dt_step) {
    for (int i = 0; i < param->n_points; i++) {
        part[i].x += part[i].v * dt_step;

        // Boundary conditions periodiche
        if (part[i].x < 0) part[i].x += param->box_length;
        if (part[i].x >= param->box_length) part[i].x -= param->box_length;
    }
}

// Funzione che scrive su file le posizioni e le velocita' per poterle plottare
void save_phase_space(Particle *part, Parametri *param, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("Errore: impossibile creare il file %s\n", filename);
        return;
    }

    for (int i = 0; i < param->n_points; i++) {
        // Stampo posizione e velocita'
        fprintf(f, "%e \t %e\n", part[i].x, part[i].v);
    }

    fclose(f);
    printf("Dati salvati in: %s\n", filename);
}

double calcola_dt_adapt(Particle *part, Parametri *p) {
    // Se dt_flag=0 allora scelgo il timestep fisso nel file parameters.dat
    if (p->dt_flag == 0) {
        return p->dt_fisso; 
    }

    // Inizializzo v_max
    double v_max = 0.0;

    // Trovo la velocita' massima tra tutte le particelle
    for (int i = 0; i < p->n_points; i++) {
        double v_abs = fabs(part[i].v);
        if (v_abs > v_max) {
            v_max = v_abs;
        }
    }

    // All'inizio della simulazione le particelle sono quasi ferme,
    // per evitare la divisione per zero uso un valore minimo molto piccolo.
    if (v_max < 1e-12) {
        return p->dt_fisso; 
    }
    // Calcolo dt basato sulla frazione 'w' della cella 'dx'
    double dt_adapt = p->w_param * (p->dx / v_max);

    // DEBUG: stampo cosa calcola prima di imporre il limite
    // printf("v_max: %f | dt_adapt: %f\n", v_max, dt_adapt);

    // Se il dt calcolato è più grande di quello fisso, uso quello fisso.
    if (dt_adapt > p->dt_fisso) {
        return p->dt_fisso;
    } 

    return dt_adapt;
}
