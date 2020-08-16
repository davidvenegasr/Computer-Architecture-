/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"

uint32_t next_free_kernel_page;

void mmu_init() {
    next_free_kernel_page = FREE_KERNEL_SPACE;
    // La direccion apunta al area libre del kernel
    // donde van a estar todas las nuevas PAGINAS
}

uint32_t mmu_nextFreeKernelPage() {
    uint32_t free_page = next_free_kernel_page;
    next_free_kernel_page += PAGE_SIZE;
    return free_page;
}

void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint8_t r_w, uint8_t u_s) {
    // DIR Virtual = | 31 ... 22 | 21 ... 12 | 11 ... 0 | 
    uint32_t directoryIdx = virtual >> 22;
    uint32_t tableIdx = (virtual >> 12) & 0x3FF;
    
    // Obtenemos la direccion de la PD
    page_dir_entry* PD_base_addr = (page_dir_entry*) ((cr3)&~0xFFF);
    // Obtenemos la PDE
    page_dir_entry PDE = PD_base_addr[directoryIdx];

    // Si la PDE todavia no fue inicializada...
    if (PDE.present != 1){
        //Obtenemos una nueva PT
        page_table_entry* newPT = (page_table_entry*) mmu_nextFreeKernelPage();
        //Inicialimos todos los PTE en 0
        for(int i = 0; i < 1024; ++i){
            newPT[i] = (page_table_entry){0};
        }
        //Modificamos los valores de la PDE
		PDE.page_table_base = (uint32_t)newPT >> 12;
		PDE.present 		= FLAG_MMU_PRESENT;
		PDE.user_supervisor	= FLAG_MMU_US_USER;
		PDE.read_write		= FLAG_MMU_RW_WRITE;            //? FIXME: porque no se setean con los valores de entrada?
		//Volcamos en memoria la PDE modificada
        PD_base_addr[directoryIdx] = PDE;
    }
    //Obtenemos la base del PT
    page_table_entry* PT_base_addr = (page_table_entry*)(PDE.page_table_base << 12);
    //Obtenemos la PTE correspondiente
    page_table_entry PTE = PT_base_addr[tableIdx];
    //Una vez obtenido PTE lo modificamos
	PTE.physical_address_base 	= phy>>12;
	PTE.present 				= FLAG_MMU_PRESENT;
	PTE.read_write 				= r_w;
	PTE.user_supervisor			= u_s;
    //Por ultimo volcamos el PTE modificado
    PT_base_addr[tableIdx] = PTE;
	//Flusheamos la TLB
    tlbflush();
}

uint32_t mmu_unmapPage(uint32_t cr3, uint32_t virtual) {
	// DIR Virtual = | 31 ... 22 | 21 ... 12 | 11 ... 0 | 
    uint32_t directoryIdx = virtual >> 22;
    uint32_t tableIdx = (virtual >> 12) & 0x3FF;

	// Obtenemos la direccion de la PD
    page_dir_entry* PD_base_addr = (page_dir_entry*) ((cr3)&~0xFFF); //_FIXME
    // Obtenemos la PDE
    page_dir_entry* PDE = &PD_base_addr[directoryIdx];

	//Si la PDE esta disponible..
	if(PDE->present == 1){
		// Obtenemos la direccion de la PT
		page_table_entry* PT_base_addr = (page_table_entry*)(PDE->page_table_base << 12);
		// Obtenemos la PTE
		page_table_entry* PTE = &PT_base_addr[tableIdx];
		// Deshabilitamos la PTE
		PTE->present = 0;
	}
	//Flusheamos la TLB
	tlbflush();
	return 0;
}

uint32_t mmu_initKernelDir() {
    page_dir_entry *pd = (page_dir_entry*) KERNEL_PAGE_DIR;
    page_table_entry *pt_0 = (page_table_entry*) KERNEL_PAGE_TABLE_0;

    // Ahora inicializamos en cero 
    // Como un directorio tiene 1024 entradas, al igual que una tabla de paginas

    for(int i =0; i < 1024; i++){
        pd[i]= (page_dir_entry){0};
        pt_0[i]= (page_table_entry){0};
    }

    //Ahora para inicializar la primera entrada del directorio 

	// Cargamos la primer entry del PD
	pd[0].present = FLAG_MMU_PRESENT;               // Presente
	pd[0].user_supervisor = FLAG_MMU_US_KERNEL;     // Kernel
	pd[0].read_write = FLAG_MMU_RW_WRITE;           // Read/Write enabled
	pd[0].page_table_base = ((uint32_t) pt_0) >> 12;
	// La direccion de pt_0 debe ser shifteada 12 bits a der porque las paginas son de 4 KiB = 2^12 Bytes
    
    for(int i = 0; i<1024;++i)
    {
        pt_0[i].present = FLAG_MMU_PRESENT;             // Presente
		pt_0[i].user_supervisor = FLAG_MMU_US_KERNEL;   // Kernel
		pt_0[i].read_write = FLAG_MMU_RW_WRITE;         // Read/Write enabled
		pt_0[i].physical_address_base = i;
		// El valor esta seteado en i por que tenemos un identity mapping desde 0x0 hasta 0x003FFFFF
		// donde dicho valor representa el ultimo valor posible con 1024 paginas de 4KiB (1024 * 4KiB - 1 = 0x3FFFFF)
    }
    return (uint32_t)pd;
}

uint32_t mmu_initTaskDir(uint32_t taskDir, uint32_t taskPage) {
    // Pedimos memoria de kernel para el PD y el PT de la tarea
    page_dir_entry* pd = (page_dir_entry*) mmu_nextFreeKernelPage();
    page_table_entry* pt_0 = (page_table_entry*) mmu_nextFreeKernelPage();
    // Tenemos que tener un identity mapping de los primeros 4 MB
    for(int i =0; i < 1024; i++){
        pd[i]= (page_dir_entry){0};
        pt_0[i]= (page_table_entry){0};
    }
    pd[0].present = FLAG_MMU_PRESENT;               // Presente
	pd[0].user_supervisor = FLAG_MMU_US_KERNEL;     // Kernel
	pd[0].read_write = FLAG_MMU_RW_WRITE;           // Read/Write enabled
	pd[0].page_table_base = ((uint32_t) pt_0) >> 12;
    for(int i = 0; i<1024;++i){
        pt_0[i].present = FLAG_MMU_PRESENT;             // Presente
		pt_0[i].user_supervisor = FLAG_MMU_US_KERNEL;   // Kernel
		pt_0[i].read_write = FLAG_MMU_RW_WRITE;         // Read/Write enabled
		pt_0[i].physical_address_base = i;
		// El valor esta seteado en i por que tenemos un identity mapping desde 0x0 hasta 0x003FFFFF
		// donde dicho valor representa el ultimo valor posible con 1024 paginas de 4KiB (1024 * 4KiB - 1 = 0x3FFFFF)
    }

    //*)Identificar el codigo de la tarea que debe ser copiado desde el kernel (src):
    //  La direccion del codigo de la tarea viene en el taskDir

    //*)Identificar la posicion de memoria donde copiar el codigo (dst):
    //  La direccion donde se va a copiar el codigo es taskPage

    //*)Mapear de ser necesario la posicion destino o fuente del codigo:
    //  Como el taskPage es una direccion fisica necesitamos mapearla temporalmente
    //  para poder hacer la copia del codigo
    //  Por ello mapearemos 2 paginas, haremos la copia y desmapearemos

    uint32_t taskPage_stack = taskPage + PAGE_SIZE;

    uint32_t taskDir_stack = taskDir + PAGE_SIZE;

    mmu_mapPage(rcr3(), taskPage, taskPage, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage(rcr3(), taskPage_stack, taskPage_stack, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);

    //*)Copiar las dos paginas de la tarea:
    copy_task(taskDir, taskPage);
    copy_task(taskDir_stack, taskPage_stack);       //? TODO: chequear si es necesario

    //*)Mapear la tarea copiada, al nuevo esquema de paginacion que se esta construyendo:
    mmu_mapPage((uint32_t) pd, TASK_CODE, taskPage, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage((uint32_t) pd, TASK_CODE + PAGE_SIZE, taskPage_stack, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);

    //*)Desmapear de ser necesario las paginas mapeadas para poder copiar:
    mmu_unmapPage(rcr3(), taskPage);
    mmu_unmapPage(rcr3(), taskPage_stack);
    
    return (uint32_t)pd;
}

void copy_task(uint32_t src, uint32_t dst){
    uint32_t * puntero_src = (uint32_t*) src;
    uint32_t * puntero_dst = (uint32_t*) dst;

    for(int i = 0; i < 1024; i++){
        *puntero_dst = *puntero_src;
        puntero_dst++;
        puntero_src++;
    }
}






