#!/bin/bash

#-------------------------------------------------------------------------------
# Este script genera los resultados para el experimento 1 del informe
# FORMA DE LLAMAR AL SCRIPT
#
#	./exp1_Zigzag.sh
#
# (solo se puede llamar desde la carpeta donde sera guardado el sh)
#-------------------------------------------------------------------------------


CANT_ITER=500
IMG1="imgs/1.bmp"

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



echo "++++++++++++ EJECUTANDO Zigzag ASM ..."
./filter_ejec.sh Zigzag asm $CANT_ITER $IMG1

mkdir resultados 2> /dev/null
mkdir resultados/exp1_Zigzag 2> /dev/null

mv log.Zigzag.asm.out resultados/exp1_Zigzag
mv log.Zigzag.asm.csv resultados/exp1_Zigzag

#HASTA ACA tengo los resultados de asm


echo "++++++++++++ EJECUTANDO Zigzag C -O0 ..."
./filter_ejec.sh Zigzag c $CANT_ITER $IMG1

mv log.Zigzag.c.out resultados/exp1_Zigzag/log.Zigzag.c.O0.out
mv log.Zigzag.c.csv resultados/exp1_Zigzag/log.Zigzag.c.O0.csv

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

echo "++++++++++++ EJECUTANDO Zigzag C -O3 ..."
./filter_ejec.sh Zigzag c $CANT_ITER $IMG1

mv log.Zigzag.c.out resultados/exp1_Zigzag/log.Zigzag.c.O3.out
mv log.Zigzag.c.csv resultados/exp1_Zigzag/log.Zigzag.c.O3.csv


cp data/Makefile_O0 data/Makefile
mv data/Makefile ../filters/Makefile

rm 1.bmp.Zigzag.*.bmp

cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
