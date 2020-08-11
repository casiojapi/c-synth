#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "codificacion_decodificacion.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//            FUNCIONES QUE DECODIFICAN LOS DATOS LEIDOS DEL ARCHIVO MIDI PARA DARLE UN SIGNIFICADO MUSICAL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DEVUELVE LA LONGITUD DEPENDIENDO EL EVENTO LEIDO

static int lon_evento[]={
    [NOTA_APAGADA]=2,
    [NOTA_ENCENDIDA]=2,
    [NOTA_DURANTE]=2,
    [CAMBIO_DE_CONTROL]=2,
    [CAMBIO_DE_PROGRAMA]=1,
    [VARIAR_CANAL]=1,
    [CAMBIO_DE_PITCH]=2,
    [METAEVENTO]=2,
};

bool decodificar_formato(uint16_t valor, formato_t *formato){

	if (valor>=3){
		return false;
	}

	*formato=valor;

	return true;
}

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud){

	if ((valor&MASK_VALOR_EN_ALTO_EVENTO)==0){
		return false;
	}

	*evento=((valor&(~MASK_VALOR_EN_ALTO_EVENTO))>>SHIF_EVENTO);

	*longitud=lon_evento[*evento];

	*canal=(valor&MASK_CANAL);

	return true;
}

bool decodificar_nota(uint8_t valor, nota_t *nota, signed char *octava){

	if (valor>=128){
		return false;
	}

	*nota=(valor%12);
	*octava=(char)(((valor)/12)-1);

	return true;
}