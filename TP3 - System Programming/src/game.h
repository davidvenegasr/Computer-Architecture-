/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "stdint.h"
#include "defines.h"
#include "screen.h"
#include "mmu.h"
#include "sched.h"
#include <stdbool.h>

typedef enum e_taskType {
    RickC137       = 1,
    MortyC137      = 2,
    RickD248       = 3,
    MortyD248      = 4,
    Cronenberg     = 5,
    CronenbergC137 = 6,
    CronenbergD248 = 7,
} e_taskType_t;

/* #Arreglo de cronenbergs:
 * ##Valores posibles: 
 * 0 = es un cronenberg,
 * 1 = su mente pertenece a RickC137,
 * 2 = su mente pertenece a RickD248,
 * -1 = muerto
 */
int32_t cronenbergs[20];

/* Arreglo de Ricks y Mortys:
 ** Usado para ver si algun Rick o Morty es controlado
 * |0|: RickC137
 * |1|: MortyC137
 * |2|: RickD248
 * |3|: MortyD248
 ** Posibles valores:
 * 0 = Universo C137
 * 1 = Universo D248                       
 * -1 = muerto
 */
int32_t rick_and_mortys_control[4];

// Flag que indica si se termino el juego
bool endOfGame;

// Variable para que no se creen mas de 1 portal por tick de reloj
bool new_portal_available; 

//Guarda el valor del tick de clock         //? No entiendo para que se usa la estructura
uint16_t ticks_clocks[20];

//Variable que lleva el puntaje de ambos ricks
//* RickC137 = 0 | RickD248 = 1
//// uint32_t score[2];

/**
 * Funcion que incializa todas las variables necesarias para el juego
 */
void game_init();

/**
 * SYSCALL: (137) UsePortalGun 
 * Genera un portal desde la posición de la tarea que llamo al servicio
 * a la celda en el desplazamiento indicado por x e y.
 * @param x: Desplazamiento en x
 * @param y: Desplazamiento en y
 * @param cross: 0 = No cruzar | 1 = Cruzar
 * @param withMorty: 0 = Sin Morty | 1 = Con Morty
 * @return 1 = portal creado | 0 = portal NO creado
 */
uint32_t rutina_137(int32_t x, int32_t y,uint32_t cross, uint32_t withMorty);

/**
 * SYSCALL: (138) IamRick 
 * Indica al sistema el universo del que proviene la tarea.
 * @param code: Codigo de identificacion de universos
 * code=0xC137 para Rick C-137 | code=0xD248 para Rick D-248
 */
void rutina_138(uint32_t code);

/**
 * SYSCALL: (139) whereIsMorty 
 * Retorna el desplazamiento que existe entre un Morty y su Rick
 * @return x = Desplazamiento entre Rick y Morty en x
 * @return y = Desplazamiento entre Rick y Morty en y
 */
void rutina_139(uint32_t *x, uint32_t *y);

/**
 * Funcion que recibe coordenadas relativas a un punto de anclaje
 * y las transforma a coordenas absolutas del mapa
 * @param centro: coordenada absoluta del centro
 * @param x: desplazamiento x del centro
 * @param y: desplazamiento y del centro
 * @return coordenadas absolutas en el mapa
 */
posicion coord_rel_a_abs(posicion centro, int32_t x, int32_t y);

/**
 * Funcion que recibe absolutas y las transforma a una direccion de memoria del mundo Cronenberg
 * @param x: coordenada absoluta x del mapa Cronenberg
 * @param y: coordenada absoluta y del mapa Cronenberg
 * @return direccion fisica del mundo Cronenberg
 */
uint32_t coord_abs_a_dir_fisica(uint32_t x, uint32_t y);

/**
 * Funcion que realiza el movimiento de Rick por un portal
 * @pre cross == 1
 * @param x coordenada x relativa a donde este el Rick que hizo el portal
 * @param y coordenada y relativa a donde este el Rick que hizo el portal
 */
void mover_Rick(int32_t x, int32_t y);

/**
 * Funcion auxiliar que realiza el movimiento de Rick por un portal
 * @pre cross == 1
 * @param dir_fisica_src: direccion FISICA de donde se va a copiar el codigo
 * @param dir_virtual_dst: direccion VIRTUAL a donde se va a copiar el codigo
 * @param cr3: cr3 de la tarea
 */
void move_task(uint32_t dir_fisica_src, uint32_t dir_virtual_dst, uint32_t cr3);

/**
 * Funcion que obtiene el cr3 de Morty para hacerlo saltar
 * @pre actualTask == RICKxxxx
 * @pre withMorty == 1
 * @pre cross == 1
 * @return cr3 de Morty
 */
uint32_t obtener_cr3_Morty();

/**
 * Funcion que realiza el movimiento de Morty por un portal solo
 * @pre actualTask == MORTYxxxx
 * @pre cross == 1
 * @param x coordenada x relativa a donde este el Morty
 * @param y coordenada y relativa a donde este el Morty
 * @param cr3 cr3 de Morty
 */
void mover_Morty(int32_t x, int32_t y, uint32_t cr3);

/**
 * Funcion que mapea los 8k del espacio destino a partir de
 * la dirección 0x08002000 de la tarea que llamo al servicio
 * @pre cross == 0
 * @param x coordenada x relativa a quien haya creado el portal
 * @param y coordenada y relativa a quien haya creado el portal
 */
void crear_portal(int32_t x, int32_t y);

void game_updateScore();
void screen_incTaskClocks();
void screen_initTaskClocks();
void screen_drawTasks();
void game_checkEndOfGame();

//FuncionesTriviales
void printRickC(uint8_t x, uint8_t y);
void printMortyC(uint8_t x, uint8_t y);
void printRickD(uint8_t x, uint8_t y);
void printMortyD(uint8_t x, uint8_t y);
void printRickCXOR(uint8_t x, uint8_t y);
void printMortyCXOR(uint8_t x, uint8_t y);
void printRickDXOR(uint8_t x, uint8_t y);
void printMortyDXOR(uint8_t x, uint8_t y);
void printCronenbergAlive(uint8_t x, uint8_t y);
void printCronenbergDead(uint8_t x, uint8_t y, uint8_t i);
void printCronenbergC(uint8_t x, uint8_t y);
void printCronenbergD(uint8_t x, uint8_t y);

#endif  /* !__GAME_H__ */
