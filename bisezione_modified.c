//
//  ESERCIZIO 1
//  Risolvere la seguente funzione x^x=exp(-x)+1
//  Utilizzare il metodo della bisezione.
//
//  Questo esercizio è una modifica all'esercizio già fatto ma con due differenze:
//  - usare I/O da linea di comando
//  - al posto di usare define per definire le funzioni usare i puntatori
//
//  Created by Aurora Valentini on 30/09/25.
//

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Qui non dichiaro più delle varibili globali! Uso minprintf
// a,b,c servono per la parabola.
// inf, sup e epsilon servono per intervallo e precisione del metodo della bisezione.

double inf= -0.7;
double sup= 1.5;
double epsilon= 0.001;


// Definisco le varie funzioni. Stavolta tutte ad argomenti varibili

#ifdef TEST
double test(double x, ...) {
    va_list ap;
    va_start(ap, x);

    double a = va_arg(ap, double);
    double b = va_arg(ap, double);
    double c = va_arg(ap, double);

    va_end(ap);

    return a*x*x + b*x + c;
}

#endif

#ifdef TRASCEND1
double trascend1(double x,...) {
    return pow(x,x) - exp(-x) - 1;
}
#endif

#ifdef TRASCEND2
double trascend2(double x,...) {
    return 3*pow(x,x) - exp(x);
}
#endif

#ifdef CUBIC
double cubic(double x,...) {
    return x*x*x +x+1;
}
#endif


// Definisco la funzione puntatore
// Chiamerò la funzione scelta da linea di comando, senza averla decisa a priori.

double (*myfunc)(double,...);


// Definisco il metodo della bisezione

double bisezione(double (*y)(double, ...), double inf, double sup, double epsilon,...) {
    va_list ap;
    va_start(ap, epsilon);

    // Leggo i parametri (se ci sono)
    double a = va_arg(ap, double);
    double b = va_arg(ap, double);
    double c = va_arg(ap, double);

    va_end(ap);
    
    if  (y(inf,a,b,c)*y(sup,a,b,c) >0){
        printf("Non posso usare la bisezione per risolvere questa equazione, la funzione non cambia segno nell'intervallo.");
        return NAN;
    }
    double media;
    while ((sup-inf)/2 > epsilon) {
        media= (inf+sup)/2;
        if (fabs(y(media,a,b,c))<epsilon){
            return media;
        } else if (y(inf,a,b,c)*y(media,a,b,c)<0){
            sup=media;
        } else {
            inf=media;
        }
    }
    // Aggiungo questo controllo perche' non mi basta che y(inf)*y(sup) < 0, ossia che la funzione cambi segno.
    // Controllo anche che |y(media)| sia minore della tolleranza.
    //(Lo faccio a causa di trascend2 che non ha radici e non passa per zero.
    
    media = (inf + sup)/2;
    if (fabs(y(media,a,b,c)) < epsilon)
        return media;
    else {
        printf("Non è stata trovata una radice con la precisione richiesta.\n");
        return NAN;
    }
}


// Programma principale in cui chiamo la funzione bisezione
int main(int argc, char *argv[]) {
    // Se da linea di comando passo più di due argomenti e il secondo argomento
    // è la parola TEST (==0 significa vero) uso test come funzione.
    // Altrimenti procedo con if e scelgo una delle altre.
    
#ifdef TEST
    if (argc >1 && strcmp(argv[1], "TEST")==0){
        if (argc != 5){
            printf("Attenzione, la funzione TEST richiede 3 parametri\n");
            exit(1);
        }
        myfunc = test;
        
        double a= atof(argv[2]);
        double b= atof(argv[3]);
        double c= atof(argv[4]);
        
        double risultato1 = bisezione(myfunc, inf, sup, epsilon, a, b, c);
        printf("Radice calcolata con metodo della bisezione: %.5f\n", risultato1);
        return 0;
    }
#else
    
#ifdef TRASCEND1
    myfunc = trascend1;
#elif defined(TRASCEND2)
    myfunc = trascend2;
#elif defined(CUBIC)
    myfunc = cubic;
#else
    printf("Funzione sconosciuta\n");
    exit(1);
#endif
#endif
          

    double risultato2= bisezione (myfunc,inf,sup,epsilon,0,0,0);
    printf("Radice calcolata con metodo della bisezione: %.5f\n", risultato2);
    return 0;
   
}


// Risultato con la funzione TRASCEND1 è 1.23
// Risultato con la parabola: in realtà passa due volte per zero, quindi ho due radici reali.
// Dovrei mettere un if per calcolare entrambe le radici ma con il metodo della bisezione
// ne trovo solo una nell' intervallo scelto.
// La TRASCEND2 non passa per zero quindi non posso usare bisezione.
// La CUBICA ha una radice in zero ma non è copmreso nell'intervallo scelto.

// Command line:
// gcc -DTEST bisezione_puntatori.c  -o bisezione_puntatori.x
// ./bisezione_puntatori.x TEST 1 -3 2
// Per gli altri casi uguale ma senza parametri.
