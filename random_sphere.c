//
//  ESERCIZIO
//  In questo esercizio devo generare due file sphere%05d_%05.5f.txt
//
//  Da linea di comando dovrò passare: sphere.x N M r1 r2
//  sfere 0001 deve avere N punti casuali all'interno
//  sfere 0002 deve avere M punti casuali all'interno
//
//  Ps: anche se i file si chiamano sfere in realtà si intende circonferenze
//
//  Devo sfruttare la generazione di numeri casuali e struct point.
//
//  Created by Aurora Valentini on 10/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//  Dichiaro le variabili globali ossia:
// - numero di punti N e M,
// - raggi dei cerchi r1, r2

int N, M;
double r1, r2;

// Definisco la funzione che genera numeri casuali all'interno del cerchio [-r,+r]
// Passo come argomento della funzione random il raggio del cerchio

double punti_random(double r) {
    double n_random = (double)rand()/ RAND_MAX;
        return  -r + 2*r*n_random ;
}

// Definisco una struttura che contiene i punti x e y.

struct point{
    double x;
    double y;
};


// Scrivo il ciclo che restituisce le cooordinate dei punti random generati nei due cerchi e li scrive sul file output corrispondente.
// Devo mettere un while affinche generi numeri casuali finche poprio il numero desiderato (N o M) caschi dentro il cerchio.

double p_inside(double r, int N, const char *filename){
    FILE *fp = fopen(filename, "w");
    struct point pt;
    
    int count=0;
    while (count<N){
        
        pt.x = punti_random(r);
        pt.y = punti_random(r);
           
        if (pt.x*pt.x + pt.y*pt.y <= r*r) {
            fprintf(fp, "%.4f %.4f\n", pt.x, pt.y);
            count++;
        }
    }
    fclose(fp);
    printf("Creato %s con %d punti (r=%.2f)\n", filename, N, r);
    return N;
}

// Programma principale:

int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    
    if (argc != 5){
            printf("Attenzione, sono richiesti 4 parametri: N M r1 r2\n");
            exit(1);
    }
    
    // Nota: argv[0] è sempre il nome del programma che compili
    
    N= atof(argv[1]);
    M= atof(argv[2]);
    r1= atof(argv[3]);
    r2= atof(argv[4]);
    
    // Ora che ho letto e variabili le dispongo in un array
    // Mi sarà utile dopo per il ciclo.


    int n_punti[]= {N,M};
    double raggi[]= {r1,r2};
       
    for (int i =0; i<2; i++) {
        char filename[50];
        sprintf(filename, "sphere%05d_%05.5f.txt", i+1, raggi[i]);
        int punti_generati= p_inside(raggi[i], n_punti[i], filename);
    }
    return 0;
}

// Da linea di comando:
// ./esercizio_sphere.x 1000 200 1 2

// Per plottare su gnuplot:
// plot "sphere00001_1.00000.txt" using 1:2 with points pt 7 title "r1"
