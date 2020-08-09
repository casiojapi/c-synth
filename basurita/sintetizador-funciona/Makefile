CFLAGS=-Wall -std=c99 -pedantic -g

all:main

main: tda_nota.o codificacion_decodificacion.o lectura_midi.o lectura_midi.h main.c tda_datos.o archivo_wave.o tda_sintetizador.o tda_tramo.o archivo_wave.h tda_tramo.h tda_sintetizador.h sintesis_de_datos.o sintesis_de_datos.h
	gcc $(CFLAGS) main.c tda_nota.o codificacion_decodificacion.o lectura_midi.o tda_datos.o tda_sintetizador.o tda_tramo.o sintesis_de_datos.o -o sintetizador archivo_wave.o -lm

codificacion_decodificacion.o: codificacion_decodificacion.c codificacion_decodificacion.h
	gcc $(CFLAGS) codificacion_decodificacion.c -c 

tda_nota.o: tda_nota.c codificacion_decodificacion.h tda_nota.h
	gcc $(CFLAGS) tda_nota.c -c 

lectura_midi.o: lectura_midi.h codificacion_decodificacion.h tda_nota.h lectura_midi.c
	gcc $(CFLAGS) lectura_midi.c -c 

tda_datos.o:tda_datos.c tda_datos.h tda_nota.h
	gcc $(CFLAGS) tda_datos.c -c 

sintesis_de_datos.o: sintesis_de_datos.h tda_sintetizador.h tda_tramo.h
	gcc $(CFLAGS) -c sintesis_de_datos.c -lm

archivo_wave.o: archivo_wave.h archivo_wave.c
	gcc $(CFLAGS) -c archivo_wave.c

tda_tramo.o:  tda_sintetizador.h tda_tramo.h tda_tramo.c
	gcc $(CFLAGS) -c tda_tramo.c -lm

tda_sintetizador.o:tda_sintetizador.h tda_tramo.h tda_sintetizador.c
	gcc $(CFLAGS) -c tda_sintetizador.c -lm

clean:
	rm *.o sintetizador

cleanwav:
	rm *.wav


