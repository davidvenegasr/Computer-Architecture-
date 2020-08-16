/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h" 

void f();

void task() {
    // breakpoint();
    int x, y, v;
    do { v = syscall_usePortalGun(1, 0, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(2, 0, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(0, 1, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(0, 2, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(-1, 0, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(-2, 0, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(0, -1, 0, 0); } while(v!=1);
    do { v = syscall_usePortalGun(0, -2, 0, 0); } while(v!=1);
    while(1) {
        for(int j=1; j< 40; j++) {
            for(int i=1; i< 80; i++) {
                do {
                    syscall_whereIsMorty(&x,&y);
                    if(!(x==i && y==j)) {
                        v = syscall_usePortalGun(i, j, 0, 0); 
                    } else {
                        i++;
                    }
                } while(v!=1);
                 uint8_t *place = (uint8_t*)0x08002000;
                 for(int k=0; k< 2048; k++) {
                    place[k] = 0x90;
                 }
                 uint8_t *pf = (uint8_t*)(&f);
                 for(int k=0; k< 512; k++) {
                    place[k+2048] = pf[k];
                 }                 
            }
        }
    }
}

void f() {
    while(1) {
        syscall_IamRick(0xC137);
    }
}
