; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

global start
extern GDT_DESC
extern IDT_DESC
extern idt_init
extern pic_reset
extern pic_enable
extern mmu_initKernelDir
extern mmu_init
extern mmu_mapPage
extern mmu_unmapPage
extern mmu_nextFreeKernelPage   ;FIXME: solo para el Ej 5c)
extern mmu_initTaskDir          ;FIXME: solo para el Ej 5c)

extern tss_init
extern tss_init_idle

extern tss_init_others      ;FIXME: solo para testing de newTask
extern new_task             ;FIXME: solo para testing de newTask

extern game_init
extern sched_init

;|INDEX=8 |TI=0|RPL=0| = 0x8<<3 = 0000 0000 0100 0000b = 0040h      ;_FIXME : va en defines.h?
%define KERNEL_CS	0040h
;|INDEX=9 |TI=0|RPL=0| = 0x9<<3 = 0000 0000 0100 1000b = 0048h
%define USER_CS 	0048h
;|INDEX=10|TI=0|RPL=0| = 0xA<<3 = 0000 0000 0101 0000b = 0050h
%define KERNEL_DS	0050h
;|INDEX=11|TI=0|RPL=0| = 0xB<<3 = 0000 0000 0101 1000b = 0058h	
%define USER_DS 	0058h

%define KERNEL_VIDEO 0060h
;|INDEX=12|TI=0|RPL=0| = 0xC<<3 = 0000 0000 0110 0000b = 0060h	

;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

msj_libreta db      'LU: 43/18 y 783/18'
msj_libreta_len equ $ - msj_libreta

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:

    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0
    
    ; Habilitar A20
	call A20_disable
	call A20_check
	call A20_enable
	call A20_check
	
    ; Cargar la GDT
	lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax
    
    ; Saltar a modo protegido
	jmp KERNEL_CS:protected_mode

BITS 32
protected_mode:
; Establecer selectores de segmentos
    ; ds = selector de data kernel 
    ; 
    ; es = user code segment
    ; gs = user data segment
    ; fs = selector de segmento video
	mov ax, KERNEL_DS ; selector de segmento
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

; Establecer la base de la pila
    mov ebp, 0x27000
	mov esp, ebp        ; tope de la pila en la base de la pila.
    
; Imprimir mensaje de bienvenida

; Inicializar pantalla
    xor eax, eax            ;Limpio EAX _FIX ME creo que no es necesario limpiar
    mov ax, KERNEL_VIDEO    ;Muevo a AX un selector para el descriptor del segmento 12 (pantalla)
    mov fs, ax              ;Uso fx para el selector de la memoria de video
    call limpiar_pantalla
    call pintar_pantalla

; Inicializar el manejador de memoria
    call mmu_init
; Inicializar el directorio de paginas
    call mmu_initKernelDir
; Cargar directorio de paginas
    mov eax, 0x27000
    mov cr3, eax
   
; Activar paginacion
    mov eax,cr0
    or eax,0x80000000  ; seteamos el bit menos significativo en 1
    mov cr0,eax

; Libretas en pantalla (Punto 4c)
    print_text_pm msj_libreta, msj_libreta_len, 0x07, 49, 0
    ;xchg bx, bx ;MB

; ;+++++++++++++++++TEST de mmu_mapPage+++++++++++++++++
    ; ;void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint8_t rw, uint8_t u_s);
    ; push 0            ; u_s = Kernel -> 00
    ; push 1            ; w_r = Writable -> 01
    ; push 0x00400000
    ; push 0x0050E000
    ; mov eax, cr3
    ; push eax
    ; call mmu_mapPage
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    ; add esp, 4*4
    ; mov BYTE [0x0050E027], 0x01
    ; 
    ; push 0x0050E000
    ; mov eax, cr3
    ; push eax
    ; call mmu_unmapPage
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    ; ;+++++++++++++++++++++++++++++++++++++++++++++++++++++

; ;+++++++++++++++++TEST de mmu_initTaskDir+++++++++++++++++
    ; ;La idea de esta funcion es simular que hay una tarea en una
    ; ;   dir del kernel libre, y por otro lado pedir una pagina libre 
    ; ;   donde se va a "copiar" esta tarea simulada.
    ; 
    ; ;uint32_t mmu_initTaskDir(uint32_t taskDir, uint32_t taskPage)
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    ; push 0x400000       ;taskPage
    ; push 0x300000       ;taskDir ---;Dir mapeada en el area libre del kernel
    ; 
    ; call mmu_initTaskDir
    ; ; rax = newPD
    ; ;mov rbx, cr3
    ; mov cr3, eax
    ; 
    ; print_text_pm msj_libreta, 1, 0x44, 0, 0
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    ; mov ebx, 0x27000
    ; mov cr3, ebx
    ; 
    ; print_text_pm msj_libreta, 1, 0x22, 0, 2
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    ; ;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

; Inicializar tss
    call tss_init
    ; Inicializar tss de la tarea Idle
    call tss_init_idle

; Inicializar pantalla
    
; Inicializar el scheduler
    call sched_init
    ; Inicializar la IDT
    call idt_init
    ; Cargar IDT
    lidt [IDT_DESC]
    ;xor eax,eax
    ;xchg bx, bx		;MAGIC BREAKPOINT
    ;idiv eax

; Configurar controlador de interrupciones
    call pic_reset
    call pic_enable
    
; Cargar tarea inicial
    ;   El indice de la tarea inicial esta definida en defines.h
    ;   GDT_IDX_TASK_INIT = 13 = 0xD ; TI=0 ; RPL = 00 
    ;        => selector = (0xD<<3) || 000 = 0x68
    mov ax, 0x68
    ltr ax
    
; Inicializar el juego 
    call game_init
    ;xchg bx, bx        ;MAGIC BREAKPOINT    
; Habilitar interrupciones
    ;sti            ; NO habilitamos las interrupciones porque cada tarea lo va a tener activado por su cuenta
; Saltar a la primera tarea: Idle
    ;   El indice de la tarea Idle esta definida en defines.h
    ;   GDT_IDX_TASK_IDLE = 14 = 0xE ; TI=0 ; RPL = 00 
    ;        => selector = (0xE<<3) || 000 = 0x70
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    jmp 0x0070:0x0
    
    
;+++++++++++++++++TEST de new_task+++++++++++++++++++++++
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    ;call tss_init_others
    ;;void new_task(uint32_t task_dir, uint16_t task_idx_gdt){
    ;push 15         ;Rick C-137
    ;push 0x10000
    ;call new_task
    ;add esp, 0x8
    ;; saltamos a la tarea Rick C-137 modo de test 
    ;;  => selector = (0xF<<3) || 000 = 0x78
    ;jmp 0x0078:0x0
    ;++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
%include "pantalla.asm"
