#include <stdint.h>
#include <stdbool.h>

#ifndef TDA_NOTA_H
#define TDA_NOTA_H

#define MAX_NOTAS 10
#define CHOP 10000
#define FRECUENCI_LA4 440

#include "codificacion_decodificacion.h"

typedef struct tda_nota tda_nota_t; // TDA NOTAS , GUARDAS LOS PARAMETROS DE UNA NOTA LEIDA EN FORMATO MIDI
typedef struct notas_guardadas notas_guardadas_t;	// MANEJAS LAS NOTAS ALMACENADAS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EL TDA NOTAS SE ENCARGA DE MANEJAR LA INFORMACION OBTENIDA DEL ARCHIVO MIDI Y TRABAJA CON ESTOS DATOS PARA 
// PASAR LA INFORMACION EN PARAMETROS NECESARIOS PARA LA SINTETIZACION 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

notas_guardadas_t *nota_crear_espacio();
bool nota_guardar_encendida(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim);
bool nota_guardar_apagada(notas_guardadas_t *notas,nota_t n,unsigned char oct,unsigned char inte,uint32_t tim);
bool nota_redimensionar_vec(notas_guardadas_t *notas);
void notas_destruir(notas_guardadas_t *notas);
size_t nota_guardadas_longitud(notas_guardadas_t *notas);
void nota_parsar_pulsos_negra(notas_guardadas_t *notas,uint16_t pulsos_negra);
float nota_frecuencia_pura(notas_guardadas_t *notas,size_t n);
float nota_amplitud(notas_guardadas_t *notas,size_t n);
double nota_tiempo_inicial(notas_guardadas_t *notas,size_t n, uint16_t pulsos_por_segundo);
double nota_tiempo_final(notas_guardadas_t *notas,size_t n,uint16_t pulsos_por_segundo);

#endif