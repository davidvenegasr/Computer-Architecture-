/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#include "gdt.h"


gdt_entry gdt[GDT_COUNT] = {
    /* Descriptor nulo*/
    /* Offset = 0x00 */
    [GDT_IDX_NULL_DESC] = (gdt_entry) {
        (uint16_t)    0x0000,         /* limit[0:15]  */
        (uint16_t)    0x0000,         /* base[0:15]   */
        (uint8_t)     0x00,           /* base[23:16]  */
        (uint8_t)     0x00,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x00,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x00,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x00,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     0x00,           /* base[31:24]  */
    },

    [GDT_IDX_CODE_0] ={
            .base_0_15      = 0x0,      //BASE: desde el inicio de la RAM
            .base_23_16     = 0x0,      //BASE: idem
            .base_31_24     = 0x0,      //BASE: idem
            .limit_0_15     = 0x88FF,   //LIMITE: (137MiB / 4KiB)-1    // Mib = 1024^2 bytes
            .limit_16_19    = 0x0,      //LIMITE: 0 (High Part)
            .type           = 10,       // Execute/Read
            .s              = 1,        // TYPE: Code or Data
            .dpl            = 0,        // PERMISO: 0
            .p              = 1,        // para que lea el descriptor
            .avl            = 0,        // SET 0
            .l              = 0,        // 64bits: OFF
            .db             = 1,        // 32 bits: ON
            .g              = 1,        // granularidad: en 1 para direccionar a 4KB 
    }, 

    [GDT_IDX_CODE_3] ={
            .base_0_15      = 0x0,      
            .base_23_16     = 0x0,      
            .base_31_24     = 0x0,      
            .limit_0_15     = 0x88FF,   
            .limit_16_19    = 0x0,    
            .type           = 10,       // Execute/Read
            .s              = 1,        // TYPE: Code or Data
            .dpl            = 3,        // PERMISO: 3
            .p              = 1,
            .avl            = 0,   
            .l              = 0,  
            .db             = 1, 
            .g              = 1, 
    }, 

    [GDT_IDX_DATA_0] ={
            .base_0_15      = 0x0,      
            .base_23_16     = 0x0,      
            .base_31_24     = 0x0,      
            .limit_0_15     = 0x88FF,   
            .limit_16_19    = 0x0,    
            .type           = 2,        // Read/Write
            .s              = 1,        // TYPE  Code or Data
            .dpl            = 0,        // PERMISO  0
            .p              = 1,
            .avl            = 0,   
            .l              = 0,  
            .db             = 1, 
            .g              = 1, 
    },

    [GDT_IDX_DATA_3] ={
            .base_0_15      = 0x0,      
            .base_23_16     = 0x0,      
            .base_31_24     = 0x0,      
            .limit_0_15     = 0x88FF,   
            .limit_16_19    = 0x0,    
            .type           = 2,        // Read/Write
            .s              = 1,        // TYPE  Code or Data
            .dpl            = 3,        // PERMISO  3
            .p              = 1,
            .avl            = 0,   
            .l              = 0,  
            .db             = 1, 
            .g              = 1, 
	},
    [GDT_IDX_VIDEO] ={
            .base_0_15      = 0x8000, 	//BASE: 0xB8000 --> 8000h
            .base_23_16     = 0x000B,  	//BASE: 0xB8000 --> 000Bh
            .base_31_24     = 0x0,      //BASE: 0xB8000 --> 0h
            .limit_0_15     = 0x1F3F,  	//LIMITE: (80 * 50 * 2) - 1 = 7999 = 1F3Fh
            .limit_16_19    = 0x0,    	//LIMITE: 0h (high part)
            .type           = 2,        // Read/Write
            .s              = 1,        // TYPE  Code or Data
            .dpl            = 0,        // PERMISO 0
            .p              = 1,
            .avl            = 0,   
            .l              = 0,  
            .db             = 1, 
            .g              = 0,        // Granularidad 0 1MB= 1024^2 bytes 
	}
};

gdt_descriptor GDT_DESC = {
    sizeof(gdt) - 1,
    (uint32_t) &gdt
};
