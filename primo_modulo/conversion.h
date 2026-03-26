#ifndef CONVERSION_H
#define CONVERSION_H

// Includo anche integr_func perche' mi serve la stric Dato anche qui
#include "integr_func.h"
#include <stdio.h>

// =====================
// PROTOTIPI DELLE FUNZIONI
// =====================

// Questa funzione converte i singoli file binari in ASCII
void bin_to_ascii(const char *file_bin, const char *file_txt, int step);

// Dato che avro' files diversi per ogni pianeta uso questa funzione che li converte
void converti_files(int n, const char *nome_sistema, char **nome_pianeta, const Parametri *param);

#endif