//
// Risolvere equazioni di secondo grado
//
// Created by Aurora Valentini
//
// 23/09/2025

// Step:
// Trovare i coeffiecienti a, b, c
// Calcolare il discriminante
//

#include <stdio.h>
#include <math.h>

// Utilizzo i puntatori

void mycoeff (double *a,double *b,double *c){
    // Immettere i coefficienti da linea di comando:
    printf("Inserisci il coefficiente a: ");
    scanf("%lf", a);
    printf("Inserisci il coefficiente b: ");
    scanf("%lf", b);
    printf("Inserisci il coefficiente c: ");
    scanf("%lf", c);
}


int main() {
    double a,b,c;
    mycoeff(&a,&b,&c);
    // Calcolo del discriminante
    
    double Delta = b*b - 4*a*c;

    // Se Delta >0 ho due soluzioni reali e distinte
    
    if (Delta > 0) {
        double x1 = (-b + sqrt(Delta)) / (2*a);
        double x2 = (-b - sqrt(Delta)) / (2*a);
        printf("Due soluzioni reali: x1 = %.2f, x2 = %.2f\n", x1, x2);
        
    // Se Delta =0 ho una soluzione reale coincidente
        
    } else if (Delta == 0) {
        double x = -b / (2*a);
        printf("Soluzione reale doppia: x = %.2f\n", x);
    } else {
        
    // Se Delta < 0 ho soluzioni complesse
        
      printf("Non esistono soluzioni reali");
    }

    return 0;
}

