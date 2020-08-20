global Zigzag_asm
section .rodata
ALIGN 16
white: times 4 dd 0xFFFFFFFF
half_white1: dd 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000
half_white2: dd 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF
alpha: times 4 dd 0xFF000000
cinco: times 4 dd 5.0
multiplicador:  times 8 dw 51
error: db 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
       db 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff

section .text
;void Zigzag_c(
;    uint8_t *src,
;    uint8_t *dst,
;    int width,
;    int height,
;    int src_row_size,
;    int dst_row_size)

;Por convenvion C me llega 
; rdi ---> src
; rsi ---> dest
; rdx ---> width 
; rcx ---> height
; r8 ---> src_row_size
; r9 ---> dst_row_size

Zigzag_asm:		;pasando a punto flotante

	push rbp
	mov rbp, rsp
	push r12
	push r13 

	%define src rdi
	mov r12, src
	%define dest rsi 
	mov r13, dest

	%define cont_f r9 
	mov r9, 2
	%define cols r10 
	mov cols, rdx

	%define filas r11 
	mov filas, rcx

	movdqa xmm8, [alpha]

	sub cols, 2
	sub filas, 2

	lea src, [src+r8*2] ; leemos dos pixeles a la izquierda de dest 
	lea dest, [dest+r8*2+8] 

	movdqu xmm15, [cinco] ;inicializamos para dividir mas adelante

	.ciclo_filas:

	; calculamos el modulo apara determinar el caso de cada fila
	xor rdx, rdx
	mov rax, cont_f
	cqo
	mov rcx, 4
	div rcx    
	; en rdx queda resto de la division
	mov rcx,2 

	.ciclo_cols:

		; revisamos condicionales
		cmp rdx, 1 ;si i%4 == 1 entonces srci,j-2
		je .caso1
		
		cmp rdx, 3 ;si i%4 == 3 entonces srci,j+2
		je .caso3

		jmp .caso2y4 ;si es par calculamos el promedio de 5 pixeles mas cercanos en fila
	   

		.caso1:
		mov rax, src
		jmp .iterar1y3

		.caso3:
		lea rax, [src+16]
		jmp .iterar1y3

		.caso2y4:
		mov rax, src
		movdqu xmm0, [rax]  ;|pixel4|p3|p2|p1|
		pmovzxbw xmm2, xmm0 ; |p2|p1| 
		psrldq xmm0, 4
		pmovzxbw xmm3, xmm0 ; |p3|p2| 
		psrldq xmm0, 4
		pmovzxbw xmm4, xmm0 ; |p4|p3| 

		add rax, 16
		movdqu xmm1, [rax]  ;|p8|p7|p6|p5|
		pmovzxbw xmm5, xmm1 ; |p6|p5|xmm2
		psrldq xmm1, 4
		pmovzxbw xmm6, xmm1 ; |p7|p6|
		psrldq xmm1, 4
		pmovzxbw xmm1, xmm1 ; |p8|p7|
		
		; Hasta ahora tenemos 
		; xmm2 |p2|p1| 
		; xmm3 |p3|p2|
		; xmm4 |p4|p3| 
		movdqu xmm7, xmm4   ;|p4|p3|
		psrldq xmm7, 8      ;|0|p4|
		movdqu xmm13, xmm5 
		pslldq xmm13, 8     ;|p5|0|
		por xmm7, xmm13
		; xmm7 |p5|p4|
		; xmm5 |p6|p5|
		; xmm6 |p7|p6|
		; xmm1 |p8|p7|
	

		paddusw xmm2, xmm3
		paddusw xmm2, xmm4
		paddusw xmm2, xmm7
		paddusw xmm2, xmm5   ;|p2+p3+p4+p5+p6|p1+p2+p3+p4+p5|

		movdqu xmm11, xmm4
		paddusw xmm11, xmm7
		paddusw xmm11, xmm5
		paddusw xmm11, xmm6
		paddusw xmm11, xmm1  ;|p4+p5+p6+p7+p8|p3+p4+p5+p6+p7|

		;primeros 2 pixeles para dividir (ya son el promedio) 
		pmovzxwd xmm10,xmm2  ;|p2|p1|
		psrldq xmm2, 8       ;|0 |p2|
		pmovzxwd xmm2, xmm2   

		cvtdq2ps xmm2, xmm2
		cvtdq2ps xmm10, xmm10       
		
		divps xmm2, xmm15
		divps xmm10, xmm15 ;xmm5 = |5.0|....|5.0|
		
		cvttps2dq xmm2, xmm2
		cvttps2dq xmm10, xmm10

		; segundos dos pixeles
		pmovzxwd xmm12,xmm11 ; |p4|p3|
		psrldq xmm11, 8
		pmovzxwd xmm11, xmm11 ;  

		cvtdq2ps xmm11, xmm11
		cvtdq2ps xmm12, xmm12       
		
		divps xmm11, xmm15
		divps xmm12, xmm15
		
		cvttps2dq xmm11, xmm11
		cvttps2dq xmm12, xmm12

		;empaquetamos ahora 
		
		packusdw xmm2, xmm2    ; | a | r | g | b | a | r | g | b |
		packuswb xmm2, xmm2    ; |a|r|g|b|a|r|g|b|a|r|g|b|a|r|g|b|
		packusdw xmm10, xmm10 
		packuswb xmm10, xmm10 
		psrldq xmm10, 12       ; |0|0|0|0|0|0|0|0|0|0|0|0|a|r|g|b|
		psrldq xmm2, 12        ; |0|0|0|0|0|0|0|0|   p2  |   p1  |
		pslldq xmm2, 4  
		por xmm2, xmm10 

		; empaquetamos segundos dos pixeles
		packusdw xmm11, xmm11    ; |a|r|g|b|a|r|g|b|
		packuswb xmm11, xmm11    ; |a|r|g|b|a|r|g|b|a|r|g|b|a|r|g|b|
		packusdw xmm12, xmm12 
		packuswb xmm12, xmm12 
		psrldq xmm12, 12
		psrldq xmm11, 12  
		pslldq xmm11, 4  
		por xmm11, xmm12 
		pslldq xmm11, 8          ; |   p4  |   p3  |   0   |   0   |

		;unimos los pixeles y nos quedna 4 ya modificados para guardar en memoria
		por xmm2, xmm11

		movdqu [dest], xmm2

		jmp .iterar2y4

		.iterar1y3:
		movdqu xmm0, [rax]
		por xmm0, xmm8
		movdqu [dest], xmm0 
		add src, 16
		add dest, 16
		add rcx, 4
		cmp rcx, cols
		jne .ciclo_cols
		jmp .iterar_columans

		.iterar2y4:
		add src, 16
		add dest, 16
		add rcx, 4
		cmp rcx, cols
		jne .ciclo_cols

	.iterar_columans:
	inc cont_f
	
	add src, 16
	add dest, 16

	cmp cont_f, filas
	jne .ciclo_filas    
  
	;agregamos bordes blancos a la imagen 
	.fin:
	mov rdi, r13
	add filas, 2
	mov rsi, filas
	add cols, 2 
	mov rdx, cols
	mov rcx, r8
	call bordes_blancos
	pop r13
	pop r12
	pop rbp
	ret

bordes_blancos:
    ; rdi ---> dest
    ; rsi ---> height 
    ; rdx ---> width
    ; rcx ---> row_size

    push rbp
    mov rbp, rsp
    lea rbx, [rdi+8*rdx]
    movdqa xmm0, [white]
    movdqa xmm1, [half_white1]
    movdqa xmm2, [half_white2]

    sub rsi, 4
    mov rax, rsi ; para multiplciar posicion donde empieza borde inferior 

    .ciclo_iterar_en_cols:
    movdqa [rdi], xmm0
    add rdi, 16
    cmp rdi, rbx
    jne .ciclo_iterar_en_cols

    mov r9, rdx ; me guardo rdx en r9 para que mul no lo afecte
    mul rdx
    lea rbx, [rdi+rax*4]

    .ciclo_iterar_en_filas:
    movdqa xmm4, [rdi]
    por xmm4, xmm1
    movdqa [rdi], xmm4
    lea rdi, [rdi+4*r9-16]
    movdqa xmm4, [rdi]    
    por xmm4, xmm2
    movdqa [rdi], xmm4    
    add rdi, 16 
    cmp rdi, rbx
    jne .ciclo_iterar_en_filas

    lea rbx, [rdi+8*r9]

    .ciclo_iterar_en_cols2:
    movdqa [rdi], xmm0
    add rdi, 16
    cmp rdi, rbx
    jne .ciclo_iterar_en_cols2

    pop rbp
    ret
