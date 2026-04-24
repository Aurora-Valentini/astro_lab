#include <stdio.h>
#include <stdlib.h>
#include "conversion.h"

// Questa funzione converte i singoli file binari in ASCII
void bin_to_ascii(const char *file_bin, const char *file_txt, int step) {

    // Leggo i file binari già generati dopo l'integrazione
    FILE *bin = fopen(file_bin, "rb");
    if (bin == NULL) {
        perror("Errore apertura file .bin");
        exit(1);
    }

    // Apro i file .txt 
    FILE *txt = fopen(file_txt, "w");
    if (txt == NULL) {
        perror("Errore apertura file .txt");
        fclose(bin);
        exit(1);
    }

    Dato d;

    // Ho gia' usato output step in integra orbite, quindi qui leggo tutti i dati.
    while (fread(&d, sizeof(Dato), 1, bin) == 1) {
        fprintf(txt, "%.12e\t%.12e\t%.12e\t%.12e\t%.12e\t%.12e\t%.12e\n", 
            d.t, d.x, d.y, d.vx, d.vy, d.ax, d.ay);
    }

    fclose(bin);
    fclose(txt);
}

// Dato che avro' files diversi per ogni pianeta uso questa funzione che li converte
void converti_files(int n, const char *nome_sistema, char **nome_pianeta, const Parametri *param) {

    char file_bin[256];
    char file_txt[256];

    int step = param-> output_step;

    for (int i = 0; i < n; i++) {

        snprintf(file_bin, sizeof(file_bin), "%s_%s.bin",
                 nome_sistema, nome_pianeta[i]);

        snprintf(file_txt, sizeof(file_txt), "%s_%s.txt",
                 nome_sistema, nome_pianeta[i]);

        bin_to_ascii(file_bin, file_txt, step);
    }
}

