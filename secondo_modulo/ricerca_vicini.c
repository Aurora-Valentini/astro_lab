#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "physic.h"

// In questo modulo scrivo le funzioni che mi permettono di identificare i 
// vicini. Utilizzero' due metodi: Quicksort e Cell-Linked List.

// ----------------- METODO 1: QUICKSORT ---------------------
// Introduco una funzione di supporto per confrontare la posizione delle particelle nella struct
int compare_part(const void *a, const void *b) {
    Particle *p1 = (Particle *)a;
    Particle *p2 = (Particle *)b;

    if (p1->pos < p2->pos) return -1;
    if (p1->pos > p2->pos) return 1;
    return 0;
}

// Con questa funzione calcolo h_i=smoothed lenght
void update_h(Particle *part, Parameter *param) {
    int k = 32; // N_ngb/2 = 64/2 = 32 vicini a destra e 32 a sinistra
    double global_h_max = 0.0;

    // Prima di calcolare h ricorda di ordinare tutte le particelle.

    for (int i = 0; i < param->n_totale; i++) {
        double dist_left = 0;
        double dist_right = 0;

        // Vicini a sinistra 
        if (i >= k) {
            dist_left = fabs(part[i].pos - part[i-k].pos);
        } else {
            // Se non ho 32 vicini a sinistra, prendo la distanza dalla prima 
            dist_left = fabs(part[i].pos - part[0].pos);
        }

        // Vicini a destra 
        if (i + k < param->n_totale) {
            dist_right = fabs(part[i].pos - part[i+k].pos) ;
        } else {
            // Se non ho 32 vicini a destra, prendo la distanza dall'ultimo
            dist_right = fabs(part[i].pos - part[param->n_totale-1].pos);  
        }

        // Scelgo il massimo tra le due distanze (ho meno oscillazioni spurie)
        double dist = (dist_left > dist_right) ? dist_left : dist_right;
        part[i].h = dist / 2.0;

        // Se scelgo la media aritmetica 
        //part[i].h = 0.5 * (dist_left + dist_right);
        
        // Check: h non deve mai essere zero
        if (part[i].h < 1e-6) part[i].h = 1e-6;

        // Trovo il massimo globale tra tutte le particelle. Mi serve per metodo linked list
        if (part[i].h > global_h_max) {
            global_h_max = part[i].h;
        }
    }

    // Salvo i parametri che mi serviranno per costruire la griglia
    param->h_max = global_h_max;
    param->d_cell = 2.0 * param->h_max;
    param->n_cell = (int)ceil((param->L / param->d_cell));

}


// ----------------- METODO 2: CELL-LINKED LIST ---------------------

void linked_list(Particle *part, Parameter *param) {

    // Svuoto le teste imponendo NULL
    for (int m = 0; m < param->n_cell; m++){
        param->head[m] = NULL;
    }

    // Collego tutte le particelle 
    for (int i = 0; i < param->n_totale; i++) {
        // Hashing spaziale: traduco una posizione continua in un indice intero
        // Dato che gli array non accettano indici negativi traslo tutto da -L/2->L/2 a 0->L 
        int k_cell = (int)((part[i].pos + param->L/2.0) / param->d_cell);
        
        // Protezione ai bordi 
        // Dato che le ghost sono fuori dal tubo, l'hashing spaziale fallirebbe. 
        // Quindi forzo queste ghost a stare nella prima e nell'ultima cella.
        // Questo rende il calcolo un po' meno efficiente infatti il codice dovrà scorrere 
        // delle liste molto lunghe per la prima e l'ultima cella.  
        if (k_cell < 0) k_cell = 0;
        if (k_cell >= param->n_cell) k_cell = param->n_cell - 1;

        // La particella i-esima guarda chi è attualmente il primo della lista nella sua cella
        // e memorizza l'indirizzo nel puntatore next
        part[i].next = param->head[k_cell];
        // La cella aggiorna la sua testa e cosi' via fino a creare la lista
        param->head[k_cell] = &part[i];
    }
}



