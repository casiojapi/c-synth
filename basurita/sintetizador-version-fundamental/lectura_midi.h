
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef LECTURA_MIDI_H
#define LECTURA_MIDI_H

#include "tda_nota.h"

#define CANAL_15 0xF
#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10
#define MAX_NOTA_ENCENDIDAS 25

//Unica funcion que se encarga de hacer la lectura entera del archivo MIDI. 
//Precodicion: debe recibir un archivo midi valido.
bool leer_midi(FILE* f, notas_guardadas_t *notas_totales, char canal_elegido);

//funciones para leer midi
bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra);
bool leer_pista(FILE *f, uint32_t *tamagno);
bool leer_tiempo(FILE *f, uint32_t *tiempo);
bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]);
void descartar_metaevento(FILE *f, uint8_t tamagno);
notas_guardadas_t* lectra_notas(char *nombre_midi , char canal);

#endif