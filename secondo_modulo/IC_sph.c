#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "physic.h"

// In questo modulo scrivo le funzioni che mi permettono di calcolare
// le condizioni iniziali.

// Funzione che legge i parametri dal file parameters.dat
void leggi_param(const char *filename, Parameter *param){
    FILE *fp = fopen(filename, "r");
    if(!fp){ perror("fopen"); exit(1); }  //check
    char line[256];

    // Variabili per buffer temporanei
    char key[64], value_str[64];

    //-----------LETTURA----------

    while (fgets(line, sizeof(line), fp)) {
        // Salto righe vuote o commenti
        if (line[0] == '#' || line[0] == '\n' || isspace(line[0]))
            continue;

        // ---- PARSING DI NUMERI ----

        //  Leggo la parola chiave (fino allo spazio o all'uguale) e il valore corrispettivo
        if (sscanf(line, " %[^ =] = %s", key, value_str) == 2) {

            double val = atof(value_str);

            // Leggo le chiavi e gli associo un valore numerico
            if (strcmp(key, "L") == 0) param->L = val;
            else if (strcmp(key, "N") == 0) param->N = val;
            else if (strcmp(key, "gamma") == 0) param->gamma = val;

            else if (strcmp(key, "p_left") == 0) param->p_left = val;
            else if (strcmp(key, "rho_left") == 0) param->rho_left = val;
            else if (strcmp(key, "v_left") == 0) param->v_left = val;

            else if (strcmp(key, "p_right") == 0) param->p_right = val;
            else if (strcmp(key, "rho_right") == 0) param->rho_right = val;
            else if (strcmp(key, "v_right") == 0) param->v_right = val;

            else if (strcmp(key, "n_ghost") == 0) param->n_ghost = val;
            else if (strcmp(key, "t_max") == 0) param->t_max = val;
            
        }
    }
    fclose(fp);
}

// Funzione che calcola i parametri aggiuntivi che mi servono per calcolare le condizioni iniziali.
void calcola_param(Parameter *param){
    // La zona destra ha 1/8 delle particelle della zona sinistra.
    // Quindi divido la densità totale per 9 e so che 1 parte resta a destra e le altre 8 vanno a sinistra.
    param->N_RZ = param->N / 9; 
    param->N_LZ = param->N_RZ * 8;
    
    // Ora calcolo la massa delle particelle. Sapendo che il tubo è lungo 1. assumo che 
    // meta' (0.5) sia parte della LZ e l'altra meta' sia parte della RZ.
    param->M_LZ = param->rho_left * (param->L / 2.0);
    param->M_RZ = param->rho_right * (param->L / 2.0);

    // Qui associo alla struct Particle la massa di ogni singola particella.
    // Utilizzo unita' normalizzate in modo che ogni particella abbia la stessa massa.
    param->m_part = (param->M_LZ + param->M_RZ)/ param->N;

  
    // Qui calcolo la spaziature tra le particelle nelle due zone. 
    // Nella zona sinistra essendo piu' densa la spaziatura sara' minore.
    // Nota: metti double al denominatore altrimenti avrai errore di troncamento!
    param->dx_LZ = (param->L / 2.0) / (double)param->N_LZ;
    param->dx_RZ = (param->L / 2.0) / (double)param->N_RZ;

    param->n_totale = param->N + (param->n_ghost);

}

void init_cond(Particle *part, Parameter *param) {
    for (int i = 0; i < param->n_totale; i++) {
        // Assegno la stessa massa a tutte le particelle.
        // Le velocita' iniziali sono nulle per tutte. 
        // Accelerazione e energia interna non variano.
        part[i].mass = param->m_part; 
        part[i].acc = 0.0; 
        part[i].vel = 0.0;
        part[i].du = 0.0; 
        part[i].h = 0.0;

        // --- ZONA SINISTRA (LZ) ---
        // --- PARTICELLE GHOST DI SINISTRA ---
        if (i < param->n_ghost/2) {
            part[i].ghost = 1;
            // Definisco l'indice relativo 
            int idx_rel = i;
            // Parto da fuori e arrivo a -L/2 
            part[i].pos = -(param->L / 2.0) + (idx_rel - param->n_ghost/2 + 0.5) * param->dx_LZ;
            part[i].rho = param->rho_left;
            part[i].p = param->p_left;
        }
        // --- PARTICELLE REALI DI SINISTRA ---
        else if (i < param->n_ghost/2 + param->N_LZ) {
            part[i].ghost = 0;
            // Vado da -L/2 a 0
            // Non metto la particella esattamente sul bordo della cella piu' a sinistra bensi' al centro 
            // per questo uso (idx_rel+0.5)*dx
            int idx_rel = i - param->n_ghost/2;
            part[i].pos = -(param->L / 2.0) + (idx_rel + 0.5) * param->dx_LZ;
            part[i].rho = param->rho_left; 
            part[i].p = param->p_left; 
            part[i].vel = param->v_left;    
        }

        // --- ZONA DESTRA (RZ) ---
        // --- PARTICELLE REALI DI DESTRA ---
        else if (i < param->n_ghost/2 + param->N) {
            // Vado da 0 a +L/2
            int idx_rel = i - (param->n_ghost/2 + param->N_LZ);
            part[i].ghost = 0;
            part[i].pos = 0.0 + (idx_rel + 0.5) * param->dx_RZ;
            part[i].rho = param->rho_right; 
            part[i].p = param->p_right;
            part[i].vel = param->v_right;
        }  
        else {
            // --- PARTICELLE GHOST DI DESTRA ---
            part[i].ghost = 1;
            int idx_rel = i - (param->n_ghost/2 + param->N);
            // Parto da +L/2 e vado fuori
            part[i].pos = (param->L / 2.0) + (idx_rel + 0.5) * param->dx_RZ;
            part[i].rho = param->rho_right;
            part[i].p = param->p_right; 
        }
        
        // Calcolo energia interna u per tutte le particelle
        // u = P / ((gamma - 1) * rho) 
        part[i].u = part[i].p / ((param->gamma - 1.0) * part[i].rho);
        
    }
}

// Uso questa funzione per salvare su file i risultati 
void salva_evol(Particle *part, Parameter *param, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Errore apertura file output");
        return;
    }

    for (int i = 0; i < param->n_totale; i++) {
        if (part[i].ghost == 1) {
            continue;
        }
        fprintf(fp, "%e\t%e\t%e\t%e\t%e\t%e\n",
                part[i].pos,
                part[i].rho,
                part[i].p,
                part[i].u,
                part[i].vel,
                part[i].h);
    }
    fclose(fp);
    printf("Dati salvati in %s\n", filename);
}