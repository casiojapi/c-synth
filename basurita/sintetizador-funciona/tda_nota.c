#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "codificacion_decodificacion.h"
#include "tda_nota.h"


// Estructura de vectores dinamicos , donde cada vector representa un n cantidad de datos leidos 

struct tda_nota{
    nota_t *tono;  //TONO DE UNA NOTA
    unsigned char *octava;	//OCTAVAS DE UNA NOTAS
    unsigned char *intensidad;   //PARAMETRO DE VELOCIDAD DEL FORMATO MIDI
    uint32_t *pulso_0;	// TIEMPOS INICIALES 
    uint32_t *pulso_f;	//TIRMPOS FINALES
};

// Estructura estatica utilizada para el guardado de notas leidas del MIDI hasta que se apagan 

typedef struct tda_nota_estatico{
    nota_t tono;
    unsigned char octava;
    unsigned char intensidad;
    uint32_t pulso_0;
    uint32_t pulso_f;
    bool prendido;	// INDICADOR DE NOTA APAGADA O ENCENDIDA
}notas_static;


//Estructura que guarda las notas prendidas y apagadas para realizar el tras paso
struct notas_guardadas{
	notas_static arreglo_notas_encendida[MAX_NOTAS];  //GUARDA LAS NOTAS PRENDIDAS
    tda_nota_t *vec_notas;	//GUARDA LAS NOTAS APAGADAS Y LISTAS PARA SINTETIZAR
    size_t lon;		//LONGITUD DE NOTAS APAGADAS GUARDADAS
    size_t pedido;		//MEMORIA PEDIDA PARA GUARDAR NOTAS
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        FUNCIONES PARA EL PEDIDO DE MEMORIA PARA GUARDAR NOTAS 
////////////////////////////////////////////////////////////////////////////////////////////////////////////

notas_guardadas_t *crear_espacio_para_notas(){

	notas_guardadas_t *notas=malloc(sizeof(notas_guardadas_t));
	if (notas==NULL){
		return NULL;
	}
	notas->vec_notas=malloc(sizeof(tda_nota_t));
	if (notas->vec_notas==NULL){
		free(notas);
		return NULL;
	}
	notas->vec_notas->tono=malloc(sizeof(nota_t)*CHOP);
	if (notas->vec_notas->tono==NULL){
		free(notas->vec_notas);
		free(notas);
		return NULL;

	}
	notas->vec_notas->octava=malloc(sizeof(unsigned char)*CHOP);
	if (notas->vec_notas->octava==NULL){
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return NULL;

	}
	notas->vec_notas->intensidad=malloc(sizeof(unsigned char)*CHOP);
	if (notas->vec_notas->intensidad==NULL){
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return NULL;

	}
	notas->vec_notas->pulso_0=malloc(sizeof(uint32_t)*CHOP);
	if (notas->vec_notas->pulso_0==NULL){
		free(notas->vec_notas->intensidad);
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return NULL;

	}
	notas->vec_notas->pulso_f=malloc(sizeof(uint32_t)*CHOP);
	if (notas->vec_notas->pulso_f==NULL){
		free(notas->vec_notas->pulso_0);
		free(notas->vec_notas->intensidad);
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return NULL;

	}
	notas->lon=0;
	notas->pedido=CHOP;

	for (size_t i = 0; i < MAX_NOTAS; ++i){
		notas->arreglo_notas_encendida[i].prendido=0;
	}

	return notas;
}

bool redimensionar_vec_notas(notas_guardadas_t *notas){

	nota_t *aux=realloc(notas->vec_notas->tono,sizeof(nota_t)*((notas->pedido)+CHOP));
	if (aux==NULL){
		free(notas->vec_notas);
		free(notas);
		return false;
	}
	notas->vec_notas->tono=aux;

	unsigned char *aux2=realloc(notas->vec_notas->octava,sizeof(unsigned char)*((notas->pedido)+CHOP));
	if (aux2==NULL){
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return false;
	}
	notas->vec_notas->octava=aux2;

	unsigned char *aux3=realloc(notas->vec_notas->intensidad,sizeof(unsigned char)*((notas->pedido)+CHOP));
	if (aux3==NULL){
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return false;
	}
	notas->vec_notas->intensidad=aux3;

	uint32_t *aux4=realloc(notas->vec_notas->pulso_0,sizeof(uint32_t)*((notas->pedido)+CHOP));
	if (aux4==NULL){
		free(notas->vec_notas->intensidad);
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return false;
	}
	notas->vec_notas->pulso_0=aux4;

	uint32_t *aux5=realloc(notas->vec_notas->pulso_f,sizeof(uint32_t)*((notas->pedido)+CHOP));
	if (aux5==NULL){
		free(notas->vec_notas->pulso_0);
		free(notas->vec_notas->intensidad);
		free(notas->vec_notas->octava);
		free(notas->vec_notas->tono);
		free(notas->vec_notas);
		free(notas);
		return false;
	}
	notas->vec_notas->pulso_f=aux5;

	notas->pedido+=CHOP;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         FUNCIONES PARA EL MANEJO DE NOTAS EXTRAIDAS DEL MIDI Y GUARDADAS EN EL VECTOR DINAMICO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool guardar_nota_encendida(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim){
	

	if (inte==0){
			guardar_nota_apagada(notas,n,oct,inte,tim);
			return true;
	}
	for (int i = 0; i < MAX_NOTAS; ++i){

		if (notas->arreglo_notas_encendida[i].prendido == 0){

			notas->arreglo_notas_encendida[i].tono = n;
			notas->arreglo_notas_encendida[i].octava = oct;
			notas->arreglo_notas_encendida[i].intensidad = inte;
			notas->arreglo_notas_encendida[i].pulso_0 = tim;
			notas->arreglo_notas_encendida[i].pulso_f = 0;
			notas->arreglo_notas_encendida[i].prendido = 1;


			return true;
		}
	}
	return false;
}

bool guardar_nota_apagada(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim){
	
	if (notas->lon==notas->pedido-1){
		
		if (!redimensionar_vec_notas(notas)){
			printf("apagadas primer false\n");
			return false;
		}
	}
	
	for (int i=0; i < MAX_NOTAS; ++i){

		if ((notas->arreglo_notas_encendida[i].octava == oct) && (notas->arreglo_notas_encendida[i].tono == n) && (notas->arreglo_notas_encendida[i].prendido==1)){
			notas->vec_notas->tono[notas->lon]=n;
			notas->vec_notas->octava[notas->lon]=oct;
			notas->vec_notas->intensidad[notas->lon]=notas->arreglo_notas_encendida[i].intensidad;
			notas->vec_notas->pulso_0[notas->lon]=notas->arreglo_notas_encendida[i].pulso_0;
			notas->vec_notas->pulso_f[notas->lon]=tim;
			notas->arreglo_notas_encendida[i].prendido = 0;

			notas->lon = notas->lon + 1;

			return true;
		}
	}
	printf("apagadas segundo false\n");
	return false;
}

size_t lon_notas_guardadas(notas_guardadas_t *notas){
	return notas->lon;
}

void destruir_notas_guardadas(notas_guardadas_t *notas){
	
	free(notas->vec_notas->pulso_f);
	free(notas->vec_notas->pulso_0);
	free(notas->vec_notas->intensidad);
	free(notas->vec_notas->octava);
	free(notas->vec_notas->tono);
	free(notas->vec_notas);
	free(notas);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//       FUNCIONES PARA EL MANEJO DEL TDA NOTAS PARA PASAR DEL LENGUJE MIDI A PARAMETROS DEL SINTETIZADOR
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float frecuencia_pura(notas_guardadas_t *notas,size_t n){

	float tono=notas->vec_notas->tono[n];
    float octava=notas->vec_notas->octava[n];

	return FRECUENCI_LA4*(pow((float)2,(octava-4)+((float)(tono-LA)/12)));
}

float amplitud_de_la_nota(notas_guardadas_t *notas,size_t n){

	return (float)notas->vec_notas->intensidad[n];
}

double tiempo_inicial_nota(notas_guardadas_t *notas,size_t n, uint16_t pulso_por_segundo){

	return (double)notas->vec_notas->pulso_0[n] / (double)pulso_por_segundo ;
}

double tiempo_final_nota(notas_guardadas_t *notas,size_t n,uint16_t pulso_por_segundo){
	
	return (double)notas->vec_notas->pulso_f[n] / (double)pulso_por_segundo ;
}