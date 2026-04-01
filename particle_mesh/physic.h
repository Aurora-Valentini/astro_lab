#ifndef PHYSIC_H
#define PHYSIC_H

#include <math.h>

#ifndef PI
    #define PI 3.14159265358979323846
#endif

// =====================
// STRUCT PARTICELLA
// =====================

typedef struct {
    double x;
    double v;
    double ax;
} Particle;

// =====================
// STRUCT PARAMETRI
// =====================
typedef struct {
    double unit_v;
    double unit_m;
    double unit_l;
    double unit_time;
    double G_cgs;
    double G_norm;
    double dx;
    double inv_dx;
    double box_length;
    double a_delta;
    double dt;
    int n_points;
    int n_grid;
    int n_step;
    int n_skip;
    
} Parametri;

// =====================
// PROTOTIPI DELLE FUNZIONI
// =====================

void leggi_param(const char *filename, Parametri *param);

void calcola_param(Parametri *param);

void scrivi_ic(Particle *part, int n_points, char *filename);

void conversion(const char *file_bin, const char *file_txt);

void sinusoidal_ic(Particle *part, Parametri *param);

void histogram(Particle *part, Parametri *p, int n_bins);

void assign_density(Particle *part, double *rho, Parametri *p);

void save_density(double *rho, Parametri *p, const char *filename);

void compute_potential(double *rho, double *pot, Parametri *p);

void comparison(double *pot_fft, Parametri *p, const char *filename);

void compute_acc(double *pot, double *accel, Parametri *p);

void assign_acc(Particle *parts, double *grid_acc, Parametri *p);

void kick(Particle *parts, Parametri *p);

void drift(Particle *parts, Parametri *p);

void save_phase_space(Particle *parts, Parametri *p, const char *filename);

#endif
