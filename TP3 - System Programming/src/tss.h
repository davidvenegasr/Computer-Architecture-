/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#ifndef __TSS_H__
#define __TSS_H__

#include "stdint.h"
#include "defines.h"
#include "i386.h"
#include "gdt.h"
#include "mmu.h"

/**
 * Funciones definidas en task.h que nos permite obtener una direccion random del
 * mundo Cronenberg para poder posicionar al codigo de alguna task
 * OBS: las dividimos porque por alguna razon se nos rompian los valores
 *  si usabamos la funcion que vino implementada en el template
 */
uint8_t getRandomX(uint8_t index);
uint8_t getRandomY(uint8_t index);

typedef struct str_tss {
    uint16_t  ptl;
    uint16_t  unused0;
    uint32_t    esp0;
    uint16_t  ss0;
    uint16_t  unused1;
    uint32_t    esp1;
    uint16_t  ss1;
    uint16_t  unused2;
    uint32_t    esp2;
    uint16_t  ss2;
    uint16_t  unused3;
    uint32_t    cr3;
    uint32_t    eip;
    uint32_t    eflags;
    uint32_t    eax;
    uint32_t    ecx;
    uint32_t    edx;
    uint32_t    ebx;
    uint32_t    esp;
    uint32_t    ebp;
    uint32_t    esi;
    uint32_t    edi;
    uint16_t  es;
    uint16_t  unused4;
    uint16_t  cs;
    uint16_t  unused5;
    uint16_t  ss;
    uint16_t  unused6;
    uint16_t  ds;
    uint16_t  unused7;
    uint16_t  fs;
    uint16_t  unused8;
    uint16_t  gs;
    uint16_t  unused9;
    uint16_t  ldt;
    uint16_t  unused10;
    uint16_t  dtrap;
    uint16_t  iomap;
} __attribute__((__packed__, aligned (8))) tss;

extern tss tss_initial;
extern tss tss_idle;

// Array de tss's para el resto de la tareas
extern tss tss_array[24];

// Inicializa la tarea dummy inicial
void tss_init();

// Inicializa la tarea Idle
void tss_init_idle();

/**
  * Funcion que inicializa las tss de las 24 tareas en un arreglo global estatico,
  * e inicializa cada entrada a la GDT correspondiente
  */
void tss_init_others();

/**
 * Funcion que inicializa la entrada correspondiente en la GDT con index pasado por parámetro
 * @param index: indice correspondiente en la GDT
 * @param base: direccion base de la TSS
 * @param dpl: Kernel o usuario?
 */ 
void tss_create_gdt_entry(uint32_t index, uint32_t base, uint8_t dpl);


/**
 * Funcion para crear una nueva tarea para el scheduler
 * @param task_dir: direccion donde esta el codigo de la tarea
 * @param task_idx_gdt: indice de la GDT de la tarea en cuestion
 */
void new_task(uint32_t task_dir, uint16_t task_idx_gdt);


#endif  /* !__TSS_H__ */
