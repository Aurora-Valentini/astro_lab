#ifndef PHYSIC_H
#define PHYSIC_H

// =====================
// STRUCT PARTICELLA
// =====================

typedef struct Particle{
    // Quantita' dinamiche 
    double pos;
    double vel;
    double acc;
    double mass;

    // Quantita' idrodinamiche 
    double rho;
    double p;
    double u;
    double du;
    double h;
    struct Particle *next;

    // Flag per generare particelle fantasma ai bordi
    int ghost; // 0 = reale, 1 = ghost
}Particle;

// =====================
// STRUCT PARAMETRI
// =====================

typedef struct {
    // Parametri per le IC
    int N;
    double L;
    double gamma;

    double p_left;
    double rho_left;
    double v_left;

    double p_right;
    double rho_right;
    double v_right;

    int N_LZ;
    int N_RZ;

    double M_LZ;
    double M_RZ;
    double m_part;

    double dx_LZ;
    double dx_RZ;

    // Parametri per costruire la griglia
    Particle **head;  // Puntatore a un array di puntatori
    int n_cell;
    double d_cell;
    double h_max;
    double t_max;

    int n_ghost;
    int n_totale;

} Parameter;

// Dichiarazione delle funzioni

// In IC_sph.c:
void leggi_param(const char *filename, Parameter *param);

void calcola_param(Parameter *param);

void init_cond(Particle *part, Parameter *param);

void salva_evol(Particle *part, Parameter *param, const char *filename);

// In ricerca_vicini.c:
int compare_part(const void *a, const void *b);

void update_h(Particle *part, Parameter *param);

void linked_list(Particle *part, Parameter *param);

// In density_forces.c:
double kernel_W(double r, double h);

double kernel_dW(double r, double h);

void density_press_qs(Particle *part, Parameter *param);

void density_press_ll(Particle *part, Parameter *param);

double viscosity(double rho_i, double rho_j, double v_i, double v_j, double x_i, double x_j, double h_ij, double c_ij);

void accel_qs(Particle *part, Parameter *param);

void accel_ll(Particle *part, Parameter *param);

// In evolution.c:

double timestep_comp(Particle *part, Parameter *param);

void kick(Particle *part, Parameter *param, double dt_step);

void drift(Particle *part, Parameter *param, double dt_step);

void leapfrog(Particle *part, Parameter *param, double dt, int metodo);


#endif


