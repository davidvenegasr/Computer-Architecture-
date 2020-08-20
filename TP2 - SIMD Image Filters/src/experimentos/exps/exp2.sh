#!/bin/bash

#-------------------------------------------------------------------------------
# Este script genera los resultados para el experimento 2 del informe
# FORMA DE LLAMAR AL SCRIPT
#
#	./exp2.sh
#
# (solo se puede llamar desde la carpeta donde sera guardado el sh)
#-------------------------------------------------------------------------------

IMGPREFIX="imgs/imgs_var_tam/img."
CANT_ITER=50
IMGS=('32x16.bmp' '64x32.bmp' '128x64.bmp' '200x100.bmp' '256x128.bmp' '400x200.bmp' '512x256.bmp' '800x400.bmp' '1600x800.bmp')

# for i in "${IMGS[@]}"
# do
#     echo "${IMGPREFIX}${i}"
# done

cd ..
#Dir actual = "experimentos"



cd ../filters
#Dir actual = "filters"

mkdir .temp
cp Zigzag_asm.asm .temp
cp Zigzag_asm_exp1.asm .temp
mv Zigzag_asm_exp1.asm Zigzag_asm.asm





cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
cd experimentos
#Dir actual = "experimentos"

mkdir resultados 2> /dev/null
mkdir resultados/exp2 2> /dev/null
mkdir resultados/exp2/original 2> /dev/null
mkdir resultados/exp2/modded 2> /dev/null


echo "++++++++++++ EJECUTANDO Zigzag_MODIFICADO ASM ..."
for i in "${IMGS[@]}"
do
	echo "+ IMG actual: ${IMGPREFIX}${i}"
	./filter_ejec.sh Zigzag asm $CANT_ITER "${IMGPREFIX}${i}"
	mv log.Zigzag.asm.out "resultados/exp2/modded/log.exp2.asm.${i}.out"
	mv log.Zigzag.asm.csv "resultados/exp2/modded/log.exp2.asm.${i}.csv"
done
echo "++++++++++++ TERMINADO Zigzag_MODIFICADO ASM_________________"
#HASTA ACA tengo los resultados de Zigzag_MODIFICADO

rm img.*bmp

cd ../filters
#Dir actual = "filters"



rm Zigzag_asm.asm 
cp .temp/Zigzag_asm_exp1.asm .
cp .temp/Zigzag_asm.asm .
rm -rf .temp/

cd ..
#Dir actual = "src"
echo "********************** MAKE CLEAN *************************"
make clean
echo "********************** MAKE -O0 ***************************"
make
echo "********************** end MAKE -O0 ***********************"
cd experimentos
#Dir actual = "experimentos"


echo "++++++++++++ EJECUTANDO Zigzag_ORIGINAL ASM ..."
for i in "${IMGS[@]}"
do
	echo "+ IMG actual: ${IMGPREFIX}${i}"
	./filter_ejec.sh Zigzag asm $CANT_ITER "${IMGPREFIX}${i}"
	mv log.Zigzag.asm.out "resultados/exp2/original/log.exp2.asm.${i}.out"
	mv log.Zigzag.asm.csv "resultados/exp2/original/log.exp2.asm.${i}.csv"
done
echo "++++++++++++ TERMINADO Zigzag_ORIGINAL ASM_________________"
#HASTA ACA tengo los resultados de Zigzag_MODIFICADO


rm img.*bmp
