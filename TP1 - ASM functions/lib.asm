
section .data
NULL: DB "NULL", 0
string: DB "%s", 0
pointer: DB "%p", 0

sorter_format_str: DB "%hu = ",0
new_line: DB 10, 0

listaInicio: DB "[",0
coma: DB ",",0
listaFin: DB "]",0

section .rodata:
;list

;list_t
%define off_first 0
%define off_last 8
%define size_list 16

;listElem_t
%define size_listElem 24

%define off_data 0
%define off_next 8
%define off_prev 16

%define size_sorter 32
%define off_size 0
%define off_sorter 8
%define off_compare 16
%define off_slots 24

%define NULL_ptr 0

section .text

global strLen
global strClone
global strCmp
global strConcat
global strDelete
global strPrint
global listNew
global listAddFirst
global listAddLast
global listAdd
global listDelete
global listPrint
global listClone
global sorterNew
global sorterAdd
global sorterRemove
global sorterGetSlot
global sorterGetConcatSlot
global strConcatNoFree
global sorterCleanSlot
global sorterDelete
global sorterPrint
global fs_sizeModFive
global fs_firstChar
global fs_bitSplit

extern malloc
extern free
extern fprintf
extern listRemoveFirst
extern listRemove


;*** String ***
;char* strClone(char* a)(19 Inst.)
;uint32_t strLen(char* a)(7 Inst.)
;int32_t strCmp(char* a, char* b)(25 Inst.)
;char* strConcat(char* a, char* b)(42 Inst.)
;void strDelete(char* a)(1 Inst.)
;void strPrint(char* a, FILE *pFile)(7 Inst.)

strClone:
	%define string_inicial r12
	%define string_copia r8
    ;armo el stack frame
	push rbp
	mov rbp, rsp
	push r12
	sub rsp, 0x08

	;se recibe le puntero en RDI ;llamo strLen
	mov string_inicial, rdi
	call strLen

	;libero memoria con malloc
	mov edi, eax 	; pasa el tamano 
	inc edi      	; contamos el cero al final
	call malloc
	mov r8, rax
	.avanzar: 		;loop para recorrer string
		cmp byte [string_inicial], 0
		je .fin
		mov dl, byte [string_inicial]
		mov byte [string_copia], dl
		inc string_inicial
		inc string_copia
		jmp .avanzar

	.fin:
	 	mov byte [string_copia], 0
		add rsp, 0X08
		pop r12
		pop rbp
		ret

strLen:
	push rbp 	;armo el stack frame
	mov rbp, rsp
	xor eax, eax
	.avanzar:		;loop para ver si llegue al fin de un string
		cmp byte [rdi], 0
		jz .fin
		inc rdi
		inc eax
		jmp .avanzar
	.fin:
		pop rbp
		ret


strCmp:
	; en rdi recibimos puntero a char1
	; en rsi recibimos puntero a char2
	push rbp
	mov rbp, rsp

	.loop:
		mov cl, [rdi]
		mov dl, [rsi]
		mov eax, 0    	; en el caso de que sean iguales retornaremos cero por defecto

		cmp cl, 0 		; revisamo si el primer char es nulo
		jne .notNull 	; si lo es vamos a querer ver mas abajo si el char 2 es cero tambien
						;	sino debe ser mayor ya que los chars no tienen signo
		cmp dl, 0
		je .fin 		; Si ambos son 0 son iguales
		jmp .b_mayor 	; Si a es 0 y b no lo es, entonces a < b
		.notNull:
			cmp dl, 0
			je .a_mayor ; Si b es cero y a no, entonces a > b

		cmp cl, dl 		; sino se compara byte a byte
		jg .a_mayor
		jl .b_mayor

		inc rdi 		;incrementamos los punteros
		inc rsi
		jmp .loop

	; Seteamos valores segun las comparaciones
	.a_mayor:
		mov eax, -1
		jmp .fin

	.b_mayor:
		mov eax, 1

	.fin:
		pop rbp
		ret


strConcat:
	push rbp 	;armo el stack frame
	mov rbp, rsp
	push r12
	push r13 ; stringA
	push r14 ; stringB
	sub rsp, 0X08

	mov r13, rdi
	mov r14, rsi

    call strLen     ;longitud del primer string
    mov ebx, eax
    mov rdi, r14
    call strLen     ;longitud del segundo string;
    add ebx, eax
    mov edi, ebx
    inc edi
    call malloc
	; ahora rax tiene el puntero a nuevo string para concatenar
	;limpio contadores
    xor r8, r8
    xor r9, r9
	.cicloA:		;loop para colocar stringA en nuevo string
		cmp byte [r13 + r8], 0
		jz .cicloB
		mov dl, byte [r13 + r8]
		mov byte [rax + r8], dl
		inc r8
		jmp .cicloA

	.cicloB: 		;loop para colocar stringB en nuevo string
		cmp byte [r14 + r9], 0
		jz .borrar
		mov dl, byte [r14 + r9]
		mov byte [rax + r8], dl
		inc r8
		inc r9
		jmp .cicloB

	.borrar:
		;agrego el cero al final
		mov byte [rax + r8], 0
		mov r12, rax
		;reviso si son el mismo puntero
		cmp r13, r14
    	je .igualitos

		;libero memoria
		mov rdi, r13
		call free
		mov rdi, r14
		call free
		jmp .final
	.igualitos:
		mov rdi, r13
		call free
	.final:
		add rsp, 0x08
		mov rax, r12
		pop r14
		pop r13
		pop r12
		pop rbp
		ret

strDelete:
	sub rsp, 0x08 ; Alineo la pila a 16 bytes
    call free
	add rsp, 0X08
    ret

    ;jump free

strPrint:
	; AUN NO ESTA LISTA
    ; fprintf(FILE *stream, const char *format, ...) OBS: formato es como para printf. Constante en section.data?
	; rdi viene string para imprimir
	; rsi puntero a archivo
	; parametros fprintf:
	;   - rdi = puntero stream
	;   - rsi = formato
	;   - rdx = texto

	; stackframe:
	push rbp
	mov rbp, rsp
	push r12
	sub rsp, 0x08
	;arreglo parametros funcion
	mov r12, rdi
	mov rdi, rsi
	mov rsi, string

	;vemos si null el string
	cmp byte [r12], 0
	jnz .not_empty
	mov rdx, NULL       ;if empty
	jmp .print

	.not_empty:
		mov rdx, r12 ; le paso el string

	.print:
		mov qword rax, 0
		call fprintf
		add rsp, 0x08
		pop r12
		pop rbp
		ret

;*** List ***

;list_t* listNew()
listNew:

    push rbp
    mov rbp, rsp
	mov rdi, size_list
	call malloc
	mov QWORD [rax+off_first], 0
	mov QWORD [rax+off_last], 0
	pop rbp
    ret

;void listAddFirst(list_t* l, void* data)
listAddFirst:
	; stack frame
 	push rbp
    mov rbp, rsp
    push r12
    push r13

    mov r12, rdi ;puntero a lista
    mov r13, rsi ;puntero a data
    mov rdi, size_listElem
    call malloc	 ; rax tiene el puntero a memoria
    cmp qword [r12], 0 ; Reviso si es vaciaList
    jne .no_vacia
    mov [r12+off_last], rax
    jmp .insertar_elem
    .no_vacia:
    mov rdx, [r12+off_first]
    mov [rdx+off_prev], rax
    .insertar_elem:
    mov [rax+off_data], r13
    mov rdx, [r12+off_first]
    mov qword [rax+off_next], rdx
    mov qword [rax+off_prev], 0
    mov qword [r12+off_first], rax
    pop r13
    pop r12
    pop rbp
    ret
;void listAddLast(list_t* l, void* data)
listAddLast:
	; stack frame
 	push rbp
    mov rbp, rsp
    push r12
    push r13

    mov r12, rdi ;puntero a lista
    mov r13, rsi ;puntero a data
    mov rdi, size_listElem
    call malloc	 ; rax tiene el puntero a memoria
    cmp qword [r12], 0 ; Reviso si es vaciaList
    jne .no_vacia
    mov [r12+off_first], rax
    jmp .insertar_elem
    .no_vacia:
    mov rdx, [r12+off_last]
    mov [rdx+off_next], rax
    .insertar_elem:
    mov [rax+off_data], r13
    mov rdx, [r12+off_last]
    mov qword [rax+off_prev], rdx
    mov qword [rax+off_next], 0
    mov qword [r12+off_last], rax

    pop r13
    pop r12
    pop rbp
    ret


;void listAdd(list_t* l, void* data, funcCmp_t* fc)
listAdd:
 	push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi ;puntero a lista
    mov r13, rsi ;puntero a data
    mov r14, rdx ;puntero a fc

    mov r15, [r12+off_first] ; primer nodo

    cmp r14, 0 ; reviso fc valida
    je .fin

    cmp r15, 0
    jne .ciclo

    call listAddLast ; ya tengo los paramentros en rdi,rsi porque no he hecho ningun call
    jmp .fin

    .ciclo:
    mov rdi, r13
    mov rsi, [r15+off_data]
    call r14				;comparamos actual

    ;revisamos el resultado de comparar
	cmp rax, 1
	jne .avanzar

	;reviso si es el primer nodo
	cmp qword [r15+off_prev], 0
	jne .insertar_entre_nodos

	mov rdi, r12
	mov rsi, r13
	call listAddFirst
	jmp .fin

	.insertar_entre_nodos:
		mov rdi, size_listElem
		call malloc 			; nuevo puntero
		mov rdx, [r15+off_prev] ; nodoAnterior

		mov [rdx+off_next], rax  ; nodoAnterior->next = nuevoNodo
		mov [rax+off_prev], rdx  ; nuevoNodo->prev = nodoAnterior
		mov [r15+off_prev], rax ; nodoActual->prev = nuevoNodo
		mov [rax+off_next], r15 ; nuevoNodo->next = nodoActual
		mov [rax+off_data], r13
		jmp .fin

	.avanzar:

	;reviso si es el ultimo nodo

	cmp qword [r15+off_next], 0
	jne .no_es_ultimo
	mov rdi, r12
	mov rsi, r13
	call listAddLast
	jmp .fin

	.no_es_ultimo:
	mov r15, [r15+off_next]
	jmp .ciclo

    .fin:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret


;list_t* listClone(list_t* l, funcDup_t* fn)
listClone:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	
	;list_t* listClone(list_t* l, funcDup_t* fn);
	
	mov r12, rdi ;puntero a lista
	mov r13, rsi ;puntero a func dup	(strClone)
	
	%define list_ptr			r12
	%define func_dup_ptr		r13
	%define current_elem_ptr	rbx
	
	%define new_list_ptr			r14
	
	;inicializo
	mov current_elem_ptr, [list_ptr + off_first]
	
	mov rdi, size_list
	call malloc
	mov new_list_ptr, rax
	
	mov QWORD [new_list_ptr + off_first], NULL_ptr
	mov QWORD [new_list_ptr + off_last], NULL_ptr
		
.ciclo:
	;while(actual != NULL)
	cmp current_elem_ptr, NULL_ptr
	je .end
		;
		;debo usar void listAddLast(list_t* l, void* data);
		;primero uso void* (funcDup_t)(void*);
		mov rdi, [current_elem_ptr + off_data]
		call func_dup_ptr	; _WARNING_
		
		mov rdi, new_list_ptr
		mov rsi, rax
		call listAddLast
		
		;
		;actual = actual->next
		mov current_elem_ptr, [current_elem_ptr + off_next]
		jmp .ciclo

.end:
	mov rax, new_list_ptr
	
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	ret


;void listDelete(list_t* l, funcDelete_t* fd)
listDelete:

	; stack frame
 	push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    sub rbp, 0x08

    mov r12, rdi ;puntero a lista
    mov r13, rsi ;puntero a func delete
    mov r14, [r12+off_first]

    ;if(l->first != NULL)
    .ciclo:
    cmp r14, 0
    je .fin

    mov r14, [r14+off_next] ;sig a borrar
    mov rdi, r12
    mov rsi, r13
    call listRemoveFirst
    jmp .ciclo

    .fin:
    mov rdi, r12
    call free

    add rbp, 0x08
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

;void listPrint(list_t* l, FILE *pFile, funcPrint_t* fp)
listPrint:
 	push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi ;puntero a lista
    mov r13, rsi ;puntero a stream
    mov r14, rdx ;puntero a funcion imprimir

	mov r15, [r12+off_first]  ; puntero a primer nodo

	mov rdi, r13
	mov rsi, listaInicio  ; Imprimir [
	mov qword rax, 0
	call fprintf

	.ciclo:
	cmp r15, 0
	je .fin

	;imprimo el valor del puntero a texto:
	cmp r14, 0 	; reviso funcion imprimir not null
	je .fin

	.imprimir:
	mov rdi, [r15+off_data]
	mov rsi, r13
	call r14

	.continuar:
	mov r15, [r15+off_next]
	CMP r15, 0
	je .fin
	mov rdi, r13
	mov rsi, coma   ; imprimir ,
	mov QWORD rax, 0
	call fprintf
	jmp .ciclo

	.fin:
	mov rdi, r13
	mov rsi, listaFin  ; Imprimir ]
	mov qword rax, 0
	call fprintf

	pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

;*** Sorter ***------------------------------------------------------
;%define off_size 0
;%define off_sorter 8
;%define off_compare 16
;%define off_slots 24
;%define size_sorter 32

;typedef struct s_sorter{
;    uint16_t size;
;    funcSorter_t *sorterFunction;
;    funcCmp_t *compareFunction;
;    list_t **slots;
;} sorter_t;


sorterNew:
	    push rbp
	    mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14

		;sorter_t* sorterNew(uint16_t slots, funcSorter_t* fs, funcCmp_t* fc)
		;	di	<- uint16_t slots	(unsigned short)
		;	rsi <- funcSorter_t* fs
		;	rdx <- funcCmp_t* fc

		;muevo a regs no volatiles
		mov bx, di
		mov r12, rsi	;<- temp
		mov r13, rdx	;<- temp

		%define cant_listas	bx
		%define sorter_ptr	r14
		

		mov rdi, size_sorter	;pido 32B para el sorter_ptr
		call malloc
		mov sorter_ptr, rax

		mov WORD [sorter_ptr + off_size], cant_listas
		mov QWORD [sorter_ptr + off_sorter], r12
		mov QWORD [sorter_ptr + off_compare], r13
		mov QWORD [sorter_ptr + off_slots], NULL_ptr

		%define slots_ptr	r12
		%define i			r13
		
		;inicializo
		xor i, i

		;pido memoria para slots_ptr
		;sabiendo que va a ser un arreglo de (cant_listas*sizeof(ptr))
		imul di, cant_listas, 8
		call malloc
		mov slots_ptr, rax
		mov [sorter_ptr + off_slots], slots_ptr

	.ciclo:
		;while (i < cant_listas)
		;agrego las listas vacias correspondientes
		cmp r13w, cant_listas
		je .end
			;llamo a list_t* listNew()
			call listNew

			;lo guardo en el arreglo
			mov QWORD [slots_ptr + i*8], rax

			inc i
			jmp .ciclo
		
	.end:
		mov rax, sorter_ptr
		
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

sorterAdd:
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14

		;void sorterAdd(sorter_t* sorter, void* data)
		; Usando la función sorterFunction determina el slot donde agregar
		; el elemento data. Una vez seleccionado, agrega de forma ordenada
		; en la lista apuntada por el correspondiente slot.
		; Si el elemento ya existe, entonces lo agrega nuevamente.
		;	rdi <- sorter_t* sorter
		;	rsi <- void* data

		; uso regs no volatiles
		mov rbx, rdi
		mov r12, rsi

		%define sorter_ptr	rbx
		%define data_ptr	r12
		%define slot_number	r13			;<-unsigned short
		%define slots_ptr	r14

		;obtengo el slot donde agregar data
		;uso typedef int32_t (funcSorter_t)(void*)
		;OBS: pregunte por slack y David me dijo que tome el output
		;	  de la funcion como si fuera un uint16_t
		mov rdi, data_ptr
		call [sorter_ptr + off_sorter]
		movzx slot_number, ax	;extiendo con 0 a ax

		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]

		;aca ya se que estoy en la lista adecuada
		;uso void listAdd(list_t* l, void* data, funcCmp_t* fc)
		mov rdi, [slots_ptr + slot_number*8]
		mov rsi, data_ptr
		mov rdx, [sorter_ptr + off_compare]
		call listAdd

	.end:
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

sorterRemove:
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14

		;void sorterRemove(sorter_t* sorter, void* data, funcDelete_t* fd)
		; Usando la función sorterFunction determina el slot de donde
		; borrar el elemento data. Una vez seleccionado, borra todas
		; las apariciones del elemnto en la lista apuntada por el
		; correspondiente slot.
		;	rsi <- sorter_t* sorter
		;	rdi <- void* data
		;	rdx <- funcDelete_t* fd

		;guardo en regs no volatiles
		mov rbx, rdi
		mov r12, rsi
		mov r13, rdx
		
		%define sorter_ptr		rbx
		%define data_ptr		r12
		%define funcDelete_ptr	r13

		%define slot_number		rdx
		%define slots_ptr		r14
		
		;obtengo el slot donde agregar data
		;uso typedef int32_t (funcSorter_t)(void*)
		;OBS: pregunte por slack y David me dijo que tome el output
		;	  de la funcion como si fuera un uint16_t
		mov rdi, data_ptr
		call [sorter_ptr + off_sorter]
		movzx slot_number, ax	;extendiendo con 0 ax

		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]

		;uso void listRemove(list_t* l, void* data, funcCmp_t* fc, funcDelete_t* fd)
		mov rdi, [slots_ptr + slot_number*8]
		mov rsi, data_ptr
		mov rdx, [sorter_ptr + off_compare]
		mov rcx, funcDelete_ptr
		call listRemove

		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

sorterGetSlot:
		push rbp
		mov rbp, rsp

		;list_t* sorterGetSlot(sorter_t* sorter, uint16_t slot, funcDup_t* fn)
		; Retorna una copia de de la lista almacenada en el indice slot.
		; Utilizando la función fn, duplica uno a uno los
		; elementos almacenados en la lista.
		;	rdi <- sorter_t* sorter
		;	si <- uint16_t slot
		;	rdx <- funcDup_t* fn

		;no necesito regs no volatiles
		
		;extiendo el reg si
		movzx rsi, si
		
		%define sorter_ptr		rdi
		%define slot_number		rsi			;<- unsigned short
		%define funcDup_ptr		rdx
		%define slots_ptr		rax
		
		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]

		;uso list_t* listClone(list_t* l, funcDup_t* fn);
		mov rdi, [slots_ptr + slot_number*8]
		mov rsi, funcDup_ptr
		call listClone
		;en rax esta la respuesta asi que no hago nada

		pop rbp
		ret

sorterGetConcatSlot:
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14

		;char* sorterGetConcatSlot(sorter_t* sorter, uint16_t slot)
		; Dado un slot, obtiene una string que concatena todos
		; los elementos de la lista de forma ordenada.
		;	rdi <- sorter_t* sorter
		;	si <- uint16_t slot

		;guardo en regs no volatiles
		mov rbx, rdi
		movzx r12, si

		%define sorter_ptr		rbx
		%define slot_number		r12		;<- unsigned short

		%define slots_ptr		r13
		%define i				rcx

		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]

		;defino la funcion: (va a estar debajo de esta)
		; char* listConcat(list_t* l)
		mov rdi, [slots_ptr + slot_number*8]
		call listConcat

		.end:
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

listConcat:
		push rbp
		mov rbp, rsp
		push r12
		push r13

		;char* listConcat(list_t* l)
		;	rdi <- list_t*l

		;no muevo nada a regs no volatiles

		%define list_ptr	 		rdi
		%define current_elem_ptr	r12
		%define string_accum_ptr	r13

		;inicializo
		mov current_elem_ptr, [list_ptr + off_first]
		
		;creo un espacio vacio para guardar el accum
		mov rdi, 1
		call malloc
		mov string_accum_ptr, rax
		mov BYTE [string_accum_ptr], NULL_ptr

	.ciclo:
		;while(actual != NULL)
		cmp current_elem_ptr, NULL_ptr
		je .end
			;--------------
			;---Voy a hacer una copia del actual
			;---y lo voy a concatenar con el accum
			;uso char* strClone(char* a);
			mov rdi, [current_elem_ptr + off_data]
			call strClone
			;ahora uso char* strConcat(char* a, char* b);
			;OBS: la func hace free de a y b
			mov rdi, string_accum_ptr
			mov rsi, rax
			call strConcat
			;por ultimo guardo lo concatenado en su registro
			mov string_accum_ptr, rax
			;--------------
			
			;actual = actual->next
			mov current_elem_ptr, [current_elem_ptr + off_next]
			jmp .ciclo
	.end:
		mov rax, string_accum_ptr
		
		pop r13
		pop r12
		pop rbp
		ret

sorterCleanSlot:
		push rbp
		mov rbp, rsp
		push rbx
		push r12

		;void sorterCleanSlot(sorter_t* sorter, uint16_t slot, funcDelete_t* fd)
		; Borra todos los elementos almacenados en slot.
		; Utiliza la función fd para borrar los datos dentro
		; de la lista.
		;	rdi <- sorter_t* sorter
		;	si <- uint16_t slot
		;	rdx <- funcDelete_t* fd
		
		;extiendo el reg si y paso a reg no volatil
		movzx rbx, si
		
		%define sorter_ptr		rdi
		%define slot_number		rbx		;<- unsigned short
		%define funcDelete_ptr	rdx
		%define slots_ptr		r12
		
		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]

		;uso void listDelete(list_t* l, funcDelete_t* fd)
		mov rdi, [slots_ptr + slot_number*8]
		mov rsi, funcDelete_ptr
		call listDelete
		
		call listNew
		mov [slots_ptr + slot_number*8], rax

		.end:
		pop r12
		pop rbx
		pop rbp
		ret

sorterDelete:		
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14

		;void sorterDelete(sorter_t* sorter, funcDelete_t* fd)
		; Borra todos los datos de una estructura sorter_t.
		;	rdi <- sorter_t* sorter
		;	rsi <- funcDelete_t* fd

		;guardo en regs no volatiles
		mov rbx, rdi
		mov r12, rsi
		
		%define sorter_ptr			rbx
		%define funcDelete			r12
		
		%define slots_ptr			r13
		%define i					r14
		
		;inicializo
		mov slots_ptr, [sorter_ptr + off_slots]
		xor i,i
		
	.array_delete_loop:
		;while (i < sorter->size)
		cmp r14w, [sorter_ptr + off_size]
		je .delete_slots_ptr
			cmp QWORD [slots_ptr + i*8], NULL_ptr
			je .empty_list
			
			;uso void listDelete(list_t* l, funcDelete_t* fd)
			; rdi = slots_ptr[i]
			mov rdi, [slots_ptr + i*8]
			mov rsi, funcDelete
			call listDelete
			
	.empty_list:		
			inc i
			jmp .array_delete_loop
		
	.delete_slots_ptr:
		;free(sorter->slots)
		mov rdi, slots_ptr
		call free
		
		;free(sorter)
		mov rdi, sorter_ptr
		call free
	.end:
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret
		
sorterPrint:
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14
		push r15
		sub rsp, 8
		
		;void sorterPrint(sorter_t* sorter, FILE *pFile, funcPrint_t* fp)
		; Escribe en el stream indicado por pFile el arreglo de listas 
		; almacenado en sorter. Las listas seran escritas en 
		; lineas distintas respetando el siguiente formato: 
		; i = [x_{0} ,...,x_{n−1} ], donde x {i} es el
		; resultado de escribir el i-ésimo dato.
		; Para cada dato llama a la función fp.
		;	rdi <- sorter_t* sorter
		;	rsi <- FILE *pFile
		;	rdx <- funcPrint_t* fp
		
		;muevo a regs no volatiles
		mov rbx, rdi
		mov r12, rsi
		mov r13, rdx
		
		%define sorter_ptr			rbx
		%define stream_ptr			r12
		%define func_print_ptr		r13
		
		%define	i			r14		;unsigned short
		%define slots_ptr	r15
		
		;inicializo
		xor i, i
		mov slots_ptr, [sorter_ptr + off_slots]
		
	.ciclo:
		; while (i < sorter->size)
		cmp r14w, [sorter_ptr + off_size]
		je .end
			;----quiero imprimir "i = [x,...,x]"
			;-primero imprimo "i = "
			; uso fprintf(FILE *stream, const char *format, ...)
			mov rdi, stream_ptr
			mov rsi, sorter_format_str	;"i = "
			mov rdx, i
			mov rax, 0
			call fprintf
			
			;--ahora quiero imprimir la lista
			; uso void listPrint(list_t* l, FILE *pFile, funcPrint_t* fp)
			mov rdi, [slots_ptr + i*8]
			mov rsi, stream_ptr
			mov rdx, func_print_ptr
			call listPrint
			
			;---por ultimo quiero el salto de linea
			mov rdi, stream_ptr
			mov rsi, new_line  			;"\n"
			mov rax, 0
			call fprintf
			
			inc i
			jmp .ciclo
		
	.end:
		add rsp, 8
		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret

;*** aux Functions ***

fs_sizeModFive:
	push rbp
	mov rbp, rsp
	call strLen
	xor dx, dx
	mov cx, 5
	div cx
	mov ax, dx
	pop rbp
	ret

fs_firstChar:
	xor rax, rax
	mov al, [rdi]
	ret

fs_bitSplit:
	push rbp
	mov rbp,rsp
	
	mov ax, 8
	cmp byte [rdi], 0
	je .fin
		mov dl, [rdi]
		mov cl, dl
		dec cl
		and	cl, dl
		cmp cl, 0
		jne .no_es_potencia
		;si es potencia de dos agregamos el bit
		xor al, al
		.ciclo:
			cmp dl, 1
			je .fin
			shr dl, 1
			inc al
			jmp .ciclo
		;sino es 0 ni potencia de 2 devolvemos 9
		.no_es_potencia:
		mov ax, 9
	
	.fin:
	pop rbp
	ret
