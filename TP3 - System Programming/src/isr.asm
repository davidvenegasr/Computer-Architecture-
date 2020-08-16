; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:     dd 0xFFFF
sched_task_selector:   dw 0xFFFFFFFF
copia_eax:             dd 0x00
copia_ebx:             dd 0x00

auxiliar_x:     dd 0x0
auxiliar_y:     dd 0x0

_eip:       dd 0x0
_cs:        dd 0x0
_eflags:    dd 0x0
_esp:       dd 0x0
_ss:        dd 0x0

;; PIC
extern pic_finish1

;; Sched
extern sched_nextTask
extern imprimir_excepcion
extern printScanCode
extern debugger

;;Rutinas
extern rutina_137
extern rutina_138
extern rutina_139

extern new_portal_available
extern game_updateScore
extern screen_drawTasks
extern screen_incTaskClocks
extern game_checkEndOfGame
extern debug_ON
extern screen_drawBrown
extern pintarDebugger

extern kill_task
extern has_errorcode
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

; Pila de lvl0 en una isr luego de hacer PUSHAD:
;   CON cambio de privilegio:
; - |  EDI   |  <- esp 
;   |  ESI   |  <- esp + 1*4
;   |  EBP   |  <- esp + 2*4
;   |  ESP*  |  <- esp + 3*4
;   |  EBX   |  <- esp + 4*4
;   |  EDX   |  <- esp + 5*4   
;   |  ECX   |  <- esp + 6*4
;   |  EAX   |  <- esp + 7*4
;   ----------
;   |  EIP   |  <- esp + 8*4
;   |  CS    |  <- esp + 8*4 + 1*4
;   |  EFLAGS|  <- esp + 8*4 + 2*4
;   |  ESP   |  <- esp + 8*4 + 3*4
;   |  SS    |  <- esp + 8*4 + 4*4
; + |  ...   |  <- ss0:esp0

%macro ISR 1
global _isr%1

_isr%1:
    mov [copia_eax], eax   ; me guardo eax en memoria
    mov [copia_ebx], ebx   ; me guardo ebx en memoria
    pushad
    
    mov eax, %1
    push eax
    call imprimir_excepcion
    add esp, 4
    cmp DWORD [debug_ON],1
    jne .debug_apagado

    .debugger_encendido:
        
        
        ;VERIFICO SI LA EXCEPCION TIENE ERRORCODE
        mov eax, %1
        push eax
        call has_errorcode
        add esp, 4
        cmp eax, 1
        jne .not_error_code
            mov ebx, 4
            jmp .continue
        .not_error_code:        
            mov ebx, 0
        .continue:
        
        ;OBTENGO eip
        mov eax,      [esp + ebx + 8*4]
        mov [_eip],   eax
        ;OBTENGO cs
        mov eax,      [esp + ebx + 8*4 + 1*4]
        mov [_cs],    eax
        ;OBTENGO eflags
        mov eax,      [esp + ebx + 8*4 + 2*4]
        mov [_eflags],eax
        ;OBTENGO esp lvl3
        mov eax,      [esp + ebx + 8*4 + 3*4]
        mov [_esp],   eax
        ;OBTENGO ss lvl3
        mov eax,      [esp + ebx + 8*4 + 4*4]
        mov [_ss],    eax
        
        
        ;GUARDO eflags
        mov eax, [_eflags]
        push eax
        
        ;GUARDO Control Registers           
        mov eax, cr4
        push eax               ; me guardo cr4
        mov eax, cr3
        push eax               ; me guardo cr3
        mov eax, cr2
        push eax               ; me guardo cr2
        push 0                 ; me guardo cr1
        mov eax, cr0           ; me guardo cr0
        push eax
        
        ;GUARDO ss
        mov eax, [_ss]          ; me guardo ss de lvl3
        push eax
        ;GUARDO resto de selectores de segmento
        push gs
        push fs
        push es
        push ds
        ;GUARDO cs de lvl3
        mov eax, [_cs]
        push eax
        
        ;GUARDO eip de lvl3
        mov eax, [_eip]
        push eax
        
        ;GUARDO esp de lvl3
        mov eax, [_esp]
        push eax
        
        push ebp 
        push edi   
        push esi
        push edx
        push ecx
        
        mov eax, [copia_ebx]        
        push eax
   
        mov eax, [copia_eax]        
        push eax
        push %1              ; paso el valor de la excepcion
   
        call pintarDebugger ; 22 variables
   
        add esp, 88 ; 22 *4 = 88 para arreglar el stack
   
        ; Hago Poll esperando que se presione 'Y'
        .esperar_Y:
          xor eax, eax
          in al, 0x60
          cmp al, 0x15
          jne .esperar_Y
    
    .debug_apagado:
    
    pushad
    call kill_task
    ; xchg bx, bx        ;MAGIC BREAKPOINT
    call screen_drawBrown
    call screen_drawTasks
    popad
    ; Saltar a la primera tarea: Idle
    ;   El indice de la tarea Idle esta definida en defines.h
    ;   GDT_IDX_TASK_IDLE = 14 = 0xE ; TI=0 ; RPL = 00 
    ;        => selector = (0xE<<3) || 000 = 0x70
    jmp 0x0070:0x0
    
    ; SI es que hubo algun error se va a quedar ciclando infinitamente
    jmp $

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32

_isr32:
    pushad
    ; avisamos al pic que se recibe la interrupcion
    call pic_finish1
    
    ; reseteamos el control para que se pueda volver a crear portales
    mov DWORD [new_portal_available], 1

    ; update score
    ;xchg bx,bx
    call game_updateScore
    
    
    ; Imprimir reloj
    call nextClock
    
    ;; imprimir relojes en pantalla
    call screen_incTaskClocks
    ;; imprimir Tareas
    call screen_drawTasks
    ;; check EndOfGame
    call game_checkEndOfGame
    
    ; Obtengo el descriptor de segmento de la siguiente tarea
    call sched_nextTask
    
    ; Verifico que el nextTask no es igual al task actual
    str bx
    cmp ax, bx
    je .fin
    
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    mov WORD [sched_task_selector], ax
    jmp FAR [sched_task_offset]
    
.fin:
    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    pushad

    in al, 0x60
    
    push eax
    call printScanCode
    call debugger
    add esp, 4

    call pic_finish1
    popad
    iret
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr137
_isr137:
    ; in EAX=x          Desplazamiento en x
    ; in EBX=y          Desplazamiento en y
    ; in ECX=cross      0: No cruzar, 1: Cruzar
    ; in EDX=withMorty  0: Sin Morty, 1: Con Morty
    push ebp
    push edi
    push esi
    
    ;RUTINA en C: uint32_t rutina_137(int32_t x, int32_t y, uint32_t cross, uint32_t withMorty)
    push edx
    push ecx
    push ebx
    push eax
    
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    call rutina_137
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    add esp, 16
    
    pop esi
    pop edi
    pop ebp
    ; out EAX=worked    0: No creado, 1: Creado
    iret

global _isr138
_isr138:
    pushad
    ; in AX=code    Código de identificación de universos.
    ;               code=0xC137 para Rick C-137
    ;               code=0xD248 para Rick D-248
    
    push eax
    call rutina_138
    add esp, 4
    
    popad
    iret

global _isr139
_isr139:    
    push ebp
    push edi
    push esi
    
    ;RUTINA en C: void rutina_139(int *x, int *y);
    push auxiliar_y
    push auxiliar_x
    ;push esp ; PARA GENERAR EXCEPTION
    ;push esp
    call rutina_139
    add esp, 8
    mov eax, [auxiliar_x];
    mov ebx, [auxiliar_y];
    ;xchg bx, bx        ;MAGIC BREAKPOINT
    pop esi
    pop edi
    pop ebp
    ; out EAX=x     Desplazamiento entre Rick y Morty en x
    ; out EBX=y     Desplazamiento entre Rick y Morty en y
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
nextClock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret

