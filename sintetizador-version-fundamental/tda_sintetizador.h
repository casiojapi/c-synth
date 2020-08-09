#include <stdio.h>
#include <stdbool.h>

#ifndef TDA_SINTETIZADOR_H
#define TDA_SINTETIZADOR_H

#define MAX_PARAMETROS 3
#define MAX_STR 256
#define MOD_MAX 3
#define MOD_TOTAL 14
#define ESPACIO_DELIM " "
#define PI 3.141592653589793238462643383279
#define MAX_ARMONICOS 15


//Declaracion de un typedef para que sea mas sencillo el acceso al tipo puntero a funcion de modulacion
typedef float (*f_modulacion_t)(const float *, double);

//Declaracion de los siguientes typedefs para que sea mas sencillo el acceso a los siguientes structs. 
typedef struct armonicos armonicos_t;
typedef struct modulacion modulacion_t;
typedef struct tda_sintetizador tda_sintetizador_t;

//Declaracion de un struct para que sea mas facil el paso de la cadena en "sintetizador.txt" a la funcion de modulacion elegida. 
struct dicc_modulacion{
    char *modulacion_nombre;
    f_modulacion_t funcion;
};
typedef struct dicc_modulacion dicc_modulacion_t;

//funciones del sintetizador
tda_sintetizador_t *sintetizador_crear();

void sintetizador_destruir(tda_sintetizador_t *sint);
void sintetizador_get_armonicos(tda_sintetizador_t *sint, float **arm, float **fre, size_t *n);
void sintetizador_get_modulacion(tda_sintetizador_t *sint,f_modulacion_t *ataque,f_modulacion_t *sotenido,f_modulacion_t *decaimiento);
double sintetizador_get_td(tda_sintetizador_t *sint);
double sintetizador_get_ta(tda_sintetizador_t *sint);


float *sintetizador_parametros_ataque(tda_sintetizador_t *sint);
float *sintetizador_parametros_sostenido(tda_sintetizador_t *sint);
float *sintetizador_parametros_decaimiento(tda_sintetizador_t *sint);

//Declaracion de la funcion encargada de leer y sustraer los datos del archivo "sintetizador.txt". Recibe el nombre del archivo, y devuelve por la interfaz un sintetizador con los datos sustraidos del archivo de texto. Devuelve true si se ejecuto correctamente, false si hay algun error con el archivo.
bool sintetizador_leer_archivo(tda_sintetizador_t *sint, char *nombre_archivo);

//funcion para simplificar un poco mas el main. Se encarga de crear un sintetizador, leer el archivo "sintetizador.txt" y guardar esa informacion en el sintetizador previamente creado.
//Pre: se debe recibir una cadena valida como nombre del archivo de sintetizador.txt
tda_sintetizador_t *sintetizador_crear_leer(char *sinte_nombre_archivo);

//Declaracion de todas las posibles funciones de modulacion para el sintetizador
float mod_constant(const float *, double);
float mod_linear(const float *, double);
float mod_invlinear(const float *, double);
float mod_sin(const float *, double);
float mod_exp(const float *, double);
float mod_invexp(const float *, double);
float mod_quartcos(const float *, double);
float mod_quartsin(const float *, double);
float mod_halfcos(const float *, double);
float mod_halfsin(const float *, double);
float mod_log(const float *, double);
float mod_invlog(const float *, double);
float mod_tri(const float *, double);
float mod_pulses(const float *, double);

#endif
