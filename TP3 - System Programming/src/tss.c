/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"

tss tss_initial;
tss tss_idle;
tss tss_array[24];


void tss_init() {
    tss_create_gdt_entry(GDT_IDX_TASK_INIT, (uint32_t) &tss_initial, 0b00);
}


void tss_create_gdt_entry(uint32_t index, uint32_t base, uint8_t dpl){
    gdt[index].limit_0_15   = sizeof(tss)-1;        // LIMITE: 0x67 = 104 Bytes, que es el tamaño minimo para una TSS
    gdt[index].limit_16_19  = 0x0;                  // LIMITE: limite[16:19] = 0
    gdt[index].base_0_15    = base & 0xFFFF;        // BASE: base[0:15]
    gdt[index].base_23_16   = (base >> 16) & 0xFF; 	// BASE: base[16:23]
    gdt[index].base_31_24   = (base >> 24);         // BASE: base[24:31]
    gdt[index].type         = 0b1001;               // TIPO: 0b10B1, donde B=Busy, pero Busy=0 => 0b1001
    gdt[index].s            = 0; 	                // TIPO: Descriptor de Sistema
    gdt[index].dpl          = dpl;                  // DPL: Kernel o Usuario
    gdt[index].p            = 1;                    // Present
    gdt[index].avl          = 0;                    // AVL: SET 0
    gdt[index].l            = 0;                    // 64bits OFF
    gdt[index].db           = 1;                    // 32bits: ON
    gdt[index].g            = 0;                    // GRANULARIDAD: OFF
}

void tss_init_idle() {
    //Cargo el descriptor de la TSS de la tarea idle en la GDT
    tss_create_gdt_entry(GDT_IDX_TASK_IDLE, (uint32_t) &tss_idle, 0b00);

	//Inicializo la TSS de la tarea Idle
    tss_idle.ptl        = 0;
    tss_idle.unused0 	= 0;
    tss_idle.esp0 		= 0x0; 	                    //ESP0: (NO va a haber cambio de privilegios -> seteamos en 0)
    tss_idle.ss0 		= 0x0; 	                    //SS0: IDEM
    tss_idle.unused1 	= 0;
    tss_idle.esp1 		= 0;
    tss_idle.ss1 		= 0;
    tss_idle.unused2 	= 0;
    tss_idle.esp2 		= 0;
    tss_idle.ss2 		= 0;
    tss_idle.unused3 	= 0;
    tss_idle.cr3 		= KERNEL_PAGE_DIR; 	        //CR3: Direccion del PD del kernel
    tss_idle.eip 		= TASK_IDLE_CODE_ADDR; 	    //EIP: Dirección del comienzo de la tarea idle
    tss_idle.eflags 	= 0x00000202;               //EFLAGS: Interrupciones: ON
    tss_idle.eax 		= 0x00000000;
    tss_idle.ecx 		= 0x00000000;
    tss_idle.edx 		= 0x00000000;
    tss_idle.ebx 		= 0x00000000;
    tss_idle.esp 		= 0x00027000; 	            //ESP: Ubicación del tope del stack del kernel  
    tss_idle.ebp 		= 0x00027000; 	            //EBP: Ubicación de la base del stack del kernel
    tss_idle.esi 		= 0x00000000;
    tss_idle.edi 		= 0x00000000;
    tss_idle.es 		= GDT_SEG_SEL_DATA_0;       //Selectores de segmento de kernel
    tss_idle.unused4 	= 0;
    tss_idle.cs 		= GDT_SEG_SEL_CODE_0;       	
    tss_idle.unused5 	= 0;
    tss_idle.ss 		= GDT_SEG_SEL_DATA_0;
    tss_idle.unused6 	= 0;
    tss_idle.ds 		= GDT_SEG_SEL_DATA_0;
    tss_idle.unused7 	= 0;
    tss_idle.fs 		= GDT_SEG_SEL_DATA_0;
    tss_idle.unused8 	= 0;
    tss_idle.gs 		= GDT_SEG_SEL_DATA_0;
    tss_idle.unused9 	= 0;
    tss_idle.ldt 		= 0;                        //SET 0
    tss_idle.unused10 	= 0;
    tss_idle.dtrap 		= 0x0;                      //SET 0
	tss_idle.iomap 		= 0xFFFF;

}


void tss_init_others(){
    // Son 24 tareas en total, 20 cronenberg, 2 ricks y 2 mortys
    // Inicializamos las entradas de la GDT 
    for(int i = 0; i < 24; i++){
        // la ultima entry de la GDT hasta este punto es GDT_IDX_TASK_IDLE con valor 14
        // por ende ahora inicilizamos entradas a partir del valor 15
        tss_create_gdt_entry(15 + i, (uint32_t)& tss_array[i], 0b11);
    }
}   

void new_task(uint32_t task_dir, uint16_t task_idx_gdt){
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
    
    //Pido una pagina del area libre del kernel para la pila de nivel 0
    uint32_t dir_pila_lvl_0 = mmu_nextFreeKernelPage() + PAGE_SIZE; //porque la pila crece en el sentido contrario. (down-descending)
    
    // Las TSS estan guardadas en un array en memoria, al cual accedemos con el indice de la gdt -15
    tss* tss_actual = &tss_array[task_idx_gdt - 15];
    
    // Obtenemos la pagina fisica correspondiente al mundo cronenberg, en la cual se copiara el codigo de la task
    // segun las coordenadas obtenidas de task.h
    uint8_t x = getRandomX(task_idx_gdt - 15);
    uint8_t y = getRandomY(task_idx_gdt - 15);

    // Ahora sumamos el inicio de mundo cronenberg y multiplicamos por 2 paginas, el tamano de una tarea
    uint32_t task_page_dir = TASKS_MEMORY + (PAGE_SIZE*2)*(y*SIZE_M + x);
    
    //Creo un page directory para la tarea
    uint32_t task_cr3 = mmu_initTaskDir(task_dir, task_page_dir);
    //Inicializarla la TSS 
    tss_actual->ptl         = 0;
    tss_actual->unused0 	= 0;
    tss_actual->esp0 		= dir_pila_lvl_0; 	        //ESP0: Pila del kernel
    tss_actual->ss0 		= GDT_SEG_SEL_DATA_0; 	    //SS0: Selector de segmento de data del kernel de la GDT
    tss_actual->unused1 	= 0;
    tss_actual->esp1 		= 0;
    tss_actual->ss1 		= 0;
    tss_actual->unused2 	= 0;
    tss_actual->esp2 		= 0;
    tss_actual->ss2 		= 0;
    tss_actual->unused3 	= 0;
    tss_actual->cr3 		= task_cr3; 	            //CR3: Direccion del PD nuevo
    tss_actual->eip 		= TASK_CODE; 	            //EIP: Dirección del comienzo de la tarea en cuestion
    tss_actual->eflags 	    = EFLAGS_INT_ON;            //EFLAGS: Interrupciones: ON
    tss_actual->eax 		= 0x00000000;
    tss_actual->ecx 		= 0x00000000;
    tss_actual->edx 		= 0x00000000;
    tss_actual->ebx 		= 0x00000000;
    tss_actual->esp 		= TASK_CODE + PAGE_SIZE*2;  //ESP: Ubicación del tope del stack de nivel 3
    tss_actual->ebp 		= TASK_CODE + PAGE_SIZE*2;  //EBP: Ubicación de la base del stack de nivel 3
    tss_actual->esi 		= 0x00000000;
    tss_actual->edi 		= 0x00000000;
    tss_actual->es 		    = GDT_SEG_SEL_DATA_3;           //Selectores de segmento de kernel
    tss_actual->unused4 	= 0;
    tss_actual->cs 		    = GDT_SEG_SEL_CODE_3;       	
    tss_actual->unused5 	= 0;
    tss_actual->ss 		    = GDT_SEG_SEL_DATA_3;
    tss_actual->unused6 	= 0;
    tss_actual->ds 	    	= GDT_SEG_SEL_DATA_3;
    tss_actual->unused7 	= 0;
    tss_actual->fs 		    = GDT_SEG_SEL_DATA_3;
    tss_actual->unused8 	= 0;
    tss_actual->gs 		    = GDT_SEG_SEL_DATA_3;
    tss_actual->unused9 	= 0;
    tss_actual->ldt 		= 0;                        //SET 0
    tss_actual->unused10 	= 0;
    tss_actual->dtrap 		= 0x0;                      //SET 0
	tss_actual->iomap 		= 0xFFFF;
}
