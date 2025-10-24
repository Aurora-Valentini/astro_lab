//
//  ESERCIZIO 2
//  Stimare il valore di pi greco tramite metodo monte carlo.
//  Considero un cerchio di raggio r inscritto in un quadrato di lato 2r.
//  Area cerchio: pi*r^2
//  Area quadrato: (2*r)^2
//
//
//  Created by Aurora Valentini on 26/09/25.
//

#include <stdio.h>
#include <stdlib.h>

// Il seguente pacchetto serve affinche il programma generi ogni volta dei numeri casuali differenti.
#include <time.h>


// Definisco la funzione che genera numeri casuali nell'invervallo [-r;r]
// Passo come argomento della funzione random gli estremi degli intevalli
// in cui generare i numeri. Essendo che il quadrato sta tra [-r;r] mi basta un argomento solo.
// Per passare da numeri generati da [0;1] a intevallo a mia scelta uso la formula: a+(b-a)*n_random con b>a.

double punti_random(double r) {
    double n_random = (double)rand()/ RAND_MAX;
        return -r + 2*r*n_random ;
}


// Devo capire quanti di questi punto cadono dentro il cerchio.
// Area quadrato = area totale
// P(dentro cerchio) = area_cerchio/ area quadrato = pi/4
// Da cui pi = 4* (numero punti nel cerchio)/ punti totali
// I punti nel cerchio li ho chiamati count

double p_inside(double r, int N){
    int count=0;
    
    for (int i=0; i<N; i++) {
        double x = punti_random(r);
        double y = punti_random(r);
           
        if (x*x + y*y <= r*r) {
        count += 1;
    }
  }
    return  (4.0 * count)/N;
}

// Programma principale in cui chiamo la funzione che calcola i punti dentro il cerchio

int main() {
    int N;
    double r;
    
    srand(time(NULL));
    
    printf("Digita il numero di numeri casuali N che desideri:");
    scanf("%d", &N);
    printf("Digita il valore del lato che desideri:");
    scanf("%lf", &r);
    
    double stima_pi = p_inside(r,N);
    printf("Stima di pi greco: %.6f\n", stima_pi);
    
    return 0;
}

