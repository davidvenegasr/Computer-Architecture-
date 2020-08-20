#!/bin/bash

#-------------------------------------------------------------------------------
# Este script genera los resultados para el experimento 1 del informe
# FORMA DE LLAMAR AL SCRIPT
#
#	./exp1_Descubrir.sh
#
# (solo se puede llamar desde la carpeta donde sera guardado el sh)
#-------------------------------------------------------------------------------


CANT_ITER=500
IMG1="imgs/1.descubrir.bmp"

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



echo "++++++++++++ EJECUTANDO Descubrir ASM ..."
./filter_ejec.sh Descubrir asm $CANT_ITER $IMG1

mkdir resultados 2> /dev/null
mkdir resultados/exp1_Descubrir 2> /dev/null

mv log.Descubrir.asm.out resultados/exp1_Descubrir
mv log.Descubrir.asm.csv resultados/exp1_Descubrir

#HASTA ACA tengo los resultados de asm


echo "++++++++++++ EJECUTANDO Descubrir C -O0 ..."
./filter_ejec.sh Descubrir c $CANT_ITER $IMG1

mv log.Descubrir.c.out resultados/exp1_Descubrir/log.Descubrir.c.O0.out
mv log.Descubrir.c.csv resultados/exp1_Descubrir/log.Descubrir.c.O0.csv

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

echo "++++++++++++ EJECUTANDO Descubrir C -O3 ..."
./filter_ejec.sh Descubrir c $CANT_ITER $IMG1

mv log.Descubrir.c.out resultados/exp1_Descubrir/log.Descubrir.c.O3.out
mv log.Descubrir.c.csv resultados/exp1_Descubrir/log.Descubrir.c.O3.csv


cp data/Makefile_O0 data/Makefile
mv data/Makefile ../filters/Makefile

rm 1.descubrir.bmp.Descubrir.*

cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
