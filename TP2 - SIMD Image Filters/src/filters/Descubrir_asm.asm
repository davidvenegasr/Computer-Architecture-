global Descubrir_asm

section .rodata
ALIGN 16

mask_byte_03: times 4 DB 0x3,0x3,0x3,0x00
mask_byte_01: times 4 DB 0x1,0x1,0x1,0x00

mask_only_bit_i: times 4 DB 0x80, 0x0, 0x0, 0x0

mask_shuf_broadcast_BnW: DB 0x00,0x00,0x00,0xFF,0x04,0x04,0x04,0xFF,0x08,0x08,0x08,0xFF,0x0C,0x0C,0x0C,0xFF

mask_byte_alpha: times 4 DB 0x00, 0x00, 0x00, 0xFF

; void Descubrir_c(
;     uint8_t *src,
;     uint8_t *dst,
;     int width,
;     int height,
;     int src_row_size,
;     int dst_row_size)
; {

;Parametros:
;	rdi 		<--- uint8_t *src
;	rsi 		<--- uint8_t *dst
;	edx 		<--- int width
;	ecx 		<--- int height
; 	r8d 		<--- int src_row_size
; 	r9d 		<--- int dst_row_size

section .text
Descubrir_asm:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	
	;paso width a un reg no volatil
	mov ebx, edx
	
	%define src_ptr 		rdi
	%define dst_ptr 		rsi
	%define cols	 		ebx
	%define rows			ecx
	%define src_row_size	r8d
	%define dst_row_size	r9d
	
	%define src_mirror_ptr	r12
	
	;-------------------------- RENOMBRE DE MASCARAS ---------------------------
	movdqa xmm11, [mask_byte_03]
	movdqa xmm12, [mask_byte_01]
	movdqa xmm13, [mask_only_bit_i]
	movdqa xmm14, [mask_shuf_broadcast_BnW]
	movdqa xmm15, [mask_byte_alpha]

	%define r_mask_byte_03				xmm11
	%define r_mask_byte_01				xmm12
	%define r_mask_only_bit_i			xmm13
	%define r_mask_shuf_broadcast_BnW	xmm14
	%define r_mask_byte_alpha			xmm15
	;---------------------------------------------------------------------------
	
	
	;* QUIERO: inicializar src_mirror_ptr---------------------------------------
	;la idea es inicializarlo en la esquina inferior derecha de mi matriz
	;pero 4 pixeles atras, es decir:
	;	|p[n-1][m-6]|p[n-1][m-5]|p[n-1][m-4]|p[n-1][m-3]|p[n-1][m-2]|p[n-1][m-1]|
	;							 ^
	; siendo n las filas y m las columnas
	; asi puedo levantar los siguientes 4 pixeles siguientes
	; y la idea seria en cada iteracion darlos vuelta para mejor acceso
	
	;calculo (n*m*4 - 16)
	xor eax, eax
	mov eax, rows
	mov edx, cols
	
	mul edx		;n*m
	movsxd rax, eax
	lea src_mirror_ptr, [src_ptr + rax*4 - 16]
	;*--------------------------------------------------------------------------
	
	;inicializo variables
	xor eax, eax	; i = 0
	
.ciclo:
	xor edx, edx	; j = 0
	cmp eax, rows	; while (i < rows)
	jge .end
.ciclo_col:	
	cmp edx, cols	; while (j < cols)
	jge .end_of_row
		
		;OBS: CASI TODAS LAS REPRESENTACIONES SERAN EN LITTLE ENDIAN
		;	  (a menos que se indique lo contrario)
	
		;* QUIERO levantar 4 pixeles de memoria de src_ptr----------------------
		movdqa xmm0, [src_ptr]
		; xmm0 = |b0|g0|r0|a0|b1|g1|r1|a1|b2|g2|r2|a2|b3|g3|r3|a3| (src)
		;*----------------------------------------------------------------------
		
		
		;* QUIERO: levantar los 4 pixeles espejo e invertirlos------------------
		movdqa xmm1, [src_mirror_ptr]
		; xmm1 = |b0|g0|r0|a0|b1|g1|r1|a1|b2|g2|r2|a2|b3|g3|r3|a3| (src_mirror)
		
		;USO: PSHUFD - Shuffle Packed Doublewords
		pshufd xmm2, xmm1, 0x1B	;00 01 10 11
		; xmm8 = |b3|g3|r3|a3|b2|g2|r2|a2|b1|g1|r1|a1|b0|g0|r0|a0| (src_mirror)
		;*----------------------------------------------------------------------
		
		
		;PRIMERO voy a decodificar la informacion
		
		;**** QUIERO: obtener los dos bits de cada componente que van a reconstruir
		; a la imagen oculta, y hacer el XOR entre el mirror y el src
		; HINT: ( (src_mirror[i] >> 2) ^ src_matrix[i] ) & 0x3
		
		;* QUIERO: obtener src_mirror[i][j] >> 2
		; no tengo una forma de shiftear bits en un BYTE,
		; sin embargo no necesito transformarlos a WORDS porque
		; el shifteo de dos bits a cada WORD no va a afectar a lo que quiero
		; EJEMPLO: tengo |12|34|56|78| y necesito los bits 2 y 3 de cada byte
		;		entonces veo esto en binario:
		;		|12|34|56|78| = |0001 0010|0011 0100|0101 0110|0111 1000|
		; 		y necesito -->        ^^        ^^        ^^        ^^
		;		ahora shifteo 2 bits (de a WORDS):
		;		|0000 0100 1000 1101|0001 0101 1001 1110|		;_FIXME
		;				^^        ^^        ^^        ^^
		;		obtuve lo que queria, y el resto no me importa porque se limpiara
		; Ahora voy a usar ese concepto
		;USO: PSRLW - Shift Packed Data Right Logical
		psRlw xmm2, 2

		;** QUIERO: hacer un XOR entre xmm2 y xmm0
		pxor xmm0, xmm2
		
		;copio xmm0
		movdqa xmm1, xmm0
		
		;*** QUIERO: limpiar todos los bits que no me interesan (0000 00xx)
		;hago un AND con 0x03 en cada componente
		movdqa xmm2, r_mask_byte_03
		pand xmm1, xmm2
		; xmm1 = |((src_mirror[i] >> 2)^src_matrix[i]) & 0x3|...| con i=[0...3]
		;o visto de a bits:
		; xmm1 = |0000 00(i_b1)(i_b0)|0000 00(i_g1)(i_g0)|0000 00(i_r1)(i_r0)|xx|...| con i=[0...3]
		; donde i va a ser el pixel correspondiente
		; y donde (i_b1) y (i_b0) van a ser el bit 1 y 0 de la componente
		; y xx es la componente alpha que no nos va a importar lo que quede ahi
		;****-------------------------------------------------------------------
		
		
		
		;AHORA voy a cambiar la vista anterior para poder trabajar mas comodo
		;ya que ahora vamos a necesitar reconstruir el valor de 8 bits que
		;representa al pixel original en blanco y negro
		
		;el orden de los bits para el pixel en blanco y negro debera ser este:
		; 	|(i_b0)(i_g0)(i_r0)(i_b1)(i_g1)(i_r1)00|
		;OBS: esto fue considerando el grafico del pdf de la seccion Ocultar
		
		;por esta razon usaremos la siguiente notacion (haciendo abuso de notacion de xmm)
		; xmm1 = |0000 00(bit4)(bit7)|0000 00(bit3)(bit6)|0000 00(bit2)(bit5)|xx|...|
		
		
		
		
		;**** QUIERO: reconstruir la componente en blanco y negro---------------
		;* QUIERO: separar a los bits 4, 3 y 2 a un reg
		; 			y a los otros 3 a otro reg shifteados un bit a der
		movdqa xmm2, xmm1
		
		;primero limpio los 7 bits mas significativos de cada componente en xmm1
		movdqa xmm3, r_mask_byte_01
		pand xmm1, xmm3
		; xmm1 = |0000 000(bit7)|0000 000(bit6)|0000 000(bit5)|0000 0000|...|
		
		;ahora me ocupo del otro registro
		; uso la misma estrategia para shiftear de a WORDS
		; sin que me importe que sean BYTES
		psRlw xmm2, 1
		;y por las dudas limpio los 7 bits mas significativos
		pand xmm2, xmm3
		; xmm2 = |0000 000(bit4)|0000 000(bit3)|0000 000(bit2)|0000 0000|...|
		
		;** QUIERO: unirlos de esta forma:
		;			|(bit7)(bit6)(bit5)(bit4)(bit3)(bit2)00|...|
		;ESTRATEGIA:
		; 1) guardo copia de xmm1 en xmm7
		; xmm7 = |0000 000(bit7)|0000 000(bit6)|0000 000(bit5)|0000 0000|...|
		; 2)	  ^--------/  (shifteo 7 bits a izq)
		; xmm7 = |(bit7)000 0000|(bit6)000 0000|(bit5)000 0000|0000 0000|...|
		; 3) y ahora borro todos los demas bytes que no tenga al byte 7
		; xmm7 = |(bit7)000 0000|0000 0000|0000 0000|0000 0000|...|
		;
		; 4) guardo copia de xmm1 en xmm6
		; xmm6 = |0000 000(bit7)|0000 000(bit6)|0000 000(bit5)|0000 0000|...|
		; 4a)            ^        \___________/
		;				  \------------/	(shifteo 1 BYTE a der)
		;						(porque mi representacion es en little endian)
		; 4b) repito estrategia de pasos 2 y 3 para los (bit6)
		; 5) Repito estrategia de paso 4 para los (bit5)
		; 6) Procedo de la misma forma (desde el paso 1) para el reg xmm2
		; 7) Mergeo todos los regs
		
		;Mascara a usar:
		movdqa xmm8, r_mask_only_bit_i
		;OBS: esta mascara se va a shiftear segun el bit que necesite,
		;	  se puede ver que ya esta inicializada para el bit7 
		; xmm8 = |80|00|00|00| = |1000 0000|00|00|00|
		
		;PROC para los (bit7):
		movdqa xmm7, xmm1
		psLlw xmm7, 7
		pand xmm7, xmm8
		; xmm7 = |(bit7)000 0000|00|00|00|(bit7)000 0000|00|00|00|...|
		
		;PROC para los (bit6):
		movdqa xmm6, xmm1
		psRldq xmm6, 1
		psLlw xmm6, 6
		
		psRlw xmm8, 1
		; xmm8 = |40|00|00|00| = |0100 0000|00|00|00|
		pand xmm6, xmm8
		; xmm6 = |0(bit6)00 0000|00|00|00|0(bit6)00 0000|00|00|00|...|
		
		;PROC para los (bit5):
		movdqa xmm5, xmm1
		psRldq xmm5, 2
		psLlw xmm5, 5
		
		psRlw xmm8, 1
		; xmm8 = |20|00|00|00| = |0010 0000|00|00|00|
		pand xmm5, xmm8
		; xmm5 = |00(bit5)0 0000|00|00|00|00(bit5)0 0000|00|00|00|...|
		
		;PASO a trabajar con xmm2 de manera analoga
		;PROC para los (bit4):
		movdqa xmm4, xmm2
		psLlw xmm4, 4
		
		psRlw xmm8, 1
		; xmm8 = |10|00|00|00| = |0001 0000|00|00|00|
		pand xmm4, xmm8
		; xmm4 = |000(bit4) 0000|00|00|00|000(bit4) 0000|00|00|00|...|
		
		;PROC para los (bit3):
		movdqa xmm3, xmm2
		psRldq xmm3, 1
		psLlw xmm3, 3
		
		psRlw xmm8, 1
		; xmm8 = |08|00|00|00| = |0000 1000|00|00|00|
		pand xmm3, xmm8
		; xmm3 = |0000 (bit3)000|00|00|00|0000 (bit3)000|00|00|00|...|
		
		;PROC para los (bit2):
		;movdqa xmm2, xmm2
		psRldq xmm2, 2
		psLlw xmm2, 2
		
		psRlw xmm8, 1
		; xmm8 = |04|00|00|00| = |0000 0100|00|00|00|
		pand xmm2, xmm8
		; xmm2 = |0000 0(bit2)00|00|00|00|0000 0(bit2)00|00|00|00|...|
		
		;-------------------------- RESUMEN --------------------------
		; xmm7 = |(bit7)000 0000|00|00|00|(bit7)000 0000|00|00|00|...|
		; xmm6 = |0(bit6)00 0000|00|00|00|0(bit6)00 0000|00|00|00|...|
		; xmm5 = |00(bit5)0 0000|00|00|00|00(bit5)0 0000|00|00|00|...|
		; xmm4 = |000(bit4) 0000|00|00|00|000(bit4) 0000|00|00|00|...|
		; xmm3 = |0000 (bit3)000|00|00|00|0000 (bit3)000|00|00|00|...|
		; xmm2 = |0000 0(bit2)00|00|00|00|0000 0(bit2)00|00|00|00|...|
		;-------------------------------------------------------------
		
		;*** QUIERO: mergear los registros
		por xmm2, xmm7
		por xmm2, xmm6
		por xmm2, xmm5
		por xmm2, xmm4
		por xmm2, xmm3
		; xmm2 = |(bit7)(bit6)(bit5)(bit4)(bit3)(bit2)00|...|
		;****-------------------------------------------------------------------
		
		; HASTA ACA ya tengo la componente en blanco y negro que buscaba
		
		;* QUIERO: pasar el resultado a las 3 componentes RGB y setear FF en alpha
		; NOTACION: (bit7)(bit6)(bit5)(bit4)(bit3)(bit2)00 = bwi
		;			con i = [0...3] correspondiente al pixel i
		
		; xmm2 = |bw0|xx|xx|xx|bw1|xx|xx|xx|bw2|xx|xx|xx|bw3|xx|xx|xx|
		;USO: PSHUFB - Packed Shuffle Bytes
		movdqa xmm1, r_mask_shuf_broadcast_BnW
		; xmm1 = |00|00|00|FF|04|04|04|FF|08|08|08|FF|0C|0C|0C|FF|
		pshufb xmm2, xmm1
		; xmm2 = |bw0|bw0|bw0|00|bw1|bw1|bw1|00|bw2|bw2|bw2|00|bw3|bw3|bw3|00|
		
		;seteo los alpha en FF
		movdqa xmm1, r_mask_byte_alpha
		por xmm2, xmm1
		;*----------------------------------------------------------------------
		
		; FINALMENTE vuelco en dst_ptr los pixeles obtenidos
		movdqa [dst_ptr], xmm2
		
		
		lea edx, [edx + 4]
		lea src_ptr, [src_ptr + 16]
		lea dst_ptr, [dst_ptr + 16]
		
		lea src_mirror_ptr, [src_mirror_ptr - 16]
		
		jmp .ciclo_col

.end_of_row:
	lea eax, [eax + 1]
	jmp .ciclo
	
.end:
	pop r12
	pop rbx
    pop rbp 
ret
