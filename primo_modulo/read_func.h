#ifndef READ_FUNC_H
#define READ_FUNC_H


// =====================
// STRUTTURA PARAMETRI
// =====================
typedef struct {
    double AU;
    double G_SI;
    double M_star;
    double anno_in_sec;
    double h;
    int output_step;
    int n_pianeti;
    int n_passi;

    //Mi serviranno per creare file di output riconducibili ad ogni pianeta
    char sistema[64];
    char **nome_pianeta; // Array di stringhe con i nomi dei pianeti
    double *r;           // Array dinamico per le distanze

} Parametri;

// =====================
// PROTOTIPI DELLE FUNZIONI
// =====================

// Le funzioni sono definite in read_func.c

// Legge il file ASCII di parametri
void leggi_param(const char *filename, Parametri *p);

// Libera la memoria allocata dinamicamente
void libera_param(Parametri *p);

#endif
