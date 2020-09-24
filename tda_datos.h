#include <stdio.h>
#include <stdlib.h>

#ifndef TDA_DATOS_H
#define TDA_DATOS_H

#include "tda_nota.h"


//FUNCIONES DE TRAFERENCIAS DE DATOS PROCESADOS 
typedef struct datos_transfer datos_transfer_t;

datos_transfer_t *datos_crear_espacio(notas_guardadas_t *notas, uint16_t negra_por_segundo);
void datos_pasar(datos_transfer_t *datos,double **t0_datos,double **tf_datos,float **amp_datos,float **frq_datos,size_t *n_datos);
void datos_destruir(datos_transfer_t *datos);


#endif