//
//  ESERCIZIO 1
//  Risolvere la seguente funzione x^x=exp(-x)+1
//  Utilizzare il metodo della bisezione.
//  Fare prima un test del metodo su una parabola 
//
//  Created by Aurora Valentini on 30/09/25.
//

#include <stdio.h>
#include <math.h>


// Definisco le funzioni.

#ifdef TEST
double y(double x) {
    double a = 1, b = -3, c = 2;
    return a*pow(x,2) + b*x + c;
}
#endif

#ifdef TRASCEND
double y(double x) {
    return pow(x,x) - exp(-x) - 1;
}
#endif


 
// Metodo della bisezione
// Si sfrutta il teorema degli zeri, motivo per cui vogliamo che f(a)*f(b)<0

double bisezione(double inf, double sup, double epsilon) {
    if  (y(inf)*y(sup) >=0){
        printf("Non posso usare la bisezione per risolvere questa equazione.");
        return NAN;
}
    double c;
    while ((sup-inf)/2 > epsilon) {
        c= (inf+sup)/2;
        if (y(c)==0){
            return c;
        } else if (y(inf)*y(c)<0){
            sup=c;
        } else {
            inf=c;
        }
    }
    return c;
}
 


// Programma principale in cui chiamo la funzione bisezione

int main() {
     double inf= 0.5;
     double sup= 1.5;
     double epsilon= 0.001;
     double risultato= bisezione (inf,sup,epsilon);
     printf("Radice calcolata con metodo della bisezione: %.5f\n", risultato);
     return 0;
}

// Risultato con la funzione TRASCEND è 1.23
// Risultato con la parabola: in realtà passa due volte per zero, quindi ho due radici reali.
// Con il metodo della bisezione ne trovo solo una in base all' intervallo scelto.

