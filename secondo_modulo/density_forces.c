#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "physic.h"

// ----------------- KERNEL E CALCOLO DELLA DENSITA ---------------------

// Definisco la funzione che calcola il kernel W(q)
double kernel_W(double r, double h) {
    double q = fabs(r) / h;
    double inv_h = 1.0 / h;
    double sigma = (2.0 / 3.0); 

    if (q >= 0.0 && q < 1.0) {
        return sigma * inv_h * (1.0 - 1.5 * q * q + 0.75 * q * q * q);
    } else if (q >= 1.0 && q < 2.0) {
        return sigma * inv_h * 0.25 * (2.0 - q) * (2.0 - q) * (2.0 - q);
    } else {
        return 0.0;
    }
}

// Per l'evoluzione mi servira' anche la derivata quindi la calcolo
// Nota: è negativa.
double kernel_dW(double r, double h) {
    double q = fabs(r) / h;
    double inv_h = 1.0 / h;
    double sigma = (2.0 / 3.0); 
    

    if (q >= 0.0 && q < 1.0) {
        return sigma * inv_h * inv_h * (-3.0 * q + 2.25 * q * q);
    } else if (q >= 1.0 && q < 2.0) {
        return sigma * inv_h * inv_h * (-0.75 * (2.0 - q) * (2.0 - q));
    } else {
        return 0.0;
    }   
}

// Con questa funzione calcolo la densita' all'interno del supporto compatto definito da h_i
// Va usata con quicksort.
void density_press_qs(Particle *part, Parameter *param) {
    // Scorro l'array delle i-esime particelle
    for (int i = 0; i < param->n_totale; i++) {
        // Inizializzo l'array a zero
        double rho_sum = 0.0;
        
        // Range di ricerca del doppio (32*2) per sicurezza
        int range = 64; 
        int start = (i - range < 0) ? 0 : i - range;
        int end = (i + range >= param->n_totale) ? param->n_totale - 1 : i + range;
        
        
        for (int j = start; j <= end; j++) {

            double r = fabs(part[i].pos - part[j].pos);
            double Wi = kernel_W(r, part[i].h);
            rho_sum += part[j].mass * Wi;
        }
        
        // Aggiorno la densita' nella struct della particella i-esima
        part[i].rho = rho_sum;
        part[i].p = (param->gamma - 1.0) * part[i].rho * part[i].u;

    }
}

// Con questa funzione calcolo nuovamente la densita' ma nel caso delle cell-linked list
void density_press_ll(Particle *part, Parameter *param) {

    for (int i = 0; i < param->n_totale; i++) {
        double rho_sum = 0.0;
        // Chiamo cosi' le posizioni e gli h relativi alla particella i-esima
        double pos_i = part[i].pos;
        double h_i = part[i].h;     

        // Trovo l'indice della cella per la particella i-esima
        int k_i = (int)((part[i].pos + param->L/2.0) / param->d_cell);

        // Ciclo sulle 3 celle vicine: k-1, k, k+1
        for (int offset = -1; offset <= 1; offset++) {
            
            // CONDIZIONI PERIODICHE: 
            // int k_nbr = (k_i + offset + param->n_cell) % param->n_cell;

            // Se k_i + offset esce fuori (es. -1), 
            // il modulo % lo riporta dentro (es. all'ultima cella). 
            // Questa pero' non è realistica come situazione
            
            int k_nbr = k_i + offset;
            // Se la particella si trova nella prima cella, l'offset cercherebbe la cella -1 che non esiste 
            // quindi uso continue per evitare che accada (idem per l'ultima cella)
            if (k_nbr < 0 || k_nbr >= param->n_cell) continue;

            // Prendo la prima particella in quella cella
            Particle *j = param->head[k_nbr];

            // Finche' ci sono particelle in questa cella...
            while (j != NULL) {
                // Calcolo distanza fisica r tra la particella i e la particella j
                double dist = pos_i - j->pos;
                double r = fabs(dist);
                double h_medio = 0.5 * (h_i + j->h);

                // Se sono vicine abbastanza, calcolo il kernel
                if (r < 2.0 * h_medio) {
                    rho_sum += j->mass * kernel_W(r, h_medio);
                }

                // Passo alla prossima particella nella stessa cella
                j = j->next; 
            }
        }

        // Una volta controllate le 3 celle adiacenti, calcolo densita' e pressione
        part[i].rho = rho_sum;
        part[i].p = (param->gamma - 1.0) * part[i].rho * part[i].u;
    }
}

// ----------------- CALCOLO DELLE ACCELERAZIONI ---------------------

// Definisco la viscosita' artificiale
double viscosity(double rho_i, double rho_j, double v_i, double v_j, double x_i, double x_j, double h_ij, double c_ij) {

    // Calcolo le differenze
    double v_ij = v_i - v_j;
    double r_ij = x_i - x_j;

    // Se le particelle si allontanano (v*r > 0), non c'è viscosità
    if (v_ij * r_ij >= 0) return 0.0;

    // 4. Calcolo di rho_ij (media aritmetica delle densità)
    double rho_ij = 0.5 * (rho_i + rho_j);

    //Parametri standard usati per generare la viscosita' artificiale
    double alpha = 1.0; 
    double beta = 2.0;  
    double eps = 0.01; 

    double mu_ij = (h_ij * v_ij * r_ij) / (r_ij * r_ij + eps * h_ij * h_ij);
    double pi_ij = (-alpha * c_ij * mu_ij + beta * mu_ij * mu_ij) / rho_ij;

    return pi_ij;
}


// Calcolo l'accelerazione usando gli stessi cicli che avevo usato per la densita e la pressione secondo 
// il metodo quicksort
void accel_qs(Particle *part, Parameter *param) {
    // Ordino le particelle per posizione con quicksort prima si usare questa funzione nel main

    for (int i = 0; i < param->n_totale; i++) {
        // Inizializzo gli array delle acccelerazioni e della variazinoe della energia interna.
        part[i].acc = 0.0;
        part[i].du = 0.0;

        if (part[i].ghost == 1) continue;

        // Uso lo stesso range che ho usato per la densità
        int range = 64; 
        int start = (i - range < 0) ? 0 : i - range;
        int end = (i + range >= param->n_totale) ? param->n_totale - 1 : i + range;
        

        for (int j = start; j <= end; j++) {
            if (i == j) continue;

            double dist = part[i].pos - part[j].pos;
            double r = fabs(dist);
            double h_medio = 0.5 * (part[i].h + part[j].h);

            if (r < 2.0 * h_medio && r > 1e-12) {
                // Calcolo dW/dr 
                double dWi = kernel_dW(r, h_medio);
               
                // Gradiente di W = dW/dr * r_versore (considero anche l'orientazione tra i e j)
                // Dato che particella i è a destra di j si ha grad_W è negativo
                double grad_W = dWi * (dist/ r);

                // Calcolo velocità del suono per la viscosità
                double cs_i = sqrt(fabs(param->gamma * part[i].p / (part[i].rho + 1e-12)));
                double cs_j = sqrt(fabs(param->gamma * part[j].p / (part[j].rho + 1e-12)));
                double cs_medio = 0.5 * (cs_i + cs_j);

                // Viscosità artificiale
                double pi_ij = viscosity(part[i].rho, part[j].rho, part[i].vel, part[j].vel, part[i].pos, part[j].pos, h_medio, cs_medio);

                // Calcolo l'accelerazione
                // Per facilita' scrivo a parte il termine che contine pressioni e densita'
                double termine = (part[i].p / (part[i].rho * part[i].rho)) + (part[j].p / (part[j].rho * part[j].rho)) + pi_ij;
                
                // - (termine positivo) * (grad_W negativo) = POSITIVO.
                part[i].acc -= part[j].mass * termine * grad_W;
                
                // Calcolo l'energia interna
                double v_ij = part[i].vel - part[j].vel;

                part[i].du +=  0.5 * part[j].mass * termine * v_ij * grad_W;
            }
        }
    }
}

// Calcolo l'accelerazione usando gli stessi cicli che avevo usato per la densita' e la pressione secondo 
// il metodo cell-linked list.
void accel_ll(Particle *part, Parameter *param) {
    // Dopo aver ricostruito la lista linkata
    for (int i = 0; i < param->n_totale; i++) {
        part[i].acc = 0.0;
        part[i].du = 0.0;

        if (part[i].ghost == 1) continue;

        // Definisco le seguenti quantita' che chiamo dopo dentro il ciclo while
        double pos_i = part[i].pos;
        double vel_i = part[i].vel;
        double rho_i = part[i].rho;
        double p_i   = part[i].p;
        double h_i   = part[i].h;
        double cs_i = sqrt(param->gamma * p_i / rho_i);

        // Trovo l'indice della cella per la particella i-esima
        int k_i = (int)((part[i].pos + 0.5) / param->d_cell);
        
        // Ciclo sulle 3 celle vicine (k-1, k, k+1)
        for (int offset = -1; offset <= 1; offset++) {
            int k_nbr = k_i + offset;
            // Stessa condizione ai bordi usata in precedenza
            if (k_nbr < 0 || k_nbr >= param->n_cell) continue;
            // Prendo la prima particella nella cella
            Particle *j = param->head[k_nbr];

            // Finche' ci sono particelle nella cella...
            while (j != NULL) {
                
                double dist = pos_i - j->pos;
                double r = fabs(dist);
                double h_medio = 0.5 * (h_i + j->h);

                // Se sono abbastanza vicine calcolo il kernel
                if (r < 2.0 * h_medio && r > 1e-12) {
                    double dWi = kernel_dW(r, h_medio);

                    double grad_W = dWi * (dist / r); // Gradiente con segno

                    // Velocità del suono media per la viscosita'
                    double cs_j = sqrt(param->gamma * j->p / j->rho);
                    double cs_medio = 0.5 * (cs_i + cs_j);
                    double Pi_ij = viscosity(rho_i, j->rho, vel_i, j->vel, pos_i, j->pos, h_medio, cs_medio);

                    // Calcolo l'accelerazione
                    double termine = (p_i / (rho_i * rho_i)) + (j->p / (j->rho * j->rho)) + Pi_ij;
                    part[i].acc -= j->mass * termine * grad_W;

                    // Calcolo l'energia interna 
                    double v_ij = vel_i - j->vel;
                    part[i].du += 0.5 * j->mass * termine * v_ij * grad_W;
                }
                j = j->next;
            }
        }
    }
}





