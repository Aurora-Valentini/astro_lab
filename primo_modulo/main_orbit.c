//
//  ESERCIZIO PRIMO MODULO
//
//  Si calcoli posizione, velocità e accelerazione a diversi tempi evolutivi 
//  per il Sistema Solare interno e per il sistema Trappist-1.
//
//  1. ORBITE PIANETI NEL SISTEMA SOLARE INTERNO (Mercurio, Venere, Terra, Marte)
//  2. ORBITE PIANETI NEL SISTEMA TRAPPIST-1 (5 pianeti piu' interni)
//      
//  Si supponga nulla l'eccentricita' di tutte le orbite (orbite circolari).
// 
//  I valori di normalizzazione (AU, Massa stella, Anno in secondi) sono contenuti in
//  parameter_sun.dat e parameter_trappist.dat 
//
//
//  Created by Aurora Valentini on 24/11/25.
//

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "read_func.h"
#include "integr_func.h"
#include "conversion.h"

//----------------------SISTEMA SOLARE INTERNO----------------------


int main(int argc, char *argv[]){
    // Check degli argomenti
    if (argc != 2) {
        printf("Uso: %s file_parametri\n", argv[0]);
        return 1;
    }

    // Leggo parametri dal file passato da linea di comando
    Parametri p;
    memset(&p, 0, sizeof(Parametri));
    leggi_param(argv[1], &p);

    // Preparo array di puntatori a FILE
    // Serve per passare i file alla funzione integra_orbite
    FILE **fp_bin = malloc(p.n_pianeti * sizeof(FILE *));
    if (!fp_bin) { perror("Errore malloc"); return 1; }

    // Inizio l'integrazione
    printf("--- Inizio Integrazione [%s] ---\n", p.sistema);
    integra_orbite(&p, fp_bin);
    printf("Integrazione completata con successo.\n");

    // Converto il file output da binario a ASCII
    printf("--- Inizio Conversione (Output step: %d) ---\n", p.output_step);
    converti_files(p.n_pianeti, p.sistema, p.nome_pianeta, &p);
    printf("Conversione completata.\n");

    // Ripulisco la memoria 
    // Libero l'array di puntatori
    free(fp_bin);      
    // Libero array delle posizioni r e dei nomi_pianeti usati nel parsing
    libera_param(&p);

    return 0;
}



















