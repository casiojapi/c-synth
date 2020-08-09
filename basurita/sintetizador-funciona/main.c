#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "tda_sintetizador.h"
#include "tda_tramo.h"
#include "tda_nota.h"
#include "archivo_wave.h"
#include "tda_datos.h"
#include "lectura_midi.h"
#include "sintesis_de_datos.h"

#define MAX_ARGUMENTOS 10
#define MAX_NOMBRE 256
#define DEF_FRECUENCIA_MUESTREO 10250
#define DEF_PULSOS_POR_SEGUNDO 250
#define N_NOTAS 20

int main(int argc, char const *argv[]){

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                  LECTURA PARAMETROS DE ENTRADA
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	char nombre_sint[MAX_NOMBRE];
	char nombre_midi[MAX_NOMBRE];
	char nombre_wave[MAX_NOMBRE];
	uint16_t f_m = DEF_FRECUENCIA_MUESTREO;
    uint16_t pulsos_por_segundo = DEF_PULSOS_POR_SEGUNDO;
    char canal = 0;

    //USO DEL PROGRAMA
	if(argc < 7){
		fprintf(stderr, "Error, faltan comandos...\nUso: $ ./sintetizador -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav> [-c <canal>] [-f <frecuencia>] [-r <pulsosporsegundo>]\n");
		return 1;
	}
	for (int i = 1; i < argc; ++i){
		if (!strcmp(argv[i],"-s")){
			i++;
			strcpy(nombre_sint, argv[i]);
			
		}
		if (!strcmp(argv[i],"-i")){
			i++;
			strcpy(nombre_midi, argv[i]);
		}
		if (!strcmp(argv[i],"-o")){
			i++;
			strcpy(nombre_wave, argv[i]);
		}
		if (!strcmp(argv[i],"-r")){
			i++;
			pulsos_por_segundo = atoi(argv[i]);
		}
		if (!strcmp(argv[i],"-c")){
			i++;
			canal = atoi(argv[i]);
		}
		if (!strcmp(argv[i],"-f")){
			i++;
			f_m = atoi(argv[i]);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	                              EXTRACCION DE DATOS DEL MIDI
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// APERTURA DE ARCHIVO MIDI:
    FILE *archivo_midi = fopen(nombre_midi, "rb");
    if(archivo_midi == NULL) {
        fprintf(stderr, "Error, no se pudo abrir el archivo midi: \"%s\".\n", nombre_midi);
        return 1;
    }
    //LECTURA DEL ARCHIVO MIDI
    notas_guardadas_t *notas = crear_espacio_para_notas();
    if (!leer_midi(archivo_midi, notas, canal)){
		fprintf(stderr, "Error, No se pudo leer el archivo midi: \"%s\".\n", nombre_midi);
		destruir_notas_guardadas(notas);
 		fclose(archivo_midi);
    	return 1;
    }
	fclose(archivo_midi);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                        TRASPASO DE DATOS (NOTA) PARA EL MUESTREO
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//SE CREA UNA ESTRUCTURA QUE ALMACENA VECTORES DINAMISCO CON LOS DATOS A PASAR AL SINTETIZADOR 
	datos_tranfer_t *trans_notas = crear_espacio_para_datos_transferir(notas, pulsos_por_segundo);
    destruir_notas_guardadas(notas);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                      CREACION DE SINTETIZADOR
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// LECTURA DE ARCHIVO SINTETIZADOR.TXT
    tda_sintetizador_t *sintetizador = sintetizador_crear_leer(nombre_sint);
	if(sintetizador == NULL){
		destruir_datos_guardadas(trans_notas);
		fprintf(stderr, "Error leyendo archivo de sintetizador.\n");
		return 1;
	}
	printf("Realizando muestreo...\n");

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                 ARMADO DEL MUESTREO Y COMIENZO DEL PROCESO DE SINTETIZACION 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	//REALIZANDO MUESTREOS
	tda_tramo_t *muestreo = sintesis_completa(sintetizador, trans_notas, f_m);
	if(muestreo == NULL){
		sintetizador_destruir(sintetizador);
		destruir_datos_guardadas(trans_notas);
		fprintf(stderr, "Error en proceso de muestreo.\n");
		return 1;
	}
	destruir_datos_guardadas(trans_notas);
	sintetizador_destruir(sintetizador);
	size_t n_muestras;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                 ETAPA DE CONVERSION A DATOS WAV Y ESCRITURA DEL ARCHIVO DE SALIDA
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ESCALA DE NOTA (MAXIMO EN 32667)
    int16_t *vector_int16 = tramo_a_int16(muestreo, &n_muestras);
	tramo_destruir(muestreo);
	
	//APERTURA DEL ACHIVO DE SALIDA
	FILE *archivo_wave = fopen(nombre_wave, "wb");
	if(archivo_wave == NULL) {
        fprintf(stderr, "No se pudo crear el archivo wave: \"%s\"\n.", nombre_wave);
        return 1;
    }
	printf("Escribiendo archivo wave...\n");

	//ESCRITURA DEL ARCHIVO DE SALIDA
	if(!wave_escribir_completo(archivo_wave, vector_int16, n_muestras, f_m)){
		fprintf(stderr, "Error escribiendo datos en el archivo wave.\n");
		return 1;
	}
	printf("Sintesis completada!\n");
	fclose(archivo_wave);
	return 0;
}