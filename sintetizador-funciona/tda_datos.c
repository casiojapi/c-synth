#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tda_nota.h"
#include "tda_datos.h"

//Estrectura de punteros que guarda los los datos de todas las notas pasadas por el TDA NOTAS
struct datos_tranfer{
	double *t0;		//TIEMPO INICIAL DE TODAS LAS NOTAS 
	double *tf;		//TIEMPO FINAL DE TODAS LAS NOTAS
	float *amp;		//AMPLITUD DE TODAS LAS NOTAS
	float *frq;		//FRCUENCI DE TODAS LAS NOTAS
	size_t n;		//CANTIDAD DE NOTAS TOTAL LEIDAS 
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//       FUNCIONES ENCARGADE DE CREAR VECTORES DE DATOS PARA ENVIARLE INFORMACION AL SINTETIZADOR.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

datos_tranfer_t* crear_espacio_para_datos_transferir(notas_guardadas_t *notas,uint16_t negra_por_segundo){
	datos_tranfer_t *datos = malloc(sizeof(datos_tranfer_t));
	if (datos == NULL){
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->t0 = malloc(sizeof(double) * lon_notas_guardadas(notas));
	if (datos->t0 == NULL){
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->tf = malloc(sizeof(double) * lon_notas_guardadas(notas));
	if (datos->tf == NULL){
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->amp = malloc(sizeof(double) * lon_notas_guardadas(notas));
	if (datos->amp == NULL){
		free(datos->tf);
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->frq = malloc(sizeof(double) * lon_notas_guardadas(notas));
	if (datos->frq == NULL){
		free(datos->amp);
		free(datos->tf);
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->n = lon_notas_guardadas(notas);

	for (size_t i = 0; i < datos->n; ++i){
		datos->t0[i] = tiempo_inicial_nota(notas,i,negra_por_segundo);
		datos->tf[i] = tiempo_final_nota(notas,i,negra_por_segundo);
		datos->amp[i] = amplitud_de_la_nota(notas,i);
		datos->frq[i] = frecuencia_pura(notas,i);
	}
	return datos;
}


void pasar_datos(datos_tranfer_t *datos, double **t0_datos, double **tf_datos,float **amp_datos, float **frq_datos, size_t *n_datos){
	*t0_datos = datos->t0;
	*tf_datos = datos->tf;
	*amp_datos = datos->amp;
	*frq_datos = datos->frq;
	*n_datos = datos->n;
}

void destruir_datos_guardadas(datos_tranfer_t *datos){
	if (datos != NULL){
		free(datos->frq);
		free(datos->amp);
		free(datos->tf);
		free(datos->t0);
		free(datos);
	}
}