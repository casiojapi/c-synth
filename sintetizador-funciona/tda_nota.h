#include <stdint.h>
#include <stdbool.h>

#ifndef TDA_NOTA_H
#define TDA_NOTA_H

#include "codificacion_decodificacion.h"

#define MAX_NOTAS 10
#define CHOP 10000
#define FRECUENCI_LA4 440


typedef struct tda_nota tda_nota_t; // TDA NOTAS , GUARDAS LOS PARAMETROS DE UNA NOTA LEIDA EN FORMATO MIDI
typedef struct notas_guardadas notas_guardadas_t;	// MANEJAS LAS NOTAS ALMACENADAS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EL TDA NOTAS SE ENCARGA DE MANEJAR LA INFORMACION OBTENIDA DEL ARCHIVO MIDI Y TRABAJA CON ESTOS DATOS PARA 
// PASAR LA INFORMACION EN PARAMETROS NECESARIOS PARA LA SINTETIZACION 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

notas_guardadas_t *crear_espacio_para_notas();
bool guardar_nota_encendida(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim);
bool guardar_nota_apagada(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim);
bool redimensionar_vec_notas(notas_guardadas_t *notas);
void destruir_notas_guardadas(notas_guardadas_t *notas);
size_t lon_notas_guardadas(notas_guardadas_t *notas);
void pulsos_negra_pasar(notas_guardadas_t *notas,uint16_t pulsos_negra);
float frecuencia_pura(notas_guardadas_t *notas,size_t n);
float amplitud_de_la_nota(notas_guardadas_t *notas,size_t n);
double tiempo_inicial_nota(notas_guardadas_t *notas,size_t n, uint16_t pulsos_por_segundo);
double tiempo_final_nota(notas_guardadas_t *notas,size_t n,uint16_t pulsos_por_segundo);

#endif