#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "tda_sintetizador.h"
#include "tda_tramo.h"

//Struct del TDA tramo. Se encarga de guardar los muestreos, y almacenar la informacion necesaria para luego redimension y juntar todos los tramos en uno unico. 
struct tda_tramo{
    float *v;   //vector que almacena las muestras del tramo
    size_t n;   //guarda una N cantidad de muestreos
    double t0;  //guarda el t0 del tramo en cuestion
    double tf;  //guarda el tf del tramo en cuestion
    double td;  //guarda el td del tramo en cuestion (luego se va a utilizar para sintetizar de t0 hasta tf + td)
};

//Funcion que se encarga de crear un TDA tramo, recibiendo los siguientes parametros.
//Precondicion: que sean datos validos leidos desde el archivo midi.
tda_tramo_t *tramo_crear(double t0, double tf, int f_m, double td){     //toma el tiempo de decaimiento para muestrear desde t0 hasta tf + td desde el principio
    tda_tramo_t *tramo;
    if((tramo = malloc(sizeof(tda_tramo_t))) == NULL)
        return NULL;

    tramo->t0 = t0;
    size_t n = (((double)f_m *(tf + td)) - ((double)f_m * t0)) + 1;
    tramo->n = n; 
    tramo->tf = tf;
    tramo->td = td;

    if((tramo->v = malloc(n * sizeof(float))) == NULL){
        free(tramo);
        return NULL;
    }
    return tramo;
}

//Funcion que libera toda la memoria alloqueada en un tramo.
//Precondicion: el tramo debe tener un vector que no apunte a NULL, y el puntero al tramo tampoco debe apuntar a NULL 
void tramo_destruir(tda_tramo_t *t){
    free(t->v);
    free(t);
}

//Funcion que se encarga de hacer un muestreo de una frecuencia y amplitud puntual en un vector. 
//Precondicion: el puntero que recibe no debe apuntar a NULL, y debe ser un vector de muestreos valido.
void muestrear_senoidal(float *v, size_t n, double t0, int f_m, float f, float a){
    double t = t0;
    for (size_t i = 0; i < n; i++) {
        t = (i * 1.0)/f_m;
        v[i] += a * sin(2*PI*f*t);
    }
}

//Funcion simple que unicamente inicializa las muestras de un vector a 0.
//Precondicion: el puntero que recibe no debe apuntar a NULL, y debe ser un vector de muestreos valido.
void inicializar_muestras(float *v, size_t n){
    for (size_t i = 0; i < n; i++){
        v[i] = 0;
    }
}

//Funcion que unicamente muestrea armonicos recibiendo un vector ya existente. Se usa luego en "tramo_crear_muestreo".
//Precondicion: los punteros que recibe no deben estar apuntando a NULL, y debe ser un vector de muestreos valido.
void muestrear_armonicos(float *v, size_t n, double t0, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_fa){
    inicializar_muestras(v, n);
    for (size_t i = 0; i < n_fa; i++) {
        muestrear_senoidal(v, n, t0, f_m, f * f_arm[i], a * a_arm[i]);
    }
}

//Funcion que recibe los siguientes parametros y a partir de estos crea un TDA tramo, lo inicializa, muestrea su frecuencia fundamental, y luego todos sus armonicos. 
//Precondicion: Los punteros que recibe a vectores de armonicos no deben apuntar a NULL.
tda_tramo_t *tramo_crear_muestreo(double t0, double tf, double td, int f_m, float f, float a, float *a_arm, float *f_arm, size_t n_arm){
    tda_tramo_t *tramo = tramo_crear(t0, tf, f_m, td);
    if (tramo == NULL){
        return NULL;    //valido que no sea NULL ya que en _tramo_crear tengo un malloc
    }
    muestrear_armonicos(tramo->v, tramo->n, t0, f_m, f, a, a_arm, f_arm, n_arm);
    return tramo;
}


//Funcion que recibe un tramo ya existente y lo extiende teniendo en cuenta nuevo parametro tf.
//Precondicion: El tramo que recibe, no debe apuntar a NULL.
bool tramo_redimensionar(tda_tramo_t *t, double tf, int f_m){
    size_t n_nuevo = ((f_m * tf) - (f_m * t->t0)) + 1;        
    double tf_viejo = (((t->n) -1)/(f_m)) + t->t0;
    float *aux;
    if ((aux = realloc(t->v, n_nuevo * sizeof(float))) == NULL)
        return false;

    t->v = aux;
    if(tf > tf_viejo)
        inicializar_muestras(t->v+(t->n), n_nuevo - (t->n));
    
    t->n = n_nuevo;
    return true;
}

//Funcion que recibe un tramo destino, y otro a agregarse al anterior. Usamos esta funcion para ir sumando el tramo de cada nota, y terminar con un tramo contenedor de todas las notas muestreadas. 
//Precondicion: los tramos no deben apuntar a NULL, y el t0 del tramo a agregarse debe ser mayor al t0 del tramo destino.
bool tramo_extender(tda_tramo_t *destino, const tda_tramo_t *extension, int f_m){
    if(destino->t0 > extension->t0)
        return false;
    
    double tf_destino = (((destino->n -1.0)/f_m) + destino->t0) + destino->td;      //agrego td a tf ya que este seria el tiempo final "real"
    double tf_extension = (((extension->n -1.0)/f_m) + extension->t0) + extension->td;   //agrego td por la misma razon que arriba
    if(tf_extension > tf_destino)
        tramo_redimensionar(destino, tf_extension, f_m);         //redimensiono en el caso de que tf de extension sea mas grande que el tf de destino

    size_t n_ext = ((f_m * extension->t0) - (f_m * destino->t0));     //saco lo n corrido que esta extension con respecto a destino
    
    for(size_t i = 0; i < extension->n; i++){
        destino->v[n_ext + i] += extension->v[i];       //le sumo n_ext al vector destino para sumar apartir del inicio de extension
    }
    return true;
}


// Funcion que recibe un tramo y las funciones y parametros leidos del archivo de texto "sintetizador.txt"
// Precondicion: que no reciba el tramo apuntando a NULL. 
void muestrear_modulacion(tda_tramo_t *tramo, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, int f_m, float *para_ataque, float *para_sostenido, float *para_decaimiento, double t_a, double t_d){
    double t0 = 0;
    double t;
    double duracion = tramo->tf - tramo->t0;
    float modulacion;

    for(size_t i = 0; i < tramo->n; i++){
        t = ((double)i)/f_m;
        if ((t >= t0) && (t < (t0 + t_a))){
            modulacion = ataque(para_ataque, t);
            tramo->v[i] = modulacion * tramo->v[i];

        } else if ((t >= (t0 + t_a)) && (t < (t0 + duracion))){
            modulacion = sostenido(para_sostenido, t - (t0 + t_a));
            tramo->v[i] = modulacion * tramo->v[i];

        } else if ((t >= t0 + duracion) && (t <= t0 + duracion + t_d)){
            modulacion = decaimiento(para_decaimiento, t - (t0 + duracion));
            modulacion *= sostenido(para_sostenido, t0 + duracion);
            tramo->v[i] = modulacion * tramo->v[i];

        } else
            break;
    }
}

//Funcion que recibe una serie parametros para hacer mas sencilla la sintesis y muestreo completo desde el main. 
//Precondicion: NO puede recibir punteros a NULL, 
tda_tramo_t *muestreo_completo(double *t0, double *tf, double td, int f_m, float *fre, float *amp, float *a_arm, float *f_arm, size_t n_arm, size_t n_notas, float *para_ataque, float *para_sostenido, float *para_decaimiento, f_modulacion_t ataque, f_modulacion_t sostenido, f_modulacion_t decaimiento, double t_a){
    size_t i = 0;
    tda_tramo_t *tramo_uno = tramo_crear_muestreo(t0[i], tf[i], td, f_m, fre[i], amp[i], a_arm, f_arm, n_arm);
    if(tramo_uno == NULL){
        printf("error de memoria creando tramo\n");
        return NULL;
    }
    muestrear_modulacion(tramo_uno, ataque, sostenido, decaimiento, f_m, para_ataque, para_sostenido, para_decaimiento, t_a, td);
    tda_tramo_t *tramo_siguiente;
    for(i = 1; i < n_notas; i++){
        tramo_siguiente = tramo_crear_muestreo(t0[i], tf[i], td, f_m, fre[i], amp[i], a_arm, f_arm, n_arm);
        if(tramo_siguiente == NULL){
            tramo_destruir(tramo_uno);
            return NULL;
        }
        muestrear_modulacion(tramo_siguiente, ataque, sostenido, decaimiento, f_m, para_ataque, para_sostenido, para_decaimiento, t_a, td);
        if (!tramo_extender(tramo_uno, tramo_siguiente, f_m)){
            tramo_destruir(tramo_uno);
            tramo_destruir(tramo_siguiente);
            return NULL;
        }
        tramo_destruir(tramo_siguiente);
    }
    return tramo_uno;
}


//Funcion que pasa el vector desde adentro de un tramo a un vector de int16_t y lo escala para dejar el maximo en 32667. Devuelve el numero de muestras por la interfaz (n_muestras)
//Precondicion: El puntero al tramo no debe apuntar a NULL.
int16_t *tramo_a_int16(tda_tramo_t *tramo, size_t *n_muestras){
    if(tramo == NULL){
        return NULL;
    }
    float cte; 
    float max = 0;
    for(size_t i = 0; i < tramo->n; i++){
        if(tramo->v[i] > max)
            max = tramo->v[i];
    }
    cte = (float)MAX_INT16T/max;
    int16_t *vector;
    if((vector = malloc(sizeof(int16_t) * tramo->n)) == NULL){
        return NULL;
    }
    for(size_t i = 0; i < tramo->n; i++){
        vector[i] = tramo->v[i] * cte;
    }
    *n_muestras = tramo->n;
    return vector;
}
