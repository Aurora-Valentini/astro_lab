#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "read_func.h"
#include "integr_func.h"

// Funzione che crea un file .bin con il nome del pianeta
void apri_file(FILE **fp, int n, const char *sistema, char **nome_pianeta){
    char filename[128];
    for (int i = 0; i < n; i++) {

        if (nome_pianeta[i] == NULL) {
        fprintf(stderr, "Errore: nome pianeta non inizializzato per indice %d\n", i);
        exit(1);
        }
        
        snprintf(filename, sizeof(filename), "%s_%s.bin", sistema, nome_pianeta[i]);
        fp[i] = fopen(filename, "wb"); //wb per scrivere file binario
    }
}

// Funzione che scrive su file binario le posizioni, le velocità e le accelerazioni a diversi tempi t
void scrivi_step(FILE **fp, int n, double t, double *x, double *y, double *vx, double *vy, double *ax, double *ay, const Parametri *p) {

    //Fattori di conversione da unita interne a MKS come richiesto
    double conv_v = p->AU / p->anno_in_sec;
    double conv_a = p->AU / (p->anno_in_sec * p->anno_in_sec);

    for (int i = 0; i < n; i++) {
        Dato d;
        d.t  = t;         // Già in anni
        d.x  = x[i];      // Già in AU 
        d.y  = y[i];      // Già in AU 
        d.vx = vx[i] * conv_v; // Convertito in m/s
        d.vy = vy[i] * conv_v; // Convertito in m/s
        d.ax = ax[i] * conv_a; // Convertito in m/s^2
        d.ay = ay[i] * conv_a; // Convertito in m/s^2
        
        fwrite(&d, sizeof(Dato), 1, fp[i]);
    }
}

// Funzione che chiude il file 
void chiudi_file(FILE **fp, int n) {
    for (int i = 0; i < n; i++)
        fclose(fp[i]);
}

// G = costante di gravità universale da calcolare in unità interne in base al sistema che considero
double calcola_G_norm(double G_SI, double AU, double M_star, double anno_in_sec) {
    return G_SI * M_star * anno_in_sec * anno_in_sec / (AU * AU * AU);
}

// Funzione che integra secondo il metodo di Eulero.
// Accetta l'array di puntatori a FILE come argomento

void integra_orbite(const Parametri *param, FILE **fp){
    int n = param->n_pianeti;

    apri_file(fp, param->n_pianeti, param->sistema, param-> nome_pianeta);

    // Alloco dinamicamente i vettori posizione, velocità e accelerazione
    double *x = malloc(n*sizeof(double));
    double *y = malloc(n*sizeof(double));
    double *vx = malloc(n*sizeof(double));
    double *vy = malloc(n*sizeof(double));
    double *ax = malloc(n*sizeof(double));
    double *ay = malloc(n*sizeof(double));

    // Calcolo G normalizzato
    double G_norm = calcola_G_norm(param->G_SI, param->AU, param->M_star, param->anno_in_sec);

    // Condizioni iniziali per l'integrazione
    for(int i=0; i<param->n_pianeti; i++){
        x[i] = -param->r[i];  // metto meno perchè la condizione iniziale è che il pianeta si trova in (-r,0)
        y[i] = 0.0;
        vx[i] = 0.0;
        vy[i] = sqrt(G_norm * 1 / param->r[i]); // moltiplico per M_star= 1 perche uso G_norm 

        double r = sqrt(x[i]*x[i] + y[i]*y[i]);
        ax[i] = -(G_norm * x[i]) / (r*r*r);
        ay[i] = -(G_norm * y[i]) / (r*r*r);
    }
    
    double t=0.0;
    // Calcolo l'accelerazione al tempo t=0
    for(int i=0;i<param->n_pianeti; i++){
        double r= sqrt( x[i]*x[i] + y[i]*y[i] );
        ax[i]= -(G_norm * 1 * x[i])/ (r*r*r);     // moltiplico per M_star= 1 perche uso G_norm 
        ay[i]= -(G_norm * 1 * y[i])/ (r*r*r);
    }
    // Scrivo lo stato iniziale del sistema 
    scrivi_step(fp, param->n_pianeti, t, x, y, vx, vy, ax, ay, param);

    // Inizio il loop di integrazione
    // Parto dallo step 1 perche' lo step zero e' gia stato trovato sopra
    for(int step=1; step < param->n_passi; step++){
        
        // Aggiorno posizioni e velocita' 
        for(int i=0;i<param->n_pianeti; i++){

            // NOTA: se calcolo prima le posizioni e poi aggiorno le velocita' allora uso il
            //       metodo di Eulero standard. Se invece aggiorno prima le velocita' e poi
            //       le posizioni allora uso il metodo di Eulero-Cromer che si rivela molto piu' preciso!

            x[i]= x[i] + vx[i] * param->h;
            y[i]= y[i] + vy[i] * param->h;

            vx[i]= vx[i] + ax[i] * param->h;
            vy[i]= vy[i] + ay[i] * param->h;

            //x[i]= x[i] + vx[i] * param->h;
            //y[i]= y[i] + vy[i] * param->h;

        }

        // Aggiorno il tempo
        t= t+param->h;

        // Ricalcolo le accelerazioni per le NUOVE posizioni
        for(int i=0; i<param->n_pianeti; i++){
            double r = sqrt(x[i]*x[i] + y[i]*y[i]);
            ax[i] = -(G_norm * x[i]) / (r*r*r);
            ay[i] = -(G_norm * y[i]) / (r*r*r);
        }

        // Scrivo i risultati in un file output binario 
        if (step % param->output_step == 0){
            scrivi_step(fp, param->n_pianeti, t, x, y, vx, vy, ax, ay, param);
        }
    }

    chiudi_file(fp,param->n_pianeti);

    // Libero la memoria
    free(x);
    free(y);
    free(vx);
    free(vy);
    free(ax);
    free(ay);
}