#include <stdbool.h>
#include <stdlib.h>

#ifndef LECTURA_MIDI_H
#define LECTURA_MIDI_H

#include "codificacion_decodificacion.h"
#include "tda_nota.h"


#define CANAL_15 0xF
#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10
#define MAX_NOTA_ENCENDIDAS 25

//Unica funcion que se encarga de hacer la lectura entera del archivo MIDI. 
//Precodicion: debe recibir un archivo midi valido.
bool leer_midi(FILE* f, notas_guardadas_t *notas_totales, char canal_elegido);

#endif