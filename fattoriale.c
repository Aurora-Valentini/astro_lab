//
//  Risolvere un fattoriale con funzione ricorsiva
//
//  Created by Aurora Valentini on 26/09/25.
//

#include <stdio.h>
#include <math.h>



int fat( int n) {
    if (n==0) {
        return 1;
    } else {
        return n * fat(n-1);
        
    }
}

int main() {
    int i;
    
    // Immetti da linea di comando il numero di cui calcolare il fattoriale
    printf("Inserisci un numero: ");
    scanf("%d", &i);
    
    //Metto %d e non %lf perchè ho un numero intero

    int result = fat(i);
    
    printf("Il fattoriale di %d è: %d\n", i, result);

    return 0;
    
}
    
// Nota che arriva ad un massimo di 16 poi inizia a sparare numeri negativi. 
    



