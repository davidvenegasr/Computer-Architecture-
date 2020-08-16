# Configuraciones útiles
#### Activar log de eventos
- Descomentar la siguiente lı́nea del bochsrc:
```
#log: bochs.log
```
- Comentar la siguiente:
```
log: /dev/null
```
- Para generar logs de todos los eventos reemplazar:
```
debug: action=ignore --> debug: action=report
```
El tamaño del archivo resultante puede ser muy grande ya que registra todos los eventos.

#### Activar GUI del debugger
- Descomentar la siguiente lı́nea del bochsrc:
```
#display library: x, options="gui debug"# use GTK debugger gui
```

# Debugger
#### Ejecución: Next y Step
```
s | step | stepi [count]
	Ejecuta [count] instrucciones

n | next | p
	Ejecuta instrucciones sin entrar a las subrutinas

c | cont | continue
	Continua la ejecución

q | quit | exit
	Sale del debugger y del emulador

Ctrl-C
	Detiene la ejecución y retorna al promt
```

#### Memory Dump
```
x /nuf [addr]
	Muestra el contenido de la dirección [addr]

xp /nuf [addr]
	Muestra el contenido de la dirección fı́sica [addr];
	nuf es número que indica cuantos valores se mostrarán,
	seguido de uno o más de los indicadores de formato.
	
x : hex
o : octal
s : ascii
d : decimal
t : binario
i : instrucción
u : sin signo
c : char

	y de tamaño
b : byte
h : word = half-word
w : doubleword = word
```

#### Memory Disassemble

```
u | disasm | disassemble [count] [start] [end]
	Desensambla intrucciones desde la dirección lineal [start] hasta [end] exclusive.

u | disasm | disassemble switch-mode
	Selecciona la sintaxis Intel o AT&T de asembler.

u | disasm | disassemble size = n
	Setea el tamaño del segmento a desensamblar.
```

#### Breakpoints
```
p | pb | break | pbreak [addr]
	Crea un breakpoint en la dirección fı́sica [addr]

vb | vbreak [seg:offset]
	Crea un breakpoint en la dirección virtual [addr]

lb | lbreak [addr]
	Crea un breakpoint en la dirección lineal [addr]

d | del | delete [n]
	Borra el breakpoint número [n]

bpe [n]
	Activa el breakpoint número [n]

bpd [n]
	Desactiva el breakpoint número [n]
```

#### Watchs
```
watch
	Muestra el estado actual de los watchs
	
watch stop
	Detiene la simulación cuando un watch es encontrado
	
watch continue
	No detiene la simulación si un wath es encontrado
	
watch r | read [addr]
	Agrega un watch de lectura en la dirección fı́sica [addr]
	
watch w | write [addr]
	Agrega un watch de escritura en la dirección fı́sica [addr]
```

#### Infos
```
info break 
	Muestra los Breakpoint creados
	
info eflags 
	Muestra el registro EEFLAGS
	
info idt 
	Muestra el descriptor de interrupciones (idt)
	
info ivt 
	Muestra la tabla de vectores de interrupción
	
info gdt 
	Muestra la tabla global de descriptores (gdt)
	
info tss 
	Muestra el segmento de estado de tarea actual (tss)
	
info tab 
	Muestra la tabla de paginas
```

### __Registros__

#### Registros de propósito general
```
r | reg | regs | registers
	Lista los registros del CPU y sus contenidos
```
<details>
	<summary>Ejemplo de Ejecucion</summary>

```
<bochs:12> registers
eax: 0x00000000 0
ecx: 0x00000000 0
edx: 0x00000543 1347
ebx: 0x00000000 0
esp: 0x00000000 0
ebp: 0x00000000 0
esi: 0x00000000 0
edi: 0x00000000 0
eip: 0x0000e05d
eflags 0x00000046
id vip vif ac vm rf nt IOPL=0 of df if tf sf ZF af PF cf
```
</details>


#### Registros de Segmento
```
sreg
	Muestra los registros de segmento
```
<details>
	<summary>Ejemplo de Ejecucion</summary>

```
<bochs:5> sreg
cs:s=0xf000, dh=0xff0093ff, dl=0x0000ffff, valid=7
ds:s=0x0000, dh=0x00009300, dl=0x0000ffff, valid=7
ss:s=0x0000, dh=0x00009300, dl=0x0000ffff, valid=7
es:s=0x0000, dh=0x00009300, dl=0x0000ffff, valid=7
fs:s=0x0000, dh=0x00009300, dl=0x0000ffff, valid=7
gs:s=0x0000, dh=0x00009300, dl=0x0000ffff, valid=7
ldtr:s=0x0000, dh=0x00008200, dl=0x0000ffff, valid=1
tr:s=0x0000, dh=0x00008b00, dl=0x0000ffff, valid=1
gdtr:base=0x00000000, limit=0xffff
idtr:base=0x00000000, limit=0xffff
```
</details>	


#### Registros de Control
```
creg
	Muestra los registros de control
```
<details>
	<summary>Ejemplo de Ejecucion</summary>

```
<bochs:10> creg
CR0=0x60000010: pg CD NW ac wp ne ET ts em mp pe
CR2=page fault laddr=0x00000000
CR3=0x00000000
PCD=page-level cache disable=0
PWT=page-level writes transparent=0
CR4=0x00000000: osxsave smx vmx osxmmexcpt osfxsr pce pge mce pae pse de tsd pvi vme
```
</details>	

### Magic Breakpoint
```
xchg bx, bx
	Esta instrucción detiene el flujo del programa y nos devuelve al prompt de bochs.
```





