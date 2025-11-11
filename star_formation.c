//
// ESERCIZIO STAR FORMATION
//
// Obiettivo: Risolvere l'equazione differenziale ordinaria (ODE) per la formazione stellare.
// 
// Primo metodo: METODO DI EULERO
// Secondo metodo: METODO DI RUNGE - KUTTA
// 
// Risolvo prima una ODE di ordine zero, poi risolvo una ODE del primo ordine 
// Per entrambe confronto i risultati numerici con quelli analitici.
//
// Infine risolvo una ODE accoppiata, dato che non assumo più rho_gas costante:
// drho_gas/dt = - drho_star/dt
// 
//
// Legenda:
// rho_gas = densita' del gas per la formazione stellare
// c_star = efficienza di formazione stellare
// rho_star = densita' di stelle
// t_gas = tempo tipico
//
//
//
//
//  Created by Aurora Valentini on 10/11/25.
//

#include <stdio.h>
#include <math.h>

// Definisco i parametri globali
double c_star= 0.02; // 
double t_gas= 1.5; // [Gyr]

// Derivata della densità stellare
// drho_star/dt = (c_star * rho_gas) / t_gas

// Definisco quindi la seguente funzione (ODE di ordine zero)
double f1 (double rho_gas) {
    return  (c_star*rho_gas)/t_gas;
}

//Definisco la funzione analitica per confrontare i risultati
double f_analitica1 (double t1, double rho_gas, double rho_star0, double t0) {
    return rho_star0 + (c_star*rho_gas)/t_gas * (t1-t0);
}

// ODE del primo ordine
// drho_star/dt = (c_star * rho_gas) / t_gas  * t
double f2 (double rho_gas, double t2) {
    return  (c_star*rho_gas)/t_gas *t2;
}

//Definisco la funzione analitica per confrontare i risultati
double f_analitica2 (double t2, double rho_gas, double rho_star0, double t0) {
    return rho_star0 + 0.5* ((c_star*rho_gas)/t_gas) * (t2-t0)*(t2-t0);
}

double f_couple1 (double rho_gas) {
    return  -(c_star*rho_gas)/t_gas;
}

int main(){
    double t0=0.0;
    double rho_star0=0.0;
    double rho_gas0=10.0;

  //  double rho_gas = 10.0;       // assumo rho_gas costante = 10 [M_sun/kpc^2]
    double h = 0.2*t_gas;      // passo di integrazione
    int n = (int) (2*t_gas/h);           // numero di passi, sono paranoid perche' t_gas è double

    //Impongo le condizioni iniziali
    double rho_star=rho_star0;
    double rho_gas1= rho_gas0;
    double t=t0;


    printf("t\tAnalitica\tEulero\tErrore\n");
    // Metodo di Eulero (ODE di ordine zero)
    for (int i = 0; i < n; i++) {
       double rho_gas = 10.0; // assumo rho_gas costante = 10 [M_sun/kpc^2]
       t=t+h;
       rho_star= rho_star + h*f1(rho_gas);

       double rho_an1= f_analitica1(t, rho_gas, rho_star0, t0);
       double err1 = fabs(rho_an1-rho_star);

       printf("%.2f\t%.6f\t%.6f\t%.6f\n", t, rho_an1, rho_star, err1);
    }
    printf("t\tAnalitica\tEulero\tErrore\n");

    // Per sicurezza reimposto variabli gia' usate.
    rho_star=rho_star0;
    rho_gas1= rho_gas0;
    t=t0;

    // Metodo di Eulero (ODE del primo ordine)
    for (int i = 0; i < n; i++) {
       double rho_gas = 10.0; // assumo rho_gas costante = 10 [M_sun/kpc^2]
       t=t+h;
       rho_star= rho_star + h*f2(rho_gas, t);

       double rho_an2= f_analitica2(t, rho_gas, rho_star0, t0);
       double err2 = fabs(rho_an2-rho_star);

       printf("%.2f\t%.6f\t%.6f\t%.6f\n", t, rho_an2, rho_star, err2);
    }
    
    // Per sicurezza reimposto variabli gia' usate.
    rho_star = rho_star0;
    rho_gas1 = rho_gas0;
    t=t0;

    printf("t\t Rho_star\t Rho_gas\n");
    for (int i = 0; i < n; i++) {
       t=t+h;
       rho_star= rho_star + h*f1(rho_gas1);
       rho_gas1= rho_gas1 + h*f_couple1(rho_gas1);

       printf("%.2f\t%.6f\t%.6f\n", t, rho_star, rho_gas1);
    }

return 0;

}







// ------------------------ COMMENTI FINALI -----------------------
// Dal confronto dell'errore tra metodo analitico e metodo di Eulero
// dovrei avere che l'accurattezza aumenta più piccolo è h, però cosi'
// aumento il tempo di calcolo.

// Nel primo caso della derivata costante effettivamente coincidono (err=0)!
// Puoi provare a mettere il formato dell'errore in %.6e per il formato scientifico 
// per avere piu cirfre significative.

// Nel secondo caso si vede che il metodo di Eulero non è piu ottimale,
// gli errori sono diversi da zero. 

// Nel terzo caso, quello del sistema accoppiato ved che nel tempo la densita'
// di stelle aumenta mentre quella di gas diminuisce come prevedibile.
