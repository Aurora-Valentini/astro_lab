//
//  ESERCIZIO 
//  
// Integra l'equazione dell'evoluzione del fattore di scala a(t).
// Dato un modello cosmologico, l'evoluzione è data dalle equazioni 
// di Friedmann-Lemaitre-Robertson-Walker.
//
// z= redshift cosmologico t.c. 1+z= 1/a
// omega_k= 1-omega_m-omega_de
// H0= 70 km/s/Mpc
// Equazione di stato della Dark Energy: p= w*rho con w=-1 
// 
// Il programma utilizza delle funzione di libreria del Numerical Recipe che devono essere compilate assieme
// al programma, nello specifico devo avere 'nrutil.h' e compilare 'nrutil.c', 'rk4.c' e 'friedmann.c'.
//
//
//
//  Created by Aurora Valentini on 12/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nrutil.h"

//  Dichiaro le variabili globali ossia:
float H0; // (km/s/Mpc)
float omega_m0, omega_de0, omega_k0, w;



// Definisco la funzione da integrare da/dt = a*H(a)
void derivs(float t, float a[], float dadt[] ){
    float y = a[1];
    float H =  H0*sqrt( pow(y,-3.)*omega_m0 + pow(y,-2.)*omega_k0 + pow(y,-3.*(1.+w))*omega_de0  );
    dadt[1]= y*H;
}

// Definisco la funzione RK4
void rk4( float y[], float dydx[], int n, float x, float h, float yout[],
          void (*derivs)(float, float [], float []) );

// Definisco la funzione che andrà ad integrare tramite RK4 e scriverà i risultati su file.
void integr(float a0, float t0, int NSTEP,
                     double omega_m0, double omega_de0, float w,
                     const char *filename){
                    
    float h = 0.001;   // Step numerico dell'integrazione in Gyr, piccolo ma non troppo
    int nvar = 1;

    // Alloco i vettori dinamici che vanno poi usati nel metodo Runge-Kutta
    float *a    = vector(1, nvar);
    float *dadt = vector(1, nvar);
    float *aout = vector(1, nvar);

    // Inizializzo i parametri
    a[1] = a0;
    float t = t0;
    float t_fin = 0.0;

    // Apro il file su cui scriverò gli output
    FILE *fp = fopen(filename, "w");
    
    // In questo ciclo integro fino a trovare il tempo dell'Universo (t_fin) per un dato modello
    for (int i = 0; i <= NSTEP; i++) {

        // Calcolo derivata 
        derivs(t, a, dadt);

        // Uso il metodo RK4 
        rk4(a, dadt, nvar, t, h, aout, derivs);

        // Aggiorno le variabili 
        a[1] = aout[1];
        t = t+h;

        // Impongo al ciclo di fermarsi quando a=1 ossia a t=oggi
        if (a[1] >= 1.0f) {
         t_fin=t;

         // Scrivo l'età dell'Universo oggi per un dato modello sul terminale
         printf("Età dell'Universo t_fin = %f Gyr\n", t_fin); 
         break;
        }
    }
    
    a[1] = a0;
    t = t0;
    
    // In questo secondo ciclo integro fino a t_fin trovato sopra e scrivo su file la differenza t-t_fin
    for (int i=0; i<=NSTEP; i++) {
        derivs(t, a, dadt);
        rk4(a, dadt, nvar, t, h, aout, derivs);
        a[1] = aout[1];
        fprintf(fp, "%f\t%f\n", t - t_fin, a[1]);

        // Per verificare printo t, t_fin e la loro differenza
        // printf("DEBUG: t = %f, t_fin = %f, t-t_fin = %f\n", t, t_fin, t - t_fin);

        t = t + h;
        if (a[1] >= 1.0) break;
    }

    fclose(fp);

    // Libero memoria
    free_vector(a, 1, nvar);
    free_vector(dadt, 1, nvar);
    free_vector(aout, 1, nvar);
}

// Definisco la funzione che serve a convertire H0 in Gyr
float convert_H0(float H0){
    // Definisco H0
    float H0_km_s_Mpc= 70.0;
    // Lo converto 
    float Mpc_in_m= 3.085677581e22; 
    float H0_SI = H0_km_s_Mpc * 1000.0 / Mpc_in_m;  // [s^-1]

    // Converto i secondi in Gyr
    double sec_per_year = 365.25 * 24 * 3600.0;
    double sec_per_Gyr = sec_per_year * 1e9;

    // H0 in Gyr^-1
    double H0_Gyr = H0_SI * sec_per_Gyr;

    return H0= H0_Gyr;
}

int main(int argc, char *argv[]) {

    H0= convert_H0(H0);
    
    // Passo da tastiera i parametri per i vari modelli  
    if (argc != 4) {
        printf("Sono richiesti 3 parametri!");
        printf("Uso: ./friedmann omega_m0 omega_de0 w\n");
        exit(1);
    }
       
    omega_m0= atof(argv[1]);
    omega_de0= atof(argv[2]);
    w= atof(argv[3]);


    // In realta i parametri da fornire non sono 4 ma 3 poiche'
    omega_k0= 1.- omega_m0-omega_de0;

    int i;
    int nvar =1; // ossia ho solo una equazione da integrare
    float t,h;
    int NSTEP = 50000; //numero di step arbitrariamente scelto
    float t0=0.;
    float a0=1e-4;


    //File di output
    char filename[50];
    sprintf(filename, "friedmann_omega_m%s_omega_qde%s_w%s.txt", argv[1], argv[2], argv[3]);
    
    integr(a0, t0, NSTEP,
                    omega_m0, omega_de0, w,
                    filename);


    printf("File generato: %s\n", filename);

    return 0;

}

/*  Per compilare:
 gcc friedmann.c rk4.c nrutil.c -lm -o friedmann.x
 ./friedmann.x 0.3 0.7 -1 

----------- COMANDI GNUPLOT ---------------
 set xlabel "Tempo (Gyr)"
 set ylabel "Fattore di scala a(t)"
 plot  "friedmann_omega_m0.3_omega_qde0.7_w-1.txt" using 1:2 with lines title "Lambda CDM",\
 "friedmann_omega_m1._omega_qde0._w-1.txt" using 1:2 with lines title "Einstein-de Sitter",\ 
 "friedmann_omega_m0.3_omega_qde0._w-1.txt" using 1:2 with lines title "Open CDM",\
 "friedmann_omega_m1.2_omega_qde0._w-1.txt" using 1:2 with lines title "Closed CDM",\
*/