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

   if (lectura_entrada(argc,argv,nombre_sint,nombre_midi,nombre_wave,&f_m,&pulsos_por_segundo,&canal) == false){
   	return 1;
   }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	                              EXTRACCION DE DATOS DEL MIDI
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

    // APERTURA DE ARCHIVO MIDI Y LECTURA DE DATOS
   	notas_guardadas_t *notas= lectra_notas(nombre_midi,canal);
   	if (notas == NULL){
   		return 1;
   	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                        TRASPASO DE DATOS (NOTA) PARA EL MUESTREO
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//SE CREA UNA ESTRUCTURA QUE ALMACENA VECTORES DINAMISCO CON LOS DATOS A PASAR AL SINTETIZADOR 
	datos_tranfer_t *trans_notas = datos_crear_espacio(notas, pulsos_por_segundo);
    notas_destruir(notas);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                      CREACION DE SINTETIZADOR
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

    //ESPACIO PARA GUARDAR DATOS LEIDOS DEL ARCHIVO
	tda_sintetizador_t *sintetizador = sintetizador_crear();
	if(sintetizador == NULL){
		datos_destruir(trans_notas);
		fprintf(stderr, "Error de memoria creando sintetizador.\n");
		return 1;
	}
	// LECTURA DE ARCHIVO SINTETIZADOR.TXT
    if(!leer_archivo_sintetizador(sintetizador, nombre_sint)){
		destruir_sintetizador(sintetizador);
		datos_destruir(trans_notas);
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
		destruir_sintetizador(sintetizador);
		datos_destruir(trans_notas);
		fprintf(stderr, "Error en proceso de muestreo.\n");
		return 1;
	}
	datos_destruir(trans_notas);
	destruir_sintetizador(sintetizador);
	size_t n_muestras;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                 ETAPA DE CONVERSION A DATOS WAV Y ESCRITURA DEL ARCHIVO DE SALIDA
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ESCALA DE NOTA (MAXIMO EN 32667)
    int16_t *vector_uint = tramo_a_int16(muestreo, &n_muestras);
	tramo_destruir(muestreo);

	//CREACION DEL ARCHIVO WAVE
	wave_t *wave =  wave_crear();
	if(wave == NULL){
		fprintf(stderr, "Error de memoria armando datos para el archivo wave.\n");
		return 1;
	}

	if(!wave_volcar_datos(wave, n_muestras, vector_uint, f_m)){
		fprintf(stderr, "Error volcando datos del wave.\n");
		return 1;
	}
	
	//APERTURA DEL ACHIVO DE SALIDA
	FILE *archivo_wave = fopen(nombre_wave, "wb");
	if(archivo_wave == NULL) {
        fprintf(stderr, "No se pudo crear el archivo wave: \"%s\"\n.", nombre_wave);
        return 1;
    }

	printf("Escribiendo archivo wave...\n");

	//ESCRITURA DEL ARCHIVO DE SALIDA
	if(!escribir_archivo_wave(wave, archivo_wave)){
		fprintf(stderr, "Error escribiendo datos en el archivo wave.\n");
		return 1;
	}

	printf("Sintesis completada!\n");
	wave_destruir(wave);  
	fclose(archivo_wave);

	return 0;
}