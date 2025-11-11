//
//  ESERCIZIO RANDOM NUMBER (PARTE 2)
//  In questo esercizio devo generare un file circle.txt e un file sphere.txt.
// 
//
//  Da linea di comando dovrò passare: sphere.x N R oppure circle.x N R
//
//  Devo sfruttare la generazione di numeri casuali e struct point.
//  Devo usare rispettivamente coordinate polari e sferiche.
//
//  Created by Aurora Valentini on 11/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//  Dichiaro le variabili globali ossia:
// - numero di punti N 
// - raggi dei cerchi  R

int N;
double R;


// Definisco una struttura che contiene i punti x e y
struct point2D{
    double x;
    double y;
};

// Definisco la funzione che genera numeri casuali all'interno della circonferenza [-r,+r]
// Passo come argomento della funzione random il raggio del cerchio

struct point2D punti_random2D(double R){
    // In questo modo i punti si addensano al centro della circonferenza
    //double r = (rand()/(RAND_MAX+1.)); 
    // Questo è il modo corretto per avere una distribuzione uniforme dentro la circonferenza
    double r = R*sqrt(rand()/(RAND_MAX+1.));     
    double phi = 2*M_PI*rand()/(RAND_MAX+1.);

    struct point2D p;
    p.x = r * cos(phi);
    p.y = r * sin(phi);
    return p;
}

// Ora generalizzo al caso di una sfera
struct point3D{
    double x;
    double y;
    double z;
};



struct point3D punti_random3D(double R){

    // Se r = raggio random con distribuzione uniforme i punti si concentreranno vicino al centro della sfera
    //double r = (rand()/(RAND_MAX+1.));                               
    // Correggo tenendo conto del volume della sfera      
    double r = R * pow((double)rand() / (RAND_MAX + 1.0), 1.0/3.0);      

    // Se scegliessi theta uniforme tra [0,pi] allora i punti di addenserebbero vicino ai poli. 
    // (1-2*u) con u= numero random fa si che cos(theta) stia tra [-1,+1]. 
    // Infine uso arccos per trovare la variabile theta.
    double theta = acos(1 - 2 * ((double)rand() / (RAND_MAX + 1.0)));  
    double phi = 2*M_PI*rand()/(RAND_MAX+1.);

    struct point3D p;
    p.x = r * sin(theta)*cos(phi);
    p.y = r * sin(theta)*sin(phi);
    p.z = r * cos(theta);
    return p;
}



int main(int argc, char *argv[]){

    srand(time(NULL));

    if (argc != 3){
            printf("Attenzione, sono richiesti 2 parametri: N R\n");
            exit(1);
    }

    // Nota: argv[0] è sempre il nome del programma che compili
    
    N= atoi(argv[1]);
    R= atof(argv[2]);

    
    char filename1[50];
    sprintf(filename1, "circles_%d_%.2f.txt", N, R);
    FILE *fp1 = fopen(filename1, "w");

    for (int i = 0; i < N; i++) {
        struct point2D p = punti_random2D(R);
        fprintf(fp1, "%.2f %.2f\n", p.x, p.y);
    }

    fclose(fp1);
    printf("Creato %s con %d punti (r=%.2f)\n", filename1, N, R);

    
    char filename2[50];
    sprintf(filename2, "sphere_%d_%.2f.txt", N, R);
    FILE *fp2 = fopen(filename2, "w");

    for (int i = 0; i < N; i++) {
        struct point3D p = punti_random3D(R);
        fprintf(fp2, "%.4f %.4f %.4f\n", p.x, p.y, p.z);
    }

    fclose(fp2);
    printf("Creato %s con %d punti (r=%.2f)\n", filename2, N, R);

    return 0;
 
}


//--------------COMANDI GNUPLOT----------------
// Per plottare la circonferenza:
// set size ratio -1 
// plot "circles_2000_1.00.txt" using 1:2 with p pt 7 ps 0.5

// Per plottare la sfera:
// set view equal xyz
// splot "sphere_2000_1.00.txt" using 1:2:3 with p pt 7 ps 0.5