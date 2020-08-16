/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Bool */
/* -------------------------------------------------------------------------- */
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

/* Atributos paginas */
/* -------------------------------------------------------------------------- */
#define PAG_P                   0x00000001
#define PAG_R                   0x00000000
#define PAG_RW                  0x00000002
#define PAG_S                   0x00000000
#define PAG_US                  0x00000004

/* Misc */
/* -------------------------------------------------------------------------- */
#define SIZE_N                  40 // X
#define SIZE_M                  80 // Y

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT                   40
#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_CODE_0              8
#define GDT_IDX_CODE_3              9
#define GDT_IDX_DATA_0              10
#define GDT_IDX_DATA_3              11
#define GDT_IDX_VIDEO               12
#define GDT_IDX_TASK_INIT           13 
#define GDT_IDX_TASK_IDLE           14 

#define GDT_IDX_BASE_TASKS          15

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC << 3)

#define GDT_OFF_CODE_0              (GDT_IDX_CODE_0 << 3)
#define GDT_OFF_CODE_3              (GDT_IDX_CODE_3 << 3)
#define GDT_OFF_DATA_0              (GDT_IDX_DATA_0 << 3)
#define GDT_OFF_DATA_3              (GDT_IDX_DATA_3 << 3)
#define GDT_OFF_TASK_IDLE           (GDT_IDX_TASK_IDLE << 3)


/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */
#define GDT_SEG_SEL_CODE_0          GDT_OFF_CODE_0 | 0x0
#define GDT_SEG_SEL_CODE_3          GDT_OFF_CODE_3 | 0x3
#define GDT_SEG_SEL_DATA_0          GDT_OFF_DATA_0 | 0x0
#define GDT_SEG_SEL_DATA_3          GDT_OFF_DATA_3 | 0x3
#define GDT_SEG_SEL_TASK_IDLE       GDT_OFF_IDLE_DESC | 0x0


/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */
#define BOOTSECTOR               0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                   0x00001200 /* direccion fisica de comienzo del kernel */
#define VIDEO                    0x000B8000 /* direccion fisica del buffer de video */

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */
#define TASK_CODE             0x08000000 /* direccion virtual del codigo */

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */
 #define TASK_IDLE_CODE_ADDR            0x0001A000
 #define TASK_RICK_C137_CODE_ADDR       0x00010000
 #define TASK_MORTY_C137_CODE_ADDR      0x00012000
 #define TASK_RICK_D248_CODE_ADDR       0x00014000
 #define TASK_MORTY_D248_CODE_ADDR      0x00016000
 #define TASK_CRONENBERG_CODE_ADDR      0x00018000
 

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR          0x00027000
#define KERNEL_PAGE_TABLE_0      0x00028000
#define CRYSTALS_MAP             0x00029000




/*Tamaño de paginas*/
#define PAGE_SIZE       0x1000

/*Inicio de Area libre del Kernel*/
#define FREE_KERNEL_SPACE 0x100000

/*Inicio del mundo Cronenberg*/
#define TASKS_MEMORY    0x400000

/*TSS - Valores para EFLAGS*/
#define EFLAGS_INT_ON   0x00000202
#define EFLAGS_INT_OFF  0x00000002

/*Cantidad de tasks*/
#define CANT_TASKS  24

/*GAME - Valor de portal no visible
  OBS: Offset fuera de la pantalla 0XFF > 80*/
#define PORTAL_NO_VISIBLE 0xFF

/*GAME - Equipo o Universo*/
#define C137  0
#define D248  1

/*GAME - Task viva o muerta*/
#define DEAD  0
#define ALIVE  1

/*DEBUGGER*/
#define CANT_EXCEPCIONES  21 

#endif  /* !__DEFINES_H__ */
