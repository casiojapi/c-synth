#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCION ENCARGADA DE RECIBIR LOS PARAMETROS DE ENTRADA Y GUARDARLOS PARA LA LECTURA Y ESCRITURA DEL ARCHIVO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool lectura_entrada(int argc,char const *argv[],char *nb_sint,char *nb_midi,char *nb_wave,uint16_t *f_m,uint16_t *p_s,char *canal){

	//USO DEL PROGRAMA
	if(argc < 7){
		fprintf(stderr, "Error, faltan comandos...\nUso: $ ./sintetizador -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav> [-c <canal>] [-f <frecuencia>] [-r <pulsosporsegundo>]\n");
		return false;
	}
	for (int i = 1; i < argc; ++i){
		if (!strcmp(argv[i],"-s")){
			i++;
			strcpy(nb_sint, argv[i]);
			
		}
		if (!strcmp(argv[i],"-i")){
			i++;
			strcpy(nb_midi, argv[i]);
		}
		if (!strcmp(argv[i],"-o")){
			i++;
			strcpy(nb_wave, argv[i]);
		}
		if (!strcmp(argv[i],"-r")){
			i++;
			*p_s = atoi(argv[i]);
		}
		if (!strcmp(argv[i],"-c")){
			i++;
			*canal = atoi(argv[i]);
		}
		if (!strcmp(argv[i],"-f")){
			i++;
			*f_m = atoi(argv[i]);
		}
	}
	return true;
}
