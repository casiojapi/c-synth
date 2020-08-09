#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "codificacion_decodificacion.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//            FUNCIONES QUE DECODIFICAN LOS DATOS LEIDOS DEL ARCHIVO MIDI PARA DARLE UN SIGNIFICADO MUSICAL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         FUNCIONES DE LECTURA MIDI , ENCARGADAS DE LEER TODO EL ARCHIVO VALIDARLO Y PASAR LAS NOTAS A DECODIFICAR 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t leer_uint8_t(FILE *f){

	char buffer[MAX_BUFFER];
	fread(buffer,sizeof(char),sizeof(buffer),f);

	return buffer[0];
}

uint16_t leer_uint16_big_endian(FILE *f){

	uint16_t valor=0;
	for (int i = 0; i < 2; ++i){
		valor = valor|(leer_uint8_t(f)<<(SHIF_UINT8*(1-i)));
	}
	return valor;
} 

uint32_t leer_uint32_big_endian(FILE *f){

	uint32_t valor=0;
	for (int i = 0; i < 4; ++i){
		valor = valor| (leer_uint8_t(f)<<(SHIF_UINT8*(3-i)));
	}
	return valor;
}

bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra){

	if (leer_uint32_big_endian(f)!=HEADER_ID){
		return false;
	}
	if (leer_uint32_big_endian(f)!=SIZE){
		return false;
	}

	if (!decodificar_formato(leer_uint16_big_endian(f),formato)){
		return false;
	}

	*numero_pistas=leer_uint16_big_endian(f);
	*pulsos_negra=leer_uint16_big_endian(f);

	return true;
}

bool leer_pista(FILE *f, uint32_t *tamagno){

	if (leer_uint32_big_endian(f)!=IDENT_MIDI){
		return false;
	}

	*tamagno=leer_uint32_big_endian(f);

	return true;
}

bool leer_tiempo(FILE *f, uint32_t *tiempo){
	*tiempo = 0;
	for (int i = 0; i < 4; ++i){
		uint8_t tem_aux= leer_uint8_t(f);
		*tiempo=*tiempo|(tem_aux & (~MASK_TIEMPO_SIGUIENTE));
		if (!(tem_aux & MASK_TIEMPO_SIGUIENTE)){
			
			return true;
		}
		*tiempo=*tiempo<<SHIF_TIEMPO_SIGUIENTE;
	}

	return false;
}

bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]){
	
	int i=0;

	uint8_t primer_even =leer_uint8_t(f);
	if ((decodificar_evento(primer_even,evento,canal,longitud))==false){
		mensaje[0]=primer_even;
		i++;
	}
	for (; i < *longitud; ++i){
		mensaje[i]=leer_uint8_t(f);
	}
	return true;
}

void descartar_metaevento(FILE *f, uint8_t tamagno){
	for (int i = 0; i < tamagno; ++i){
		leer_uint8_t(f);
	}
}