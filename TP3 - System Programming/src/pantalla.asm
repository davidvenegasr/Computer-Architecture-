; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; Subrutinas de Pantalla (nuestro)

BITS 32

section .text

;Valores
%define C_BLACK 	(0x00 << 12)    ;Color celeste verdoso windows 98 de fondo
%define C_GREEN     (0X02 << 12) 	;Verde (el shifteo es para mandarlo a los bits de background)
%define C_WHITE     (0x07 << 12) 	;Blanco
%define C_RED	    (0x04 << 12)	;Rojo
%define C_BLUE	    (0x01 << 12)	;Azul
%define C_LIGHTBROWN    (0xE << 12) ;Marron claro

;borra la pantalla escribiendo al registro de segmento fs asociado al area de la pantalla
limpiar_pantalla:
    pushad
    xor ecx, ecx
    .limpiar:
    mov word [fs:2*ecx], C_BLACK
    inc ecx
    cmp ecx, 4000
    jne .limpiar
    popad
    ret

pintar_pantalla:
    pushad
    mov ecx, 80
    .pintar:
    mov word [fs:2*ecx], C_LIGHTBROWN
    inc ecx
    cmp ecx, 3280
    jne .pintar

    mov ecx, 3443
    .salto1:
    mov word [fs:2*ecx], C_BLUE
    inc ecx
    cmp ecx, 3453
    jne .salto1

    mov ecx, 3523
    .salto2:
    mov word [fs:2*ecx], C_BLUE
    inc ecx
    cmp ecx, 3533
    jne .salto2
    
    mov ecx, 3603
    .salto3:
    mov word [fs:2*ecx], C_BLUE
    inc ecx
    cmp ecx, 3613
    jne .salto3
    
    mov ecx, 3507
    .salto4:
    mov word [fs:2*ecx], C_RED
    inc ecx
    cmp ecx, 3517
    jne .salto4

    mov ecx, 3587
    .salto5:
    mov word [fs:2*ecx], C_RED
    inc ecx
    cmp ecx, 3597
    jne .salto5
    
    mov ecx, 3667
    .salto6:
    mov word [fs:2*ecx], C_RED
    inc ecx
    cmp ecx, 3677
    jne .salto6

    popad
    ret