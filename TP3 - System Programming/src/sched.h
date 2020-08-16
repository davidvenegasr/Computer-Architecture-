/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "stdint.h"
#include "screen.h"
#include "tss.h"
#include <stdbool.h>

typedef struct str_posicion{
    uint32_t x;
    uint32_t y;
} posicion;

typedef enum task_type{
    RICKC137        = 1,
    MORTYC137       = 2,
    RICKD248        = 3,
    MORTYD248       = 4,
    CRONENBERG      = 5,
    CRONENBERGC127  = 6,
    CRONENBERGD248  = 7
} task_type_t;

//**)VARIABLES:
// Posicion de cada tarea
posicion posTask_actual[CANT_TASKS];

// Posicion de cada tarea viejas:
// OBS: Usado para la pantalla
posicion posTask_viejas[CANT_TASKS];

// Posicion de cada portal:
// |0|: De la portal gun del universo C137
// |1|: De la portal gun del universo D248
posicion posPortal_actual[2];

// Posicion de cada portal viejo
// OBS: Usado para la pantalla
// |0|: De la portal gun del universo C137
// |1|: De la portal gun del universo D248
posicion posPortal_viejas[2];

// Tipo de cada tarea:(Rick-C137, Rick-D248, o Cronenberg)
//      |0| Rick C-137
//      |1| Morty C-137
//      |2| Rick D-248
//      |3| Morty D-248
//      |4| Cronenberg_1
//      ...
//      |24| Cronenberg_20
task_type_t type[CANT_TASKS];

// Tareas vivas
bool alive_tasks[CANT_TASKS];

// debug act            //! Especificar valores posibles que va a tomar
uint32_t debug_ON;

// Contador de tareas vivas de cada Rick
// |0|: RickC137
// |1|: RickD248
uint32_t score[2];

// Tarea ejecutando actualmente
// 0 = Rick C-137
// 1 = Morty C-137
// 2 = Rick D-248
// 3 = Morty D-248
// 4 = Cronenberg_1
// ...
// 24 = Cronenberg_20
uint32_t actual_task;
  
// Portales accionados por Rick para saber si Morty puede hacer un portal,
// |0|: RickC137
// |1|: RickD248
uint32_t contadorMorty[2];

/**
 * Funcion que inicializa todas las estructuras usadas en el juego
 */
/*-------------------------------------------------------------
    Index semantico de tareas en la GDT (elegido por nosotros):
    |15| Rick C-137
    |16| Morty C-137
    |17| Rick D-248
    |18| Morty D-248
    |19| Cronenberg_1
    ...
    |39| Cronenberg_20
--------------------------------------------------------------*/
void sched_init();

/**
 * Funcion que devuelve el la entrada de la GDT correspondiente
 * y modifica la variable actual_task.
 * @return Descriptor de segmento de la siguiente tarea a ejecutar
 */
int16_t sched_nextTask();

//? No estoy muy seguro de como funciona, COMPLETAR DESCRIPCION
void debugger(uint8_t scanCode);

//Funcion encargada de dar de baja a la tarea
void kill_task();

//Funcion que segun el numero de la isr devuelve si la pila tiene un ERRORCODE
uint8_t has_errorcode(uint32_t isr);

#endif	/* !__SCHED_H__ */
