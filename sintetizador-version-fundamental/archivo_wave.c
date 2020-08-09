#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#include "archivo_wave.h"

//Struct de TDA wave. Se encargar de guardar la informacion necesaria para escribir un archivo wave, y a la vez sea un proceso mas sencillo. 
struct wave{
    char chunk_id[WAVE_4];          //"RIFF"
    uint32_t chunk_size ;           //36 + 2n
    char format[WAVE_4];            //"WAVE"
    char sub_chunk_1_id[WAVE_4];    //"fmt "
    uint32_t sub_chunk_1_size;      //16
    uint16_t audio_format;          //1
    uint16_t num_channels;          //1
    uint32_t sample_rate;           //f_m
    uint32_t byte_rate;             //2 * f_m
    uint16_t block_allign;          //2
    uint16_t bits_per_sample;       //16
    char sub_chunk_2_id[WAVE_4];    //"data"
    uint32_t sub_chunk_2_size;      // 2n
    int16_t *data;                  //secuencia de n muestreos
};

//Funcion que rea una estructura wave, para luego volcarla en archivo wave
wave_t *wave_crear(){
    wave_t *nuevo;
    if((nuevo = malloc(sizeof(wave_t))) == NULL){
        return NULL;
    }
    return nuevo;
}

//Funcion que se encarga de liberar la memoria utilizada en un struct wave
void wave_destruir(wave_t *wave){
    if(wave == NULL)
        return;
    if(wave->data != NULL)
        free(wave->data);
    free(wave);
}

//Funcion que se encarga de volcar los datos desde afuera hacia el struct wave_t
bool wave_volcar_datos(wave_t *wave, size_t n_muestras, int16_t *muestras, int f_m){
    if((wave == NULL) || (muestras == NULL) || (n_muestras == 0)){
        return false;
    }
    strcpy(wave->chunk_id, WAVE_CHUNK_ID);
    wave->chunk_size = 36 + (2*n_muestras);
    strcpy(wave->format, WAVE_FORMAT);
    strcpy(wave->sub_chunk_1_id, WAVE_SUBCH_1);
    wave->sub_chunk_1_size = 16;
    wave->audio_format = 1;
    wave->num_channels = 1;
    wave->sample_rate = f_m;
    wave->byte_rate = 2 * f_m;
    wave->block_allign = 2;
    wave->bits_per_sample = 16;
    strcpy(wave->sub_chunk_2_id, WAVE_SUBCH_2);
    wave->sub_chunk_2_size = 2 * n_muestras;
    wave->data = muestras;    
    return true;
}

//Algunas funciones que usamos para escribir el archivo wave.
static void escribir_little_endian_8(uint8_t dato, FILE *archivo){
    fwrite(&dato,sizeof(char),1,archivo);
}

static void escribir_little_endian_16(uint16_t dato, FILE *archivo){

    for (int i = 0; i < 2; ++i){
        uint8_t valor = (dato>>(8*i))&MASK_VALOR_A_ESCRIBIR;
        escribir_little_endian_8(valor,archivo);
    }
}

static void escribir_little_endian_32(uint32_t dato, FILE *archivo){   
    for (int i = 0; i < 4; ++i){
        uint8_t valor = (dato>>(8*i))&MASK_VALOR_A_ESCRIBIR;
        escribir_little_endian_8(valor,archivo);
    }
}


//escribir archivo waves
//Funcion que se encarga de escribir todo el archivo wave, sacando la informacion de un wave_t lleno. 
//Precondicion: debe recibir un archivo valido en modo "rb" ".wav". Y tambien debe recibir una estructura wave valida (NO PUEDE APUNTAR A NULL)
bool wave_escribir_archivo(wave_t *wave, FILE *archivo_wave){
    if((archivo_wave == NULL) || (wave == NULL)){
        return false;
    }
    fwrite(&(wave->chunk_id), sizeof(char), WAVE_4, archivo_wave);
    escribir_little_endian_32(wave->chunk_size, archivo_wave);
    fwrite(&(wave->format), sizeof(char), WAVE_4, archivo_wave);
    fwrite(&(wave->sub_chunk_1_id), sizeof(char), WAVE_4, archivo_wave);
    escribir_little_endian_32(wave->sub_chunk_1_size, archivo_wave);
    escribir_little_endian_16(wave->audio_format, archivo_wave);
    escribir_little_endian_16(wave->num_channels, archivo_wave);
    escribir_little_endian_32(wave->sample_rate, archivo_wave);
    escribir_little_endian_32(wave->byte_rate, archivo_wave);
    escribir_little_endian_16(wave->block_allign, archivo_wave);
    escribir_little_endian_16(wave->bits_per_sample, archivo_wave);
    fwrite(&(wave->sub_chunk_2_id), sizeof(char), 4, archivo_wave);
    escribir_little_endian_32(wave->sub_chunk_2_size, archivo_wave);

    size_t n = (wave->sub_chunk_2_size) / 2;
    for(size_t i = 0; i < n; i++){
        escribir_little_endian_16(wave->data[i], archivo_wave);
    }
    return true;
}


bool wave_escribir_completo(FILE *archivo_wave, int16_t *vector_int16, size_t n_muestras, int f_m){
    wave_t *wave = wave_crear();
    if(wave == NULL) 
        return false;
    if(!wave_volcar_datos(wave, n_muestras, vector_int16, f_m)){
        wave_destruir(wave);
		return false;
    }
	if(!wave_escribir_archivo(wave, archivo_wave)){
        wave_destruir(wave);
		return false;
    }
    wave_destruir(wave);
	return true;
}
