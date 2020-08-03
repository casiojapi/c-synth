#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TDA_DATOS_H
#define TDA_DATOS_H

#include "codificacion_decodificacion.h"
#include "tda_nota.h"


typedef struct datos_tranfer datos_tranfer_t;

datos_tranfer_t *crear_espacio_para_datos_transferir(notas_guardadas_t *notas, uint16_t negra_por_segundo);
void pasar_datos(datos_tranfer_t *datos,double **t0_datos,double **tf_datos,float **amp_datos,float **frq_datos,size_t *n_datos);
void destruir_datos_guardadas(datos_tranfer_t *datos);

#endif