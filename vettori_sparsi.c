//  ESERCIZIO
//  
//  Crea due vettori sparsi x e y di N=10.000 elementi. 
//  Genera quindi M indici casuali in cui mettere 1 per il vettore x. 
//  Poi genera altri M indici casuali in cui mettere 1 per il vettore y
//  Calcola infine il vettore z, dato dalla somma di x e y.
//
//  Quale è il minimo M che devi scgliere per avere in z almeno un elemento pari a 2?
//
//  Alla fine usa funzione ricorsiva per deallocare il tutto.  
//
//
//  Created by Aurora Valentini on 23/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definisco una variabile globale per il numero totale di elementi dei vettori.
int N = 10000;

// -----------------------------------------------------------
// STRUTTURE 
// -----------------------------------------------------------

// Definisco la struttura per un singolo nodo della lista (vettore sparso)
typedef struct nodo_vettore {
    int indice;                // Indice (posizione) del nodo considerato
    int valore;                // Valore del nodo non nullo
    struct nodo_vettore *next; // Puntatore al nodo successivo
} nodo_vettore;


// Definizione della struttura per l'intera lista
typedef struct vettore{
    int N;                  // Dimensione totale del vettore
    nodo_vettore *head;     // Puntatore al primo nodo della lista
} vettore;

// -----------------------------------------------------------
// FUNZIONI PER CREARE VETTORI E NODI
// -----------------------------------------------------------

// Funzione per creare il vettore.
// La funzione restituisce un puntatore a una stuttura. 
// v punta alla struttura vettore 
vettore* genera_vettore() {
    vettore* v = (vettore*) malloc(sizeof(vettore));
    if (v == NULL) {
        printf("Errore di allocazione\n");
        exit(EXIT_FAILURE);
    }
    v->head = NULL;  // Lista inizialmente vuota
    v->N = 0;
    return v;
}

// Funzione per creare il nodo.
nodo_vettore* genera_nodo(int indice, int valore) {
    nodo_vettore* nodo = (nodo_vettore*) malloc(sizeof(nodo_vettore));
    if (nodo == NULL) {
        printf("Errore di allocazione\n");
        exit(EXIT_FAILURE);
    }
    nodo->indice = indice;
    nodo->valore = valore;
    nodo->next = NULL;  
    return nodo;
}

// -----------------------------------------------------------
// FUNZIONI PER INSERIRE NODI
// -----------------------------------------------------------

// Funzione per generare indice casuale
int indice_random(int N) {
    return (int)( (double)rand() / (RAND_MAX + 1.0) * N );
}


// Questa funzione mi servirà per controllare se i nodi hanno stesso indice 
// al momento di fare la somma tra i due vettori. 
nodo_vettore* trova_nodo(vettore *v, int indice) {
    nodo_vettore *p = v->head;
    while (p != NULL) {
        if (p->indice == indice)
            return p; // Nodo trovato
        p = p->next;
    }
    return NULL; // Nodo non trovato
}

// Userò questa funzione per valutare se ho indici casuali ripetuti.
// Potrei avere più nodi con lo stesso indice. Per evitare uso trova_nodo.

int indice_ripetuto(vettore* v, int index) {
    return trova_nodo(v, index) != NULL;  // 1 se indice esiste, 0 se non esiste
}


// Funzione per generare M nodi casuali con valore 1 senza indici ripetuti
void nodi_random_unici(vettore* v, int M) {
    for (int i = 0; i < M; i++) {
        int index;
        do {
            index = indice_random(N);           // Genera indice casuale
        } while (indice_ripetuto(v, index));    // Rigenera il numero se l'indice è già presente

        nodo_vettore* nuovo = genera_nodo(index, 1);
        nuovo->next = v->head;                 
        v->head = nuovo;
    }
    v->N = M; // Numero di nodi non nulli inseriti
}

// -----------------------------------------------------------
// FUNZIONE PER LA SOMMA DI DUE VETTORI SPARSI
// -----------------------------------------------------------

// Voglio che il vettore z sia dato dalla somma dei vettori x e y.
vettore* somma_vettori(vettore *x, vettore *y) {
    vettore *z = genera_vettore();  // Crea vettore sparso z

    nodo_vettore *p = x->head;
    while (p != NULL) {
        // Inserisci tutti i nodi di x in z
        nodo_vettore *nuovo = genera_nodo(p->indice, p->valore);
        nuovo->next = z->head;
        z->head = nuovo;
        p = p->next;
    }

    nodo_vettore *q = y->head;
    while (q != NULL) {
        // Controllo se indice di y è già presente in z
        nodo_vettore *esistente = trova_nodo(z, q->indice);

        if (esistente != NULL) {
            // Se esiste, somma i valori
            esistente->valore += q->valore;
        } else {
            // Se non esiste, crea un nuovo nodo
            nodo_vettore *nuovo = genera_nodo(q->indice, q->valore);
            nuovo->next = z->head;
            z->head = nuovo;
        }

        q = q->next;
    }    
    return z;
}


// -----------------------------------------------------------
// FUNZIONI PER STAMPARE E DEALLOCARE VETTORI SPARSI
// -----------------------------------------------------------

// Funzione che printa tutti i nodi non nulli con valore 1 dei singoli vettori
void print_vettore(vettore *v, const char *nome) {
    nodo_vettore *p;
    p = v->head;
    printf("\nIl vettore sparso  %s contiene i seguenti nodi non nulli:\n", nome);
    printf("Indice\tValore\n");
    // Itera finché non si raggiunge la fine della lista (NULL)
    while (p != NULL) {
        printf("%d\t%d\n", p->indice, p->valore);
        p = p->next; // Passa al nodo successivo
    }
    printf("NULL\n"); // Indica la fine della lista
}


// Dealloco in modo ricorsivo tutta la memoria occupata dal vetotre e dai suoi nodi
void dealloco_vettore(vettore *v) {
    nodo_vettore *nodo, *temp;
    nodo = v->head;

    // Itera su ogni nodo per liberare la memoria
    while (nodo != NULL) {
        temp = nodo->next; // Salva il puntatore al nodo successivo
        free(nodo);        // Libera la memoria del nodo corrente
        nodo = temp;       // Passa al nodo successivo salvato
    }
    // Libera la memoria della struttura List stessa
    free(v);
    printf("Vettore deallocato correttamente.\n");
}


// -----------------------------------------------------------
// PROGRAMMA PRINCIPALE
// -----------------------------------------------------------

int main() {
    srand(time(NULL));

    vettore *x = genera_vettore();
    nodi_random_unici(x, 100);  // genera 10 nodi con valore 1 nel vettore x
    print_vettore(x, "X");

    vettore *y = genera_vettore();
    nodi_random_unici(y, 100);  // genera 10 nodi con valore 1 nel vettore y
    print_vettore(y, "Y");

    vettore *z = somma_vettori(x,y); // genero il vettore somma
    print_vettore(z, "Z= X+Y");



    dealloco_vettore(x);
    dealloco_vettore(y);
    dealloco_vettore(z);

    return 0;

}


// Per avere almeno un elemento pari a 2 nel vettore somma devo fare considerazioni statistiche:
// M = indici scelti casualmente con nodi =1
// N = valore totale di indici
//
// M/N = probabilità che un indice sia comune a entrambi i vettori x e y
// M^2/N = numero di indici in comune
//
// Impongo M^2/N >=1 --> M>sqrt(N)
// Dato che N= 10000 allora M deve essere almeno 100 per avere un valore pari a 2 nella somma.
