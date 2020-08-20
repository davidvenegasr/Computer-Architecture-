#!/bin/bash

#-------------------------------------------------------------------------------
# Este script genera los resultados para el experimento 1 del informe
# FORMA DE LLAMAR AL SCRIPT
#
#	./exp1_Ocultar.sh
#
# (solo se puede llamar desde la carpeta donde sera guardado el sh)
#-------------------------------------------------------------------------------


CANT_ITER=500
IMG1="imgs/1.bmp"
IMG2="imgs/2.bmp"

cd ..
#Dir actual = "experimentos"



cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
cd experimentos
#Dir actual = "experimentos"



echo "++++++++++++ EJECUTANDO Ocultar ASM ..."
./filter_ejec.sh Ocultar asm $CANT_ITER $IMG1 $IMG2

mkdir resultados 2> /dev/null
mkdir resultados/exp1_Ocultar 2> /dev/null

mv log.Ocultar.asm.out resultados/exp1_Ocultar
mv log.Ocultar.asm.csv resultados/exp1_Ocultar

#HASTA ACA tengo los resultados de asm


echo "++++++++++++ EJECUTANDO Ocultar C -O0 ..."
./filter_ejec.sh Ocultar c $CANT_ITER $IMG1 $IMG2

mv log.Ocultar.c.out resultados/exp1_Ocultar/log.Ocultar.c.O0.out
mv log.Ocultar.c.csv resultados/exp1_Ocultar/log.Ocultar.c.O0.csv

#HASTA ACA tengo los resultados de c con -O0

#AHORA paso a hacer la modificacion del Makefile para cambiar la optimizacion

cp data/Makefile_O3 data/Makefile
mv data/Makefile ../filters/Makefile



cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O3 ***************************"
make
echo "********************** end MAKE -O3 ***********************"
cd experimentos
#Dir actual = "experimentos"

echo "++++++++++++ EJECUTANDO Ocultar C -O3 ..."
./filter_ejec.sh Ocultar c $CANT_ITER $IMG1 $IMG2

mv log.Ocultar.c.out resultados/exp1_Ocultar/log.Ocultar.c.O3.out
mv log.Ocultar.c.csv resultados/exp1_Ocultar/log.Ocultar.c.O3.csv


cp data/Makefile_O0 data/Makefile
mv data/Makefile ../filters/Makefile

rm 1.bmp.Ocultar.*.bmp

cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
