// In questo file scrivo le funzioni che mi servono per fare l'integrazione
// vera e propria secondo il metodo del Leapfrog
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "physic.h"

// Passo 1: KICK 
// Aggiorno le velocità di dt/2 = dt * 0.5
void kick(Particle *part, Parametri *param) {
    for (int i = 0; i < param->n_points; i++) {
        part[i].v += part[i].ax * (param->dt * 0.5);
    }
}

// Passo 2: DRIFT
// Aggiorno le posizioni 
void drift(Particle *part, Parametri *param) {
    for (int i = 0; i < param->n_points; i++) {
        part[i].x += part[i].v * param->dt;

        // Boundary conditions periodiche
        if (part[i].x < 0) part[i].x += param->box_length;
        if (part[i].x >= param->box_length) part[i].x -= param->box_length;
    }
}

// Funzione che scrive su file le posizione e le velocita' per poterle plottare
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