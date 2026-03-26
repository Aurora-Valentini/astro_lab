#ifndef INTEGR_FUNC_H
#define INTEGR_FUNC_H

#include <stdio.h>
// Dato che mi serve la struct Parametri
#include "read_func.h"

// Definisco una struct che rende tutto piu semplice per scrivere il file binario
typedef struct {
    double t, x, y, vx, vy, ax, ay;
} Dato;


// =====================
// PROTOTIPI DELLE FUNZIONI
// =====================

// Questa funzione apre un file per ogni pianeta di ogni sistema.
void apri_file(FILE **fp, int n, const char *sistema, char **nome_pianeta);

// Scrive i vari step dell'integrazione nel file
void scrivi_step(FILE **fp, int n, double t, double *x, double *y, double *vx, double *vy, double *ax, double *ay, const Parametri *p);

// Chiude il file dopo aver scritto i valori
void chiudi_file(FILE **fp, int n);

// Calcola il valore di G normalizzato secondo il sistema utilizzato (solare o Trappist-1)
double calcola_G_norm(double G_SI, double AU, double M_star, double anno_in_sec);

// Funzione che integra secondo il metodo di Eulero
void integra_orbite(const Parametri *param, FILE **fp);


#endif