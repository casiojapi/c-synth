#include <stdio.h>
#include <stdint.h>

#ifndef TDA_TRAMO_H
#define TDA_TRAMO_H




typedef struct tda_tramo tda_tramo_t;

tda_tramo_t *tramo_crear(double t0, double tf, int f_m, double td);

void tramo_destruir(tda_tramo_t *t);

//internas
void muestrear_senoidal(float *v, size_t n, double t0, int f_m, float f, float a);
void inicializar_muestras(float *v, size_t n);
void muestrear_armonicos(float *v, size_t n, double t0, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_fa);




void muestrear_modulacion(tda_tramo_t *tramo, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, int f_m, float *para_ataque, float *para_sostenido, float *para_decaimiento, double t_a, double t_d);
//internas (static)


tda_tramo_t *tramo_clonar(const tda_tramo_t *t);
tda_tramo_t *tramo_crear_muestreo(double t0, double tf, double td, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_fa);
bool tramo_redimensionar(tda_tramo_t *t, double tf, int f_m);
bool tramo_extender(tda_tramo_t *destino, const tda_tramo_t *extension, int f_m);

tda_tramo_t *muestreo_completo(double *t0, double *tf, double td, int f_m, float *fre, float *amp, float *a_arm, float *f_arm, size_t n_arm, size_t n_notas, float *para_ataque, float *para_sostenido, float *para_decaimiento, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, double t_a);
int16_t *tramo_a_int16(tda_tramo_t *tramo, size_t *n_muestras);

#endif