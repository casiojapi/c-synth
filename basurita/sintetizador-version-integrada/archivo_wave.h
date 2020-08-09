#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef ARCHIVO_WAVE_H
#define ARCHIVO_WAVE_H

#define WAVE_4 4
#define WAVE_CHUNK_ID "RIFF"
#define WAVE_FORMAT "WAVE"
#define WAVE_SUBCH_1 "fmt "
#define WAVE_SUBCH_2 "data"
#define MASK_VALOR_A_ESCRIBIR 0xff




typedef struct wave wave_t;

//Funcion que rea una estructura wave, para luego volcarla en archivo wave
wave_t *wave_crear();

//Funcion que se encarga de liberar la memoria utilizada en un struct wave
void wave_destruir(wave_t *);

//Funcion que se encarga de volcar los datos desde afuera hacia el struct wave_t
bool wave_volcar_datos(wave_t *wave, size_t n_muestras, int16_t *muestras, int f_m);

//Funcion que se encarga de escribir todo el archivo wave, sacando la informacion de un wave_t lleno. 
//Precondicion: debe recibir un archivo valido en modo "rb" ".wav". Y tambien debe recibir una estructura wave valida (NO PUEDE APUNTAR A NULL)
bool wave_escribir_archivo(wave_t *wave, FILE *archivo_wave);


bool wave_escribir_completo(FILE *archivo_wave, int16_t *vector_int16, size_t n_muestras, int f_m);
#endif