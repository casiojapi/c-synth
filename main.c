#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "tda_sintetizador.h"
#include "tda_tramo.h"
#include "tda_nota.h"
#include "archivo_wave.h"
#include "tda_datos.h"
#include "lectura_midi.h"
#include "sintesis_de_datos.h"
#include "entrada.h"

#define MAX_ARGUMENTOS 10
#define MAX_NOMBRE 256
#define DEF_FRECUENCIA_MUESTREO 44100
#define DEF_PULSOS_POR_SEGUNDO 250
#define N_NOTAS 20

int main(int argc, char const *argv[]){

	char nombre_sint[MAX_NOMBRE];
	char nombre_midi[MAX_NOMBRE];
	char nombre_wave[MAX_NOMBRE];
	uint16_t f_m = DEF_FRECUENCIA_MUESTREO;
    uint16_t pulsos_por_segundo = DEF_PULSOS_POR_SEGUNDO;
    char canal = 0;

	//LECTURA DE ARGUMENTOS DE LINEA DE COMANDO
    if(!lectura_entrada(argc, argv, nombre_sint, nombre_midi, nombre_wave, &f_m, &pulsos_por_segundo, &canal)){
		fprintf(stderr, "error - failed reading arguments.\nUse: $ ./synth -s <patches/patch.txt> -i <midiscore.mid> -o <out.wav> [OPTIONAL] [-c <channel>] [-f <sample_rate>] [-r <pulses_per_second>]\n");
   		return 1;
    }

    //APERTURA DE ARCHIVO MIDI Y LECTURA DE DATOS
   	notas_guardadas_t *notas = lectura_notas(nombre_midi,canal);
   	if(notas == NULL){
		fprintf(stderr, "error - failed reading midi file.\n");
   		return 1;
   	}

	//SE CREA UNA ESTRUCTURA QUE ALMACENA VECTORES DINAMICOS CON LOS DATOS A PASAR AL SINTETIZADOR 
	datos_transfer_t *trans_notas = datos_crear_espacio(notas, pulsos_por_segundo);
    notas_destruir(notas);

	// LECTURA DE ARCHIVO SINTETIZADOR.TXT
    tda_sintetizador_t *sintetizador = sintetizador_crear_leer(nombre_sint);
	if(sintetizador == NULL){
		datos_destruir(trans_notas);
		fprintf(stderr, "error - failed reading patch file.\n");
		return 1;
	}

	//REALIZANDO MUESTREOS
	printf("sampling phase in progress...\n");
	tda_tramo_t *muestreo = sintesis_completa(sintetizador, trans_notas, f_m);
	if(muestreo == NULL){
		sintetizador_destruir(sintetizador);
		datos_destruir(trans_notas);
		fprintf(stderr, "error - failed on sampling phase.\n");
		return 1;
	}
	datos_destruir(trans_notas);
	sintetizador_destruir(sintetizador);

	//ESCALA DE NOTA (MAXIMO EN 32667)
	size_t n_muestras;
    int16_t *vector_int16 = tramo_a_int16(muestreo, &n_muestras);
	tramo_destruir(muestreo);
	
	//APERTURA DEL ACHIVO DE SALIDA
	FILE *archivo_wave = fopen(nombre_wave, "wb");
	if(archivo_wave == NULL) {
        fprintf(stderr, "error - failed creating wave file: \"%s\"\n.", nombre_wave);
        return 1;
    }
	printf("writing wave file...\n");

	//ESCRITURA DEL ARCHIVO DE SALIDA
	if(!wave_escribir_completo(archivo_wave, vector_int16, n_muestras, f_m)){
		fprintf(stderr, "error - failed writing wave file.\n");
		return 1;
	}
	printf("finished succesfully!\n");
	fclose(archivo_wave);
	return 0;
}