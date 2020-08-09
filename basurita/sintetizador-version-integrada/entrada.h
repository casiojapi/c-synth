#include <stdbool.h>
#include <stdint.h>


#ifndef ENTRADA_H
#define ENTRADA_H

bool lectura_entrada(int argc,char const *argv[],char *nb_sint,char *nb_midi,char *nb_wave,uint16_t *f_m,uint16_t *p_s,char *canal);

#endif