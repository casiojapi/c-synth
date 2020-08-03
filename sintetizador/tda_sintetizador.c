#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "tda_sintetizador.h"
#include "tda_tramo.h"

//Declaracion de un struct para que sea mas facil el paso de la cadena en "sintetizador.txt" a la funcion de modulacion elegida. 
const dicc_modulacion_t operaciones_modulacion[MOD_TOTAL] = {
    {"CONSTANT", mod_constant},
    {"LINEAR", mod_linear},
    {"INVLINEAR", mod_invlinear},
    {"SIN", mod_sin},
    {"EXP", mod_exp},
    {"INVEXP", mod_invexp},
    {"QUARTCOS", mod_quartcos},
    {"QUARTSIN", mod_quartsin},
    {"HALFCOS", mod_halfcos},
    {"HALFSIN", mod_halfsin},
    {"LOG", mod_log},
    {"INVLOG", mod_invlog},
    {"TRI", mod_tri},
    {"PULSES", mod_pulses}
};

//Struct interna para almacenar armonicos dentro de sintetizador.
struct armonicos{
	size_t n;       //Almacena la n cantidad de armonicos leidos del archivo "sintetizador.txt"
    float *a_arm;   //Puntero a vector de amplitudes de armonicos
    float *f_arm;   //Puntero a vector de frecuencias de armonicos
};

//Struct interna para almacenar las funciones de modulacion dentro de un sintetizador.
struct modulacion{
    f_modulacion_t funcion;     //Guarda la funcion leida del archivo "sintetizador.txt"
    float *parametros;          //Puntero a vector de parametros de la funcion puntual que acompagna a en este struct
};

//Struct principal del TDA sintetizador. 
struct tda_sintetizador{
    armonicos_t *arm;           //Guarda un vector de armonicos, de cantidad N que se haya leido en el archivo "sintetizador.txt"
    modulacion_t *mod;          //Guarda un vector de modulaciones, de cantidad 3 que haya leido del archivo "sintetizador.txt"
};



//Crea un sintetizador, devuelve NULL en caso de que la memoria falle (y libera la memoria pedida en caso de falla).
tda_sintetizador_t *sintetizador_crear(){
    tda_sintetizador_t *sint;
    if((sint = malloc(sizeof(tda_sintetizador_t))) ==  NULL)
        return NULL;

    if ((sint->arm = malloc(sizeof(armonicos_t) * MAX_ARMONICOS)) == NULL){
        free(sint);
        return NULL;
    }
    
    if ((sint->arm->a_arm = malloc(sizeof(float) * MAX_ARMONICOS)) == NULL){
        free(sint->arm);
        free(sint);
        return NULL;
    }

    if ((sint->arm->f_arm = malloc(sizeof(float) * MAX_ARMONICOS)) == NULL){
        free(sint->arm->a_arm);
        free(sint->arm);
        free(sint);
        return NULL;
    }

    if((sint->mod = malloc(sizeof(modulacion_t) * MOD_MAX)) == NULL){
        free(sint->arm->f_arm);
        free(sint->arm->a_arm);
        free(sint->arm);
        free(sint);
        return NULL;
    }
    for(size_t i = 0; i < MAX_PARAMETROS; i++){
        if((sint->mod[i].parametros = malloc(sizeof(float) * MAX_PARAMETROS)) == NULL){
            _destruir_sintetizador_n(sint, --i);
            return NULL;
        }
    }
    return sint;
}


//Funcion interna que en caso de haber fallo de memoria a la hora de crear un sintetizador, y falle en la asignacion de parametros, se le puede asignar hasta que indice liberar.
void _destruir_sintetizador_n(tda_sintetizador_t *sint, size_t n){
    free(sint->arm->a_arm);
    free(sint->arm->f_arm);
    free(sint->arm);
    for(size_t i = 0; i < n; i++){
        free(sint->mod[i].parametros);
    }
    free(sint->mod);
    free(sint);
}

//Funcion para destruir sintetizador. 
void destruir_sintetizador(tda_sintetizador_t *sint){
    free(sint->arm->a_arm);
    free(sint->arm->f_arm);
    free(sint->arm);
    for(size_t i = 0; i < MAX_PARAMETROS; i++){
        free(sint->mod[i].parametros);
    }
    free(sint->mod);
    free(sint);
}

//PRIMITIVAS QUE NECESITAMOS
//armonicos

void sinte_get_fqr_arm_n_arm(float **arm,float **fqr,size_t *n,tda_sintetizador_t *sint){
    *arm=sint->arm->a_arm;
    *fqr=sint->arm->f_arm;
    *n=sint->arm->n;
}

//Funciones que devuelven algo especifico del sintetizador, para no tener que acceder internamente. (modulacion)


void sinte_ataque_sostenido_decaimiento(tda_sintetizador_t *sint,f_modulacion_t *ataque,f_modulacion_t *sotenido,f_modulacion_t *decaimiento){
    *ataque=sint->mod[0].funcion;
    *sotenido=sint->mod[1].funcion;
    *decaimiento=sint->mod[2].funcion;
}

//(tiempos de modulacion)
double sinte_get_td(tda_sintetizador_t *sint){
    return sint->mod[2].parametros[0];
}
double sinte_get_ta(tda_sintetizador_t *sint){
    return sint->mod[0].parametros[0];
}

//(parametros de modulacion)
float *sinte_get_para_ataque(tda_sintetizador_t *sint){
    return sint->mod[0].parametros;
}

float *sinte_get_para_sostenido(tda_sintetizador_t *sint){
    return sint->mod[1].parametros;
}

float *sinte_get_para_decaimiento(tda_sintetizador_t *sint){
    return sint->mod[2].parametros;
}



//Lectura de archivo "sintetizador.txt".
//Recibe un puntero a sintetizador ya creado, con su struct de armonicos ya creado pero no inicializado.
bool leer_archivo_sintetizador(tda_sintetizador_t *sint, char *nombre_archivo){  
    if (sint == NULL)
        return false;
        
    FILE *sinte_txt;
    sinte_txt = fopen(nombre_archivo, "r");
    if (sinte_txt == NULL){
        return false;
    }
    char aux[MAX_STR];

    //Lectura y procesamiento de N cantidad de armonicos.

    if(fgets(aux, MAX_STR, sinte_txt) == NULL){
        fclose(sinte_txt);
        return false;
    }

    size_t n = atoi(aux);
    sint->arm->n = n;

    //Lectura y procesamiento de armonicos.

    for(size_t i = 0; i < n; i++){
        if(fgets(aux, MAX_STR, sinte_txt) == NULL){
            fclose(sinte_txt);
            return false;
        }
        char *resto;
        sint->arm->a_arm[i] = strtof(aux, &resto);
        sint->arm->f_arm[i] = atof(resto);
    }
    //Lectura y procesamiento de funciones de modulacion.

    for(size_t i = 0; i < MOD_MAX; i++){
        if((fgets(aux, MAX_STR, sinte_txt)) == NULL){
            fclose(sinte_txt);
            return false;
        }
        char token[MAX_STR];

        sscanf(aux, "%s %f %f %f", token, &(sint->mod[i].parametros[0] ), &(sint->mod[i].parametros[1]), &(sint->mod[i].parametros[2]));
        size_t j;

        for(j = 0; j < MOD_TOTAL; j++){
            if(!strcmp(token, operaciones_modulacion[j].modulacion_nombre)){
                sint->mod[i].funcion = operaciones_modulacion[j].funcion;
                break;
            }
        }
        if(j == MOD_TOTAL){
            fclose(sinte_txt);
            return false;
        }        
    }
    fclose(sinte_txt);
    return true;
}



//Funciones de modulacion. Se declararon todas con los mismos parametros para poder llamarlas como punteros a funcion luego, en el momento de la sintesis. 
//Todas devuelven float, y se usa (cuando se llame) como factor para modular correctamente la onda.
float mod_constant(const float *parametros, double t){
    return 1.0;
}

float mod_linear(const float *parametros, double t){
    return t/parametros[0];
}

float mod_invlinear(const float *parametros, double t){
    if(1.0 - t/parametros[0] > 0)
        return 1.0 - t/parametros[0];
    return 0;
}

float mod_sin(const float *parametros, double t){
    return 1.0 + (parametros[0] * sin(parametros[1] * t));
}

float mod_exp(const float *parametros, double t){
    return exp((5.0 * (t - parametros[0]))/parametros[0]);
}

float mod_invexp(const float *parametros, double t){
    return exp((-5.0 * t)/parametros[0]);
}

float mod_quartcos(const float *parametros, double t){
    return cos((PI * t)/(2.0 * parametros[0]));
}

float mod_quartsin(const float *parametros, double t){
    return sin((PI * t) / (2.0 * parametros[0]));
}

float mod_halfcos(const float *parametros, double t){
    return (1.0 + cos((PI * t)/parametros[0])) / 2.0;
}

float mod_halfsin(const float *parametros, double t){
    return (1.0 + sin(PI * ((t/parametros[0]) - 0.5))) / 2.0;
}

float mod_log(const float *parametros, double t){
    return log10(((-9.0 * t)/parametros[0]) + 1.0);
}

float mod_invlog(const float *parametros, double t){
    if(t < parametros[0])
        return log10(((-9.0 * t)/parametros[0]) + 10.0 );
    return 0;
}
float mod_tri(const float *parametros, double t){
    if(t < parametros[1])
        return (t * parametros[2])/parametros[1];
    return (((t - parametros[1])/(parametros[1] - parametros[0])) * (parametros[2] - 1.0)) + parametros[2];
}
float mod_pulses(const float *parametros, double t){
    double tprim = ((t/parametros[0]) - floor(t/parametros[0])) * parametros[0];
    float res = abs(((1.0 - parametros[2]) / parametros[1]) * (tprim - parametros[0] + parametros[1]) + parametros[2]);
    if(res < 1)
        return res;
    return 1;
}
