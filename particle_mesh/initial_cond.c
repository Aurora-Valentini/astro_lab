#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "physic.h"


// In questo file sono raccolte tute le funzioni che servono per fissare le
// condizioni iniziali per la simulazione di particle mesh. 

// Funzione che legge i parametri dal file parameters.dat
void leggi_param(const char *filename, Parametri *param){
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
            if (strcmp(key, "unit_v") == 0) param->unit_v = val;
            else if (strcmp(key, "unit_m") == 0) param->unit_m = val;
            else if (strcmp(key, "unit_l") == 0) param->unit_l = val;
            else if (strcmp(key, "G_cgs") == 0) param->G_cgs = val;

            else if(strcmp(key, "n_points") == 0) param->n_points = (int)val;
            else if(strcmp(key, "n_grid") == 0) param->n_grid = (int)val;
            else if(strcmp(key, "box_length") == 0) param->box_length = val;
            else if(strcmp(key, "a_delta") == 0) param->a_delta = val;

            else if(strcmp(key, "dt_fisso") == 0) param->dt_fisso = val;
            else if(strcmp(key, "n_step") == 0) param->n_step = val;
            else if(strcmp(key, "n_skip_adapt") == 0) param->n_skip_adapt = val;
            else if(strcmp(key, "n_skip_fix") == 0) param->n_skip_fix = val;

            else if(strcmp(key, "dt_flag") == 0) param->dt_flag = val;
            else if(strcmp(key, "t_final") == 0) param->t_final = val;
            else if(strcmp(key, "w_param") == 0) param->w_param = val;

        }
    }
    fclose(fp);
}

// Funzione che calcola i parametri aggiuntivi che mi servono sia per definire l'unita' interna del 
// tempo e G_norm, sia per definire la geometria della griglia (celletta dx).
void calcola_param(Parametri *param){
    param->unit_time = param->unit_l / param->unit_v; 
    param->G_norm = param->G_cgs * param->unit_m * (param->unit_time * param->unit_time) / (param->unit_l * param->unit_l * param->unit_l);
    param->dx = param->box_length / (double)param->n_grid;
    param->inv_dx = 1.0 / param->dx;
}

// Funzione che scrive le condizioni iniziali su file per il plot
void scrivi_ic(Particle *part, int n_points, char *filename){
    FILE *fp = fopen(filename, "wb");
    if (!fp) { perror("Errore apertura file binario"); 
        return; 
    }

    fwrite(part, sizeof(Particle), n_points, fp);
    fclose(fp);
    printf("Dati salvati in %s (formato binario)\n", filename);
}

void conversion(const char *file_bin, const char *file_txt) {

    // Apro il file binario e lo leggo
    FILE *bin = fopen(file_bin, "rb");
    if (bin == NULL) {
        perror("Errore apertura file binario");
        exit(1);
    }

    // Apro il file .txt e ci scrivo
    FILE *txt = fopen(file_txt, "w");
    if (txt == NULL) {
        perror("Errore apertura file .txt");
        fclose(bin);
        exit(1);
    }

    Particle p;
    fprintf(txt, "# x \t\t v\n");
    
    // Legge una particella alla volta finché il file non finisce
    while (fread(&p, sizeof(Particle), 1, bin) == 1) {
        fprintf(txt, "%15.8e %15.8e\n", p.x, p.v);
    }

    fclose(bin);
    fclose(txt);
    printf("Conversione bin to ASCII completata con successo.\n");
}

// Funzione che genera un contrasto di densità sinusoidale come condizione iniziale
void sinusoidal_ic(Particle *part, Parametri *param) {
    // Calcolo il numero d'onda k a partire dalla lunghezza della box 
    double k = 2.0 * PI / param->box_length;
    double dx_unif = param->box_length / (double)param->n_points;

    printf("Generazione IC: %d particelle, A = %.3f\n", param->n_points, param->a_delta);

    for (int i = 0; i < param->n_points; i++) {
        // 1. Posizione imperturbata, uso la staggered grid
        double x_unif = (i + 0.5) * dx_unif;

        // 2. Spostamento calcolato a partire da contrasto di densita' delta con
        //    distribuzione sinusoidale: 
        part[i].x = x_unif + (param->a_delta/k) * sin(k * x_unif);;
        
        // 3. Velocità iniziale nulla
        part[i].v = 0.0;

        // 4. Boundary conditions periodiche 
        if (part[i].x < 0) part[i].x += param->box_length;
        if (part[i].x >= param->box_length) part[i].x -= param->box_length;
    }

    // 5. Scrivo i dati sul file di output binario
    scrivi_ic(part, param->n_points, "init_cond.bin"); 
}

void histogram(Particle *part, Parametri *p, int n_bins) {
    // Alloco i bin per il conteggio
    double *counts = (double *)calloc(n_bins, sizeof(double));
    double dx_bin = (double)p->box_length / n_bins;

    // Riempio i bin (Metodo Nearest Grid Point - NGP)
    for (int i = 0; i < p->n_points; i++) {
        // cosi capisco in quale celletta cade la particella
        int idx = (int)(part[i].x / dx_bin);   
        if (idx >= 0 && idx < n_bins) {
            counts[idx] += 1.0;
        }
    }

    
    // Calcolo il contrasto di densità delta = (n / n_medio) - 1
    // Assumo masse delle particelle tutte uguali.
    double n_medio = (double)p->n_points / n_bins;
    
    FILE *f_hist = fopen("istogramma.txt", "w");
    fprintf(f_hist, "# x_centro \t delta\n");

    for (int j = 0; j < n_bins; j++) {
        double x_centro = (j + 0.5) * dx_bin;
        double delta = (counts[j] / n_medio) - 1.0;
        fprintf(f_hist, "%f \t %f\n", x_centro, delta);
    }

    fclose(f_hist);
    free(counts);
    printf("Verifica completata: controlla 'istogramma.txt'\n");
}



