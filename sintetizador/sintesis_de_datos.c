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

    sinte_get_fqr_arm_n_arm(&a_arm, &f_arm, &n_arm, sinte);

    f_modulacion_t ataque, sostenido, decaimiento;

    sinte_ataque_sostenido_decaimiento(sinte, &ataque, &sostenido, &decaimiento);

    float *para_ataque = sinte_get_para_ataque(sinte);
    float *para_sostenido = sinte_get_para_sostenido(sinte);
    float *para_decaimiento = sinte_get_para_decaimiento(sinte);

    double td = sinte_get_td(sinte);
	double ta = sinte_get_ta(sinte);

    tda_tramo_t *tramo_completo = muestreo_completo(t0, tf, td, f_m, fre, amp, a_arm, f_arm, n_arm, n_datos, para_ataque, para_sostenido, para_decaimiento, ataque, sostenido, decaimiento, ta);
    
    return tramo_completo;
}
