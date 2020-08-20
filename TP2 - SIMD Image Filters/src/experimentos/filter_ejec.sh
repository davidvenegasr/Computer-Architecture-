#!/bin/bash

#-------------------------------------------------------------------------------
# FORMA DE LLAMAR AL SCRIPT
#
#	./filter_ejec.sh <filtro> <asm o c> <cant> <img1> <(solo para ocultar)img2>
#
# (en ese orden especificamente)
# (solo se puede llamar desde la carpeta donde sera guardado el sh)
#-------------------------------------------------------------------------------

#VARIABLES A MODIFICAR----------------------------

#IMAGENES PARA OCULTAR
#IMG_SRC 	= imagen a copiar
#IMG_SRC2 	= imagen a ocultar en IMG_SRC
	#IMG_OCULTAR_SRC="evolution.800x400.bmp"
	#IMG_OCULTAR_SRC2="idiocracy.800x400.bmp"

#IMAGEN A DESCUBRIR
	#IMG_DESCUBRIR_SRC="BladeRunner.bmp.Ocultar.C.bmp"

#IMAGEN PARA ZIGZAG
	#IMG_ZIGZAG_SRC="evolution.800x400.bmp"

#CANTIDAD DE EJECUCIONES POR LLAMADO
CANT_ITER=1
#-------------------------------------------------

INPUT_OPTION=$1		#FILTRO
INPUT_MODE=$2		#Implementacion
IT=$3				#cantidad de iteraciones a ejecutar

IMG_OCULTAR_SRC=$4
IMG_OCULTAR_SRC2=$5

IMG_DESCUBRIR_SRC=$4
IMG_ZIGZAG_SRC=$4

#NOMBRE DE LOGS
OUTPUT_LOG="log.${INPUT_OPTION}.${INPUT_MODE}.out"
OUTPUT_LOG2="log.${INPUT_OPTION}.${INPUT_MODE}.csv"


if [ "$INPUT_MODE" != "asm"  ] && [ "$INPUT_MODE" != "c" ]; then
	echo "La opcion de implementacion es incorrecta"
fi

rm $OUTPUT_LOG 2>/dev/null
rm $OUTPUT_LOG2 2>/dev/null


while [ $IT -gt "0" ]
do
	case $INPUT_OPTION in
		Ocultar)
			FLAGS="Ocultar -i $INPUT_MODE -t $CANT_ITER $IMG_OCULTAR_SRC $IMG_OCULTAR_SRC2"
			;;
		Descubrir)
			FLAGS="Descubrir -i $INPUT_MODE -t $CANT_ITER $IMG_DESCUBRIR_SRC"
			;;
		Zigzag)
			FLAGS="Zigzag -i $INPUT_MODE -t $CANT_ITER $IMG_ZIGZAG_SRC"
			;;
		*)
			echo "*** El Filtro $INPUT_OPTION no existe"
			;;
	esac
	../build/tp2 $FLAGS >> $OUTPUT_LOG
	../build/tp2 $FLAGS | grep "de ciclos insumidos totales" | awk '{print $7}' >> $OUTPUT_LOG2
	
	IT=`expr $IT - 1`
done
