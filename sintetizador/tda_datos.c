#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tda_nota.h"
#include "tda_datos.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EL TDA DATOS TIENE COMO FUNCION PRINCIPAL TOMAR LOS VALORES OBTENIDOS EN LECTURA MIDI Y TRANFORMALOS EN 
// DATOS DE TIPO NECESARIO PARA LOS PARAMETROS DE TDA TRAMO 
// ESTOS DATOS SON AGRUPADOS EN VECTORES DINAMICOS PARA UN MEJOR ACCESO ITERATIVO
////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

datos_tranfer_t* datos_crear_espacio(notas_guardadas_t *notas,uint16_t negra_por_segundo){
	datos_tranfer_t *datos = malloc(sizeof(datos_tranfer_t));
	if (datos == NULL){
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->t0 = malloc(sizeof(double) * nota_guardadas_longitud(notas));
	if (datos->t0 == NULL){
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->tf = malloc(sizeof(double) * nota_guardadas_longitud(notas));
	if (datos->tf == NULL){
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->amp = malloc(sizeof(double) * nota_guardadas_longitud(notas));
	if (datos->amp == NULL){
		free(datos->tf);
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->frq = malloc(sizeof(double) * nota_guardadas_longitud(notas));
	if (datos->frq == NULL){
		free(datos->amp);
		free(datos->tf);
		free(datos->t0);
		free(datos);
		fprintf(stderr, "No se pudo transferir datos \n");
		return NULL;
	}

	datos->n = nota_guardadas_longitud(notas);

	for (size_t i = 0; i < datos->n; ++i){
		datos->t0[i] = nota_tiempo_inicial(notas,i,negra_por_segundo);
		datos->tf[i] = nota_tiempo_final(notas,i,negra_por_segundo);
		datos->amp[i] = nota_amplitud(notas,i);
		datos->frq[i] = nota_frecuencia_pura(notas,i);
	}
	return datos;
}

// FUNCION ENCARGADA DE DEVOLVER LOS VECTORES DINAMOCOS CON LOS DATOS 
void datos_pasar(datos_tranfer_t *datos, double **t0_datos, double **tf_datos,float **amp_datos, float **frq_datos, size_t *n_datos){
	*t0_datos = datos->t0;
	*tf_datos = datos->tf;
	*amp_datos = datos->amp;
	*frq_datos = datos->frq;
	*n_datos = datos->n;
}

void datos_destruir(datos_tranfer_t *datos){
	if (datos != NULL){
		free(datos->frq);
		free(datos->amp);
		free(datos->tf);
		free(datos->t0);
		free(datos);
	}
}