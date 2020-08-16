/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h" 

void task() {
    // breakpoint();
    while(1) {
        syscall_usePortalGun(4, 0, 1, 0);
    };
}
