#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include "physic.h"

// Calcolo il minimo step temporale per l'evoluzione secondo il criterio di Courant
double timestep_comp(Particle *part, Parameter *param) {
    double h_min = 1e10;
    double cs_max = 0.0;
    double k_courant = 0.15;

    for (int i = 0; i < param->n_totale; i++) {
        if (part[i].ghost == 1) continue;
        // Trovo l'h più piccolo tra quelli calcolati da update_h
        if (part[i].h < h_min) h_min = part[i].h;
                
        // Calcolo velocità del suono locale
        double cs = sqrt(param->gamma * part[i].p / part[i].rho);
        if (cs > cs_max) cs_max = cs;        
    }

    // Evito divisioni per zero se tutto è fermo all'inizio
    if ((cs_max) < 1e-12) return 0.0001; 
    return (k_courant * h_min) / (cs_max);
}


// Passo 1: KICK 
// Aggiorno le velocità e le energie interne di mezzo passo
void kick(Particle *part, Parameter *param, double dt_step) {

    for (int i = 0; i < param->n_totale; i++) {
        if (part[i].ghost == 1) {
            part[i].vel = 0.0; 
            part[i].acc = 0.0;
            continue; 
        } 
        // Aggiorno la velocita': v=v+a*dt/2
        part[i].vel += part[i].acc * (dt_step * 0.5);

        // Aggiorno l'energia interna: u= u+ du*dt/2
        part[i].u += part[i].du * (dt_step * 0.5);
        if (part[i].u < 0) part[i].u = 1e-12;
    }
}

// Passo 2: DRIFT
// Aggiorno le posizioni di un passo intero
void drift(Particle *part, Parameter *param, double dt_step) {

    for (int i = 0; i < param->n_totale; i++) {
        
        if (part[i].ghost == 1) {
            part[i].vel = 0.0; 
            part[i].acc = 0.0;
            continue; 
        } 
        part[i].pos += part[i].vel * dt_step;
    }
}


void leapfrog(Particle *part, Parameter *param, double dt, int metodo) {
    // --- STEP 1: KICK & DRIFT  ---
    kick(part, param, dt);
    drift(part, param, dt);

    // --- STEP 2: RICERCA VICINI E CALCOLI ---
    if (metodo == 0) { 
        // Uso QUICKSORT
        qsort(part, param->n_totale, sizeof(Particle), compare_part);
        update_h(part, param);
        density_press_qs(part, param);
        accel_qs(part, param);
    } 
    else { 
        // Uso LINKED LIST
        update_h(part, param); // Aggiorna h e h_max adattivo
        
        // Ricalcolo parametri griglia
        param->d_cell = 2.0 * param->h_max;
        param->n_cell = (int)ceil(param->L / param->d_cell);
        
        linked_list(part, param); 
        density_press_ll(part, param);
        accel_ll(part, param);
    }

    // --- STEP 3: SECONDO KICK ---
    kick(part, param, dt);
}