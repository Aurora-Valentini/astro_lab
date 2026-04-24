#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "read_func.h"

// Nella lettura dei files parameters che è non posizionale 
// potrebbe succedere che la riga n_pianeti venga dopo le righe delle posizioni
// quindi prima che io abbia allocato la memoria causando un segmentation fault.
// Per evitare faccio due letture del file: nella prima cerco solo n_pianeti e nome del sistema e alloco la memoria;
// nella seconda leggo i dati.

void leggi_param(const char *filename, Parametri *p){
    FILE *fp = fopen(filename, "r");
    if(!fp){ perror("fopen"); exit(1); }  //check
    
    char line[256];
    // Variabili per buffer temporanei
    char key[64], value_str[64];

    // ---- PRIMA LETTURA ----
    // Cerco prima n_pianeti e sistema (il nome)

    // Per sicurezza inizializzo a zero
    p->n_pianeti = 0;
    memset(p->sistema, 0, sizeof(p->sistema));

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || isspace(line[0]))
            continue;
        if (sscanf(line, " %[^ =] = %s", key, value_str) == 2){
            if (strcmp(key, "n_pianeti") == 0){
                p->n_pianeti = atoi(value_str);
            }
            else if (strcmp(key, "sistema") == 0){
                strcpy(p->sistema, value_str);
            } 
        }
    }

    // Genero un messaggio di errore se non trovo n_pianeti nel file parameters
    if (p->n_pianeti <= 0) {
        fprintf(stderr, "Errore: n_pianeti non trovato o non valido.\n");
        exit(1);
    }

    // Ora alloco la memoria. Alloco tanti array 'distanze(r)' quanti sono i pianeti 
    // e alloco tanti array 'nome_pianeti' quanti sono i pianeti.
    p->r = (double*) malloc(p->n_pianeti * sizeof(double));
    p->nome_pianeta = (char**) malloc(p->n_pianeti * sizeof(char*)); // Alloco la memoria per i puntatori
    
    for(int i = 0; i < p->n_pianeti; i++) {
        p->nome_pianeta[i] = (char*)malloc(64); // Alloco la memoria per i caratteri che compongono il nome
        p->nome_pianeta[i][0] = '\0'; // Inizializzo una stringa vuota
    }

    // Riporto il puntatore all'inizio del file per la seconda passata
    rewind(fp); 

    // ---- SECONDA LETTURA ----

    while (fgets(line, sizeof(line), fp)) {
        // Salto righe vuote o commenti
        if (line[0] == '#' || line[0] == '\n' || isspace(line[0]))
            continue;

        // ---- PARSING DI NUMERI ----

        //  Leggo la parola chiave (fino allo spazio o all'uguale) e il valore corrispettivo
        if (sscanf(line, " %[^ =] = %s", key, value_str) == 2) {

            // Saltiamo n_pianeti e sistema perché già letti nella prima passata
            if (strcmp(key, "n_pianeti") == 0 || strcmp(key, "sistema") == 0) continue;

            double val = atof(value_str);

            // Leggo le chiavi e gli associo un valore numerico
            if (strcmp(key, "AU") == 0) p->AU = val;
            else if (strcmp(key, "M_star") == 0) p->M_star = val;
            else if (strcmp(key, "anno_in_sec") == 0) p->anno_in_sec = val;
            else if (strcmp(key, "G_SI") == 0) p->G_SI = val;
            else if(strcmp(key, "h") == 0) p->h = val;
            else if(strcmp(key, "output_step") == 0) p->output_step = val;
            else if(strcmp(key, "n_passi") == 0) p->n_passi = (int)val;

            //Confronto i primi due caratteri della stringa con 'r_'
            else if(strncmp(key,"r_",2)==0){  
                int index = atoi(key+2) - 1; 
                // key+2 significa che parte dal terzo carattere e lo trasforma in intero. Quindi index parte da 0
                // Questo perche sono partita da 1 per definire r_1, r_2,... nel file parameter.
                // Qui controllo che l'indice stia tra 0 e n_pianeti
                if (index>=0 && index < p->n_pianeti){
                    p->r[index] = val;
                }
            }
            // ---- PARSING DI STRINGHE ----
            else if (strncmp(key,"nome_",5)==0) {
                int index = atoi(key+5) - 1;
                // Come sopra, sono partita da 1 nel definire nome_1, nome_2,... nel file parameter
                if(index>=0 && index < p->n_pianeti){
                    strcpy(p->nome_pianeta[index], value_str);
                }  
            } 
        }
    }
    fclose(fp);
}

void libera_param(Parametri *p){
    if(p->r){
        free(p->r);
    } 
    if(p->nome_pianeta) {
        for(int i = 0; i < p->n_pianeti; i++) {
            free(p->nome_pianeta[i]);
        }
        free(p->nome_pianeta);
    }
}
