#include "tda_sintetizador.h"
#include "tda_tramo.h"
#include "sintesis_de_datos.h"
#include "tda_datos.h"

tda_tramo_t *sintesis_completa(tda_sintetizador_t *sinte, datos_tranfer_t *trans_notas, int f_m){
    
    double *t0, *tf;
    float *amp, *fre;
	size_t n_datos;

    datos_pasar(trans_notas, &t0, &tf, &amp, &fre, &n_datos);


    float *a_arm ,*f_arm;
    size_t n_arm; 

    sintetizador_get_armonicos(sinte, &a_arm, &f_arm, &n_arm);

    f_modulacion_t ataque, sostenido, decaimiento;
    sintetizador_get_modulacion(sinte, &ataque, &sostenido, &decaimiento);

    float *para_ataque = sintetizador_parametros_ataque(sinte);
    float *para_sostenido = sintetizador_parametros_sostenido(sinte);
    float *para_decaimiento = sintetizador_parametros_decaimiento(sinte);

    double td = sintetizador_get_td(sinte);
	double ta = sintetizador_get_ta(sinte);

    tda_tramo_t *tramo_completo = tramo_muestreo_completo(t0, tf, td, f_m, fre, amp, a_arm, f_arm, n_arm, n_datos, para_ataque, para_sostenido, para_decaimiento, ataque, sostenido, decaimiento, ta);
    
    return tramo_completo;
}

