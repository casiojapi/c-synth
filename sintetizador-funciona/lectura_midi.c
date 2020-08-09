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
                        if (!guardar_nota_encendida(notas_guardadas, nota, octava, buffer[EVNOTA_VELOCIDAD], tiempo)){
                        
                            fprintf(stderr, "Error guardar notas encendidas\n");
                            return false;
                        }
                    }
                    if (evento == NOTA_APAGADA){
                        if (!guardar_nota_apagada(notas_guardadas, nota, octava, buffer[EVNOTA_VELOCIDAD], tiempo)){
                        
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