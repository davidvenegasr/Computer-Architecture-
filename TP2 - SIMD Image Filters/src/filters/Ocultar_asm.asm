global Ocultar_asm

section .rodata
ALIGN 16
						;		b   g      r   a 
mask_word_green: 	times 2 DW 0x0,0xFFFF,0x0,0x0
mask_word_alpha:	times 2 DW 0x0,0x0,0x0,0xFFFF

mask_shuf_b: DB 0x00,0xFF,0xFF,0xFF,0x02,0xFF,0xFF,0xFF,0x04,0xFF,0xFF,0xFF,0x06,0xFF,0xFF,0xFF
mask_shuf_g: DB 0xFF,0x00,0xFF,0xFF,0xFF,0x02,0xFF,0xFF,0xFF,0x04,0xFF,0xFF,0xFF,0x06,0xFF,0xFF
mask_shuf_r: DB 0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x02,0xFF,0xFF,0xFF,0x04,0xFF,0xFF,0xFF,0x06,0xFF

mask_word_0001: times 8 DW 0x0001

mask_byte_03: times 4 DB 0x3,0x3,0x3,0x00

mask_test: times 2 DD 1,2


; ------------MASCARAS PARA PROBAR LA IMAGEN EN BLANCO Y NEGRO------------------
; mask_alpha:		times 4 DB 0x0,0x0,0x0,0xFF
; mask_black_white: DB 0x00,0x00,0x00,0xFF,0x01,0x01,0x01,0xFF,0x02,0x02,0x02,0xFF,0x03,0x03,0x03,0xFF
; ------------------------------------------------------------------------------

;void Ocultar(
;	uint8_t *src,
;	uint8_t *src2,
;	uint8_t *dst,
;	int width,
;	int height,
;	int src_row_size,
;	int dst_row_size)
;	);

;Parametros:
;	rdi 		<--- uint8_t *src
;	rsi 		<--- uint8_t *src2
;	rdx 		<--- uint8_t *dst
;	ecx 		<--- int width,
; 	r8d 		<--- int height,
; 	r9d 		<--- int src_row_size,
; 	[rbp + 16]	<--- int dst_row_size)
section .text
Ocultar_asm:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	
	;paso dst_row_size a un reg
	mov r10d, [rbp + 16]
	
	;paso dst_ptr a un reg no volatil
	mov rbx, rdx
	
	%define src_ptr 		rdi
	%define src2_ptr 		rsi
	%define dst_ptr 		rbx
	%define cols			ecx
	%define rows			r8d
	%define src_row_size	r9d
	%define dst_row_size	r10d
	
	%define src_mirror_ptr	r12
	
	;-------------------------- RENOMBRE DE MASCARAS ---------------------------
	movdqa xmm9,  [mask_word_green]
	movdqa xmm10, [mask_word_alpha]
	movdqa xmm11, [mask_shuf_b]
	movdqa xmm12, [mask_shuf_g]
	movdqa xmm13, [mask_shuf_r]
	movdqa xmm14, [mask_word_0001]
	movdqa xmm15, [mask_byte_03]
	
	%define r_mask_word_green		xmm9
	%define r_mask_word_alpha		xmm10
	%define r_mask_shuf_b			xmm11
	%define r_mask_shuf_g			xmm12
	%define r_mask_shuf_r			xmm13
	%define r_mask_word_0001		xmm14
	%define r_mask_byte_03			xmm15
	;---------------------------------------------------------------------------
	
	movdqu xmm7, [mask_test]
	
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
	
	mul edx				;n*m
	movsxd rax, eax
	lea src_mirror_ptr, [src_ptr + rax*4 - 16]
	;*--------------------------------------------------------------------------
	
	;inicializo variables
	xor eax, eax	; i = 0
	
.ciclo:
	xor r11d, r11d	; j = 0
	cmp eax, rows	; while (i < rows)
	jge .end
.ciclo_col:	
	cmp r11d, cols	; while (j < cols)
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
		pshufd xmm8, xmm1, 0x1B	;00 01 10 11
		; xmm8 = |b3|g3|r3|a3|b2|g2|r2|a2|b1|g1|r1|a1|b0|g0|r0|a0| (src_mirror)
		;*----------------------------------------------------------------------
		
		
		;* QUIERO: levantar 4 pixeles de memoria de src2_ptr--------------------
		movdqa xmm1, [src2_ptr]
		; xmm1 = |b0|g0|r0|a0|b1|g1|r1|a1|b2|g2|r2|a2|b3|g3|r3|a3| (src2)
		;-----------------------------------------------------------------------
		
		
		;----AHORA me enfoco en src2 solamente----
		
		;***** QUIERO: (src.b + 2*src.g + src.r)/4 de cada pixel de xmm1--------
		
		;* QUIERO: 2*src.g
		;primero extiendo a WORDS xq las operaciones a realizar
		;pueden necesitar mas espacio
		;USO: PMOVZX - Packed Move with Zero Extend
		pmovzxbw xmm2, xmm1
		; xmm2 = |b0|g0|r0|a0|b1|g1|r1|a1|
		
		;shifteo a derecha para obtener la parte alta de xmm1
		psRldq xmm1, 8
		; xmm1 = |b2|g2|r2|a2|b3|g3|r3|a3|00|00|00|00|00|00|00|00|
		
		pmovzxbw xmm3, xmm1
		; xmm3 = |b2|g2|r2|a2|b3|g3|r3|a3|
		
		;guardo xmm2 y xmm3
		movdqa xmm4, xmm2
		movdqa xmm5, xmm3
		
		;tomo solo los g de ambos regs
		movdqa xmm1, r_mask_word_green
		pand xmm4, xmm1
		pand xmm5, xmm1
		; xmm4 = |00|g0|00|00|00|g1|00|00|
		; xmm5 = |00|g2|00|00|00|g3|00|00|
		
		;shifteo 1 bit a izq para multiplicar por 2
		psLlw xmm4, 1
		psLlw xmm5, 1
		; xmm4 = |00|2*g0|00|00|00|2*g1|00|00|
		; xmm5 = |00|2*g2|00|00|00|2*g3|00|00|
		
		;ahora mergeo los registros
		;uso PBLENDW - Blend Packed Words
		; xmm2 = |b0| g0 |r0|a0|b1| g1 |r1|a1|	(dst)
		; xmm4 = |00|2*g0|00|00|00|2*g1|00|00|	(src)
		;         0   1   0  0  0   1   0  0
		pblendw xmm2, xmm4, 0x22 	;00100010
		pblendw xmm3, xmm5, 0x22
		
		; xmm2 = |b0|2*g0|r0|a0|b1|2*g1|r1|a1|
		; xmm3 = |b2|2*g2|r2|a2|b3|2*g3|r3|a3|			
		
		
		;** QUIERO: setear a los alpha a 0 para poder hacer una suma horizontal
		;primero copio la mascara de alpha
		movdqa xmm1, r_mask_word_alpha
		movdqa xmm5, xmm1
		; xmm1 		= |0000|0000|0000|FFFF|0000|0000|0000|FFFF|
		; not(xmm1) = |FFFF|FFFF|FFFF|0000|FFFF|FFFF|FFFF|0000|
		;ahora limpio las componentes alpha de xmm2 y xmm3
		pandn xmm1, xmm2
		pandn xmm5, xmm3
		; xmm1 = |b0|2*g0|r0|0000|b1|2*g1|r1|0000|
		; xmm5 = |b2|2*g2|r2|0000|b3|2*g3|r3|0000|


		;*** hago la suma horizontal de cada pixel
		;uso: PHADDSW - Packed Horizontal Add and Saturate
		phaddsw xmm1, xmm5
		; xmm1 = |b0+2*g0| r0 |b1+2*g1| r1 |b2+2*g2| r2 |b3+2*g3| r3 |
		phaddsw xmm1, xmm1
		; xmm1 = |b0+2*g0+r0|b1+2*g1+r1|b2+2*g2+r2|b3+2*g3+r3|
		;		 |b0+2*g0+r0|b1+2*g1+r1|b2+2*g2+r2|b3+2*g3+r3|

		;**** QUIERO: dividir a los resultados por 4
		psRlw xmm1, 2
		; xmm1 = |(b0+2*g0+r0)/4|(b1+2*g1+r1)/4|(b2+2*g2+r2)/4|(b3+2*g3+r3)/4|
		;		 |(b0+2*g0+r0)/4|(b1+2*g1+r1)/4|(b2+2*g2+r2)/4|(b3+2*g3+r3)/4|	
		;*****------------------------------------------------------------------


		; ;CODIGO PARA GUARDAR IMAGEN EN BLANCO Y NEGRO-------------------------
		; packuswb xmm1, xmm1
		; ;xmm1 = "times 4 |(b0+2*g0+r0)/4|(b1+2*g1+r1)/4|(b2+2*g2+r2)/4|(b3+2*g3+r3)/4|"
		; movdqa xmm2, [mask_black_white]
		; pshufb xmm1, xmm2 ; 00 00 00 FF 01 01 01 FF 02 02 02 FF 03 03 03 FF
		; por xmm1, [mask_alpha]
		; movdqa [dst_ptr], xmm1
		; ;---------------------------------------------------------------------
		
		
		;----------------------------RESUMEN:----------------------------
		; xmm1 = "times 2 |(b0+2*g0+r0)/4|(b1+2*g1+r1)/4|(b2+2*g2+r2)/4|(b3+2*g3+r3)/4|"
		; xmm1 tiene el pixel en escala de grises de los 4 pixeles levantados en WORDS
		;----------------------------------------------------------------
		
		
		;OBS: Dejo xmm1 en words para hacer uso del shifteo de SSE para words
		
		;***** QUIERO: Dejar cada pixel de la siguiente forma:------------------
		;						   7 6 5 4 3 2    1        0
		;				pixel_b = |x|x|x|x|x|x|pixel[4]|pixel[7]|
		;				pixel_g = |x|x|x|x|x|x|pixel[3]|pixel[6]|
		;				pixel_r = |x|x|x|x|x|x|pixel[2]|pixel[5]|
		
		;* copio xmm1 en 2 regs mas
		movdqa xmm3, xmm1
		movdqa xmm4, xmm1
		movdqa xmm5, xmm1
		;ahora xmm3 va a corresponder a lo que quiero en pixel_b, xmm4 a pixel_g y xmm5 a pixel_r
		
		;OBS: las representaciones seran de los 8 bits de un BYTE (esquivando el hecho de que son words en los xmm)
		; xmm3[8:0] = |p[7]|p[6]|p[5]|p[4]|p[3]|p[2]|p[1]|p[0]|
		; y abuso de la notacion ya que se que las operaciones que haga se van a hacer en los 4 pixeles del registro. Entonces eso va a ser equivalente a:
		; 	xmm3 = |p[7]|p[6]|p[5]|p[4]|p[3]|p[2]|p[1]|p[0]|
		
		;** QUIERO: Armar el formato de pixel_b
		; HINT: uso la linea 29 de Ocultar_c.c como guia:
		; uint8_t bitsB = (((color >> 4) & 0x1) << 1) | ((color >> 7) & 0x1)
		
		; xmm3 = |p[7]|x|x|p[4]|x|x|x|x| = |b0|x|x|b1|x|x|x|x|
		;guardo xmm3
		movdqa xmm2, xmm3
		;shifteo 4 bits a la derecha
		psRlw xmm3, 4
		; xmm3 = |0|0|0|0|b0|x|x|b1|
		;hago un AND con 0x0001
		movdqa xmm1, r_mask_word_0001
		pand xmm3, xmm1
		; xmm3 = |0|0|0|0|0|0|0|b1|
		;shifteo 1 bit a la izquierda
		psLlw xmm3, 1
		; xmm3 = |0|0|0|0|0|0|b1|0|
		
		;ahora preparo xmm2
		;shifteo 7 bits
		psRlw xmm2, 7
		;hago un AND con 0x0001
		pand xmm2, xmm1
		; xmm2 = |0|0|0|0|0|0|0|b0|
		
		;hago un OR entre los dos regs
		por xmm3, xmm2
		; xmm3 = |0|0|0|0|0|0|b1|b0|
		
		;*** QUIERO: Armar el formato de pixel_g
		; HINT: uso la linea 30 de Ocultar_c.c como guia:
		; uint8_t bitsG = (((color >> 3) & 0x1) << 1) | ((color >> 6) & 0x1)
		;OBS: repito la misma logica que en el anterior pixel
		
		; xmm4 = |x|p[6]|x|x|p[3]|x|x|x| = |x|g0|x|x|g1|x|x|x|
		movdqa xmm2, xmm4
		psRlw xmm4, 3
		pand xmm4, xmm1
		psLlw xmm4, 1
		
		psRlw xmm2, 6
		pand xmm2, xmm1
		por xmm4, xmm2
		; xmm4 = |0|0|0|0|0|0|g1|g0|
		
		;**** QUIERO: Armar el formato de pixel_r
		; HINT: uso la linea 31 de Ocultar_c.c como guia:
		; uint8_t bitsR = (((color >> 2) & 0x1) << 1) | ((color >> 5) & 0x1)
		;OBS: repito la misma logica que en el anterior pixel
		
		; xmm5 = |x|x|p[5]|x|x|p[2]|x|x| = |x|x|g0|x|x|g1|x|x|
		movdqa xmm2, xmm5
		psRlw xmm5, 2
		pand xmm5, xmm1
		psLlw xmm5, 1
		
		psrlw xmm2, 5
		pand xmm2, xmm1
		por xmm5, xmm2
		; xmm4 = |0|0|0|0|0|0|r1|r0|
		;*****------------------------------------------------------------------
		
		
		;----------------------------RESUMEN:----------------------------
		; (representacin de 8 bits de las componentes)
		; xmm3 = |0|0|0|0|0|0|b1|b0| 
		; xmm4 = |0|0|0|0|0|0|g1|g0|
		; xmm5 = |0|0|0|0|0|0|r1|r0|
		;----------------------------------------------------------------
		
		
		;OBS: cambio de notacion
		; vuelvo a la representacion de BYTES considerando little endian y que
		; ahora el BYTE |0|0|0|0|0|0|X1|X2| sera el BYTE |X_i| del iesimo pixel
		; del registro xmm de color X.
		;		  0   1  2   3  4   5  6   7  ...
		; xmm3 = |b_0|xx|b_1|xx|b_2|xx|b_3|xx|b_0|xx|b_1|xx|b_2|xx|b_3|xx|
		; xmm4 = |g_0|xx|g_1|xx|g_2|xx|g_3|xx|g_0|xx|g_1|xx|g_2|xx|g_3|xx|
		; xmm5 = |r_0|xx|r_1|xx|r_2|xx|r_3|xx|r_0|xx|r_1|xx|r_2|xx|r_3|xx|
		;		 |...........................|...........................|
		;				lo que me importa	 |		 Repeticion
		
		;* QUIERO: Reordenarlos a esta forma en un solo registro:---------------
		;	|b_0|g_0|r_0|00|b_1|g_1|r_1|00|b_2|g_2|r_2|00|b_3|g_3|r_3|00|
		
		;USO: PSHUFB — Packed Shuffle Bytes
		; mask_shuf_b = 00,FF,FF,FF,02,FF,FF,FF,04,FF,FF,FF,06,FF,FF,FF
		movdqa xmm1, r_mask_shuf_b
		pshufb xmm3, xmm1
		; xmm3 = |b_0|00|00|00|b_1|00|0|00|b_2|00|0|00|b_3|00|0|00|
		
		;mask_shuf_g = FF,00,FF,FF,FF,02,FF,FF,FF,04,FF,FF,FF,06,FF,FF
		movdqa xmm1, r_mask_shuf_g
		pshufb xmm4, xmm1
		; xmm4 = |00|g_0|00|00|00|g_1|00|00|00|g_2|00|00|00|g_3|00|00|
		
		;mask_shuf_r = FF,FF,00,FF,FF,FF,02,FF,FF,FF,04,FF,FF,FF,06,FF
		movdqa xmm1, r_mask_shuf_r
		pshufb xmm5, xmm1
		; xmm5 = |00|00|r_0|00|00|00|r_1|00|00|00|r_2|00|00|00|r_3|00|
		
		;uno los resultados obtenidos
		por xmm3, xmm4
		; xmm3 = |b_0|g_0|00|00|b_1|g_1|00|00|b_2|g_2|00|00|b_3|g_3|00|00|
		por xmm3, xmm5
		; xmm3 = |b_0|g_0|r_0|00|b_1|g_1|r_1|00|b_2|g_2|r_2|00|b_3|g_3|r_3|00|
		;*----------------------------------------------------------------------
		
		
		;----------------------------RESUMEN:----------------------------
		; xmm0 = |b0|g0|r0|a0|b1|g1|r1|a1|b2|g2|r2|a2|b3|g3|r3|a3| (src)
		; xmm3 = |b_0|g_0|r_0|00|b_1|g_1|r_1|00|b_2|g_2|r_2|00|b_3|g_3|r_3|00| (src2)
		;----------------------------------------------------------------
		
		
		; AHORA paso a trabajar con el src(mirror)
		
		;* QUIERO: obtener |bi>>2 & 0x03|gi>>2 & 0x03|ri>>2 & 0x03|-------------
		; xmm1 va a guardar al src(mirror)
		movdqa xmm1, xmm8
		;voy a shiftear de a WORDS ya que no tengo forma de shiftear solo los bytes
		;puedo confiar de que el shifteo sera correcto ya que luego limpiare
		;lo que no necesito.
		psRlw xmm1, 2
		; xmm1 = |b0>>2|g0>>2|r0>>2|a0>>2|b1>>2|g1>>2|r1>>2|a1>>2|b2>>2|g2>>2|r2>>2|a2>>2|b3>>2|g3>>2|r3>>2|a3>>2|
		;ahora limpio lo que no me importa
		movdqa xmm2, r_mask_byte_03
		pand xmm1, xmm2
		; xmm1 = |bi>>2 & 0x03|gi>>2 & 0x03|ri>>2 & 0x03|00|...|
		;*----------------------------------------------------------------------
		
		;* QUIERO: hacer un XOR entre xmm1 y xmm3-------------------------------
		;me aseguro de que xmm3 no contenga basura
		pand xmm3, xmm2
		pxor xmm1, xmm3
		; xmm1 = |000000(grey_b_0)|000000(grey_r_0)|000000(grey_r_0)|00000000|...
		; donde grey_X_i es el byte del color X del iesimo pixel de la imagen oculta
		; en escala de grises
		;*----------------------------------------------------------------------
		
		;* QUIERO: mergear el src con xmm1 y volcar en memoria------------------
		;uso la mascara para limpiar el BYTE menos significativo
		; mask_byte_03 			= times 4 DB 0x03, 0x03, 0x03, 0x00
		; not(mask_byte_03) 	= times 4 DB 0xFC, 0xFC, 0xFC, 0xFF
		pandn xmm2, xmm0
		;por ultimo mergeo xmm1 con xmm2
		por xmm2, xmm1
		movdqa [dst_ptr], xmm2
		;*----------------------------------------------------------------------



		lea r11d, [r11d + 4]
		lea src_ptr, [src_ptr + 16]
		lea src2_ptr, [src2_ptr + 16]
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
