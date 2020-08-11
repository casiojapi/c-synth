#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef TDA_TRAMO_H
#define TDA_TRAMO_H

#define MAX_INT16T 32767


//Declaracion de typedef para tener mas facil acceso al TDA tramo.
typedef struct tda_tramo tda_tramo_t;

//Funcion que se encarga de crear un TDA tramo, recibiendo los siguientes parametros.
//Precondicion: que sean datos validos leidos desde el archivo midi.
tda_tramo_t *tramo_crear(double t0, double tf, int f_m, double td);

//Funcion que libera toda la memoria alloqueada en un tramo.
//Precondicion: el tramo debe tener un vector que no apunte a NULL, y el puntero al tramo tampoco debe apuntar a NULL 
void tramo_destruir(tda_tramo_t *t);

//Funcion que se encarga de hacer un muestreo de una frecuencia y amplitud puntual en un vector. 
//Precondicion: el puntero que recibe no debe apuntar a NULL, y debe ser un vector de muestreos valido.
void muestrear_senoidal(float *v, size_t n, double t0, int f_m, float f, float a);

//Funcion simple que unicamente inicializa las muestras de un vector a 0.
//Precondicion: el puntero que recibe no debe apuntar a NULL, y debe ser un vector de muestreos valido.
void inicializar_muestras(float *v, size_t n);

//Funcion que unicamente muestrea armonicos recibiendo un vector ya existente. Se usa luego en "tramo_crear_muestreo".
//Precondicion: los punteros que recibe no deben estar apuntando a NULL, y debe ser un vector de muestreos valido.
void tramo_muestrear_armonicos(float *v, size_t n, double t0, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_fa);

//Funcion que recibe los siguientes parametros y a partir de estos crea un TDA tramo, lo inicializa, muestrea su frecuencia fundamental, y luego todos sus armonicos. 
//Precondicion: Los punteros que recibe a vectores de armonicos no deben apuntar a NULL.
tda_tramo_t *tramo_crear_muestreo(double t0, double tf, double td, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_fa);

//Funcion que recibe un tramo ya existente y lo extiende teniendo en cuenta nuevo parametro tf.
//Precondicion: El tramo que recibe, no debe apuntar a NULL.
bool tramo_redimensionar(tda_tramo_t *t, double tf, int f_m);

//Funcion que recibe un tramo destino, y otro a agregarse al anterior. Usamos esta funcion para ir sumando el tramo de cada nota, y terminar con un tramo contenedor de todas las notas muestreadas. 
//Precondicion: los tramos no deben apuntar a NULL, y el t0 del tramo a agregarse debe ser mayor al t0 del tramo destino.
bool tramo_extender(tda_tramo_t *destino, const tda_tramo_t *extension, int f_m);

// Funcion que recibe un tramo y las funciones y parametros leidos del archivo de texto "sintetizador.txt"
// Precondicion: que no reciba el tramo apuntando a NULL. 
void tramo_muestrear_modulacion(tda_tramo_t *tramo, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, int f_m, float *para_ataque, float *para_sostenido, float *para_decaimiento, double t_a, double t_d);

//Funcion que recibe una serie parametros para hacer mas sencilla la sintesis y muestreo completo desde el main. 
//Precondicion: NO puede recibir punteros a NULL, 
tda_tramo_t *tramo_muestreo_completo(double *t0, double *tf, double td, int f_m, float *fre, float *amp, float *a_arm, float *f_arm, size_t n_arm, size_t n_notas, float *para_ataque, float *para_sostenido, float *para_decaimiento, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, double t_a);

//Funcion que pasa el vector desde adentro de un tramo a un vector de int16_t y lo escala para dejar el maximo en 32667. Devuelve el numero de muestras por la interfaz (n_muestras)
//Precondicion: El puntero al tramo no debe apuntar a NULL.
int16_t *tramo_a_int16(tda_tramo_t *tramo, size_t *n_muestras);

#endif