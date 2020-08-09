#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "codificacion_decodificacion.h"
#include "tda_nota.h"
#include "lectura_midi.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	LECTURA MIDI ES UNA FUNCION ENCARGADA DE ASEGURAR QUE EL ARCHIVO LEIDO SEA CORRECTO Y EXTRAE LAS NOTAS UNA A UNA PARA SE GUARDADAS  
//	EN EL TDA CORRESPONDIENTE EN CASO QUE ALGUNAS DE ESTAS CONDICIONES FALLE EL ARCHIVO DEVOLVERA EL ERROR COMETIDO 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};

bool leer_midi(FILE* f,notas_guardadas_t *notas_guardadas, char canal_elegido){

	// LECTURA DEL ENCABEZADO:
    formato_t formato;
    uint16_t numero_pistas;
    uint16_t pulsos_negra;

    if(! leer_encabezado(f, &formato, &numero_pistas, &pulsos_negra)) {
        fprintf(stderr, "Fallo lectura encabezado\n");
        return false;
    }

    // ITERAMOS LAS PISTAS:
    for(uint16_t pista = 0; pista < numero_pistas; pista++) {
        // LECTURA ENCABEZADO DE LA PISTA:
        uint32_t tamagno_pista;
        if(! leer_pista(f, &tamagno_pista)) {
            fprintf(stderr, "Fallo lectura pista\n");
            return false;
        }

        evento_t evento;
        char canal;
        int longitud;
        uint32_t tiempo = 0;

        // ITERAMOS LOS EVENTOS:
        while(1) {
            uint32_t delta_tiempo;
            leer_tiempo(f, &delta_tiempo);
            tiempo += delta_tiempo;

            // LECTURA DEL EVENTO:
            uint8_t buffer[EVENTO_MAX_LONG];
            if(! leer_evento(f, &evento, &canal, &longitud, buffer)) {
                fprintf(stderr, "Error leyendo evento\n");
                return false;
            }
            
            // PROCESAMOS EL EVENTO:
            if(evento == METAEVENTO && canal == CANAL_15) {
                // METAEVENTO:
                if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
                    break;
                }
                descartar_metaevento(f, buffer[METAEVENTO_LONGITUD]);
            }
            else if (evento == NOTA_ENCENDIDA || evento == NOTA_APAGADA) {
                // NOTA:
                nota_t nota;
                signed char octava;
                if(!decodificar_nota(buffer[EVNOTA_NOTA], &nota, &octava)) {
                    fprintf(stderr, "Error leyendo nota\n");
                    return false;
                }
                if (canal == canal_elegido){
                    if (evento == NOTA_ENCENDIDA){
                        if (!nota_guardar_encendida(notas_guardadas, nota, octava, buffer[EVNOTA_VELOCIDAD], tiempo)){
                        
                            fprintf(stderr, "Error guardar notas encendidas\n");
                            return false;
                        }
                    }
                    if (evento == NOTA_APAGADA){
                        if (!nota_guardar_apagada(notas_guardadas, nota, octava, buffer[EVNOTA_VELOCIDAD], tiempo)){
                        
                            fprintf(stderr, "Error guardar nota apagada\n");
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         FUNCIONES DE LECTURA MIDI , ENCARGADAS DE LEER TODO EL ARCHIVO VALIDARLO Y PASAR LAS NOTAS A DECODIFICAR 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t leer_uint8_t(FILE *f){

    char buffer[MAX_BUFFER];
    fread(buffer,sizeof(char),sizeof(buffer),f);

    return buffer[0];
}

static uint16_t leer_uint16_big_endian(FILE *f){

    uint16_t valor=0;
    for (int i = 0; i < 2; ++i){
        valor = valor|(leer_uint8_t(f)<<(SHIF_UINT8*(1-i)));
    }
    return valor;
} 

static uint32_t leer_uint32_big_endian(FILE *f){

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
notas_guardadas_t* lectra_notas(char *nombre_midi,char canal){
    // APERTURA DE ARCHIVO MIDI:
    FILE *archivo_midi = fopen(nombre_midi, "rb");
     if(archivo_midi == NULL) {
        fprintf(stderr, "Error, no se pudo abrir el archivo midi: \"%s\".\n", nombre_midi);
        return NULL;
    }
    //LECTURA DEL ARCHIVO MIDI
    notas_guardadas_t *notas = nota_crear_espacio();
    if (!leer_midi(archivo_midi, notas, canal)){
        fprintf(stderr, "Error, No se pudo leer el archivo midi: \"%s\".\n", nombre_midi);
        notas_destruir(notas);
        fclose(archivo_midi);
        return NULL;
    }
    fclose(archivo_midi);
    return notas;
}    