/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "stdint.h"
#include "defines.h"
#include "i386.h"
#include "tss.h"
//#include "game.h"

#define FLAG_MMU_PRESENT    1
#define FLAG_MMU_US_USER    1
#define FLAG_MMU_US_KERNEL  0
#define FLAG_MMU_RW_WRITE   1
#define FLAG_MMU_RW_READ    0

typedef struct str_page_dir_entry
{
  uint8_t present:1;
  uint8_t read_write:1;
  uint8_t user_supervisor:1;
  uint8_t page_write_through:1;
  uint8_t page_cache_disable:1;
  uint8_t accesed:1;
  uint8_t x:1;
  uint8_t page_size:1;
  uint8_t ignored:1;
  uint8_t available:3;
  uint32_t page_table_base:20;
}__attribute((__packed__)) page_dir_entry;

typedef struct str_page_table_entry
{
  uint8_t present:1;
  uint8_t read_write:1;
  uint8_t user_supervisor:1;
  uint8_t page_write_through:1;
  uint8_t page_cache_disable:1;
  uint8_t accesed:1;
  uint8_t dirty:1;
  uint8_t x:1;
  uint8_t global:1;
  uint8_t available:3;
  uint32_t physical_address_base:20;
}__attribute((__packed__)) page_table_entry;

void mmu_init();

uint32_t mmu_nextFreeKernelPage();

void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint8_t rw, uint8_t u_s);

uint32_t mmu_unmapPage(uint32_t cr3, uint32_t virtual);

uint32_t mmu_initKernelDir();

/**
 * * Se encarga de inicializar un directorio de páginas y tablas de páginas para una tarea,
 * respetando la figura 3. La rutina mapea las páginas del mundo Cronenberg donde 
 * estará ubicada la tarea a partir de la dirección virtual 0x08000000(128MB).
 * Luego, debe copiar en está área el código de la tarea. 
 * @param taskDir: Direccion del codigo de la tarea que debe ser copiado desde el kernel (src)
 * @param taskPage: Direccion donde copiar el codigo (dst)
 * @return cr3 nuevo para la tarea
 */
uint32_t mmu_initTaskDir(uint32_t taskDir, uint32_t taskPage);

/**
 * * COPY_TASK lo que hace es copiar el código de una tarea (4KiB) de un punto de la memoria a otro.
 * @param src: Direccion de donde va a copiar
 * @param dst: Direccion a donde va a pegar
 */
void copy_task(uint32_t src, uint32_t dst);

#endif	/* !__MMU_H__ */




