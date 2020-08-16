/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "task.h"

uint32_t debug_var_1 = 0;    //? Variables solo para debugueo propio
uint32_t debug_var_2 = 0;

void game_init() {
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
    // Creamos todas las tareas del mapa: 20 Cronembergs, 2 Ricks y 2 Mortys
    for (int i = GDT_IDX_BASE_TASKS; i < 39; i++){
        switch (i){
            case 15:
                new_task(TASK_RICK_C137_CODE_ADDR,i);
                break;
            case 16:
                new_task(TASK_MORTY_C137_CODE_ADDR,i);
                break;
            case 17:
                new_task(TASK_RICK_D248_CODE_ADDR,i);
                break;
            case 18:
                new_task(TASK_MORTY_D248_CODE_ADDR,i);
                break;
            default:
                new_task(TASK_CRONENBERG_CODE_ADDR,i);
                break;
        }
    }
    // Marcamos a todas las tareas como vivas
    for(int i=0; i < 24;i++){
        alive_tasks[i] = ALIVE;
    }
    
    // Marcamos todos los cronenbergs como normales (sin dueño)
    for (int i = 0; i < 20; i++){
        cronenbergs[0]=0;
    }
    
    //Marcamos clocks en 0
    for (int i = 0; i < 20; i++){
        ticks_clocks[0]=0;
    }
    
    // Inicializamos los Ricks y Mortys con sus universos correctos
    rick_and_mortys_control[0]=C137;
    rick_and_mortys_control[1]=C137;
    rick_and_mortys_control[2]=D248;
    rick_and_mortys_control[3]=D248;
    
    // Comienza habilitada la posibilidad de hacer portales
    new_portal_available = true;
    endOfGame = false;
    screen_initTaskClocks();
}

uint32_t rutina_137(int32_t x, int32_t y, uint32_t cross, uint32_t withMorty){
    /*
    Genera un portal desde la posición de la tarea que llamo al servicio a la celda en el desplazamiento
    indicado por x e y.
    * Si se indica cross en 1:
        la tarea será copiada a destino.
    * Si se indica cross en 0:
        la tarea no será copiada, pero serán mapeados los 8k del espacio destino, a partir de la
        dirección 0x08002000 de la tarea que llamo al servicio.
    * A su vez, si se marca withMorty en 1,
        la tarea morty asociada al tarea Rick que creo el portal se desplazará o mapeará el lugar destino
        considerando el desplazamiento indicado, pero esta vez desde donde este ubicada la tarea Morty.
    
    Este servicio no puede ser usado ilimitadamente.
    * Los Ricks puede crear portales solamente una vez por cada tick de reloj,
    * mientras que los Mortys puede crear portales cada 10 veces que Rick cree un portal.
    * Obviamente, las tareas Cronenberg no tienen arma de portales, así que no pueden usar este servicio.
    
    En el caso que se intente crear un portal pero ya se haya llamado al servicio en el presente tick
    de reloj, el servicio retornará un 0 en EAX indicando que el portal no fue creado. En el caso
    contrario, cuando el portal sea creado exitosamente, el servicio deberá retornar un 1.
    */
   
    //Variable para actualizar pantalla
    //Actualizamos el portal -UNIVERSE PORTAL
    uint8_t up = C137*(type[actual_task]==RICKC137 || type[actual_task]==MORTYC137) + 
                 D248*(type[actual_task]==RICKD248 || type[actual_task]==MORTYD248);
                 
    if(new_portal_available){
        new_portal_available = false; //setea para que en el mismo ciclo de clock no se llama dos veces a usePortalGun
        switch (actual_task){
            case 0: //RICK-C137
            case 2: //RICK-D248
                if(cross){
                    mover_Rick(x,y);
                    
                    //Funciones para controlar portales en pantalla
                    if((posPortal_actual[up].x != PORTAL_NO_VISIBLE)&&(posPortal_actual[up].y != PORTAL_NO_VISIBLE)){
                        posPortal_viejas[up].x = posPortal_actual[up].x;
                        posPortal_viejas[up].y = posPortal_actual[up].y;
                    }
                    
                    if(withMorty) {
                        mover_Morty(x,y, obtener_cr3_Morty()); 
                    }
                } else {
                    crear_portal(x,y);
                    //Si el flag WithMorty esta activado no hago nada //TODO: agregar al informe
                }
                contadorMorty[actual_task==2]++;
                return 1;
                break;
            case 1: //MORTY-C137
            case 3: //MORTY-D248
                /** Para que morty pueda hacer un portal:
                 * + contadorMorty valido
                 * + no puede estar activado withMorty
                 * Si no cumplen: no hacen nada //TODO: agregar al informe
                 */
                if((contadorMorty[actual_task==3]%10 == 0) && (contadorMorty[actual_task==3]!=0) && (withMorty !=1)){
                    contadorMorty[actual_task==3] = 0;
                    if(cross){
                        mover_Morty(x,y, rcr3());
                    } else {
                        //crear portal
                        crear_portal(x,y);
                    }
                    return 1;
                }
                break;
            default:
                debug_var_1 = 2;
                debug_var_2 = 0;
                debug_var_1 = debug_var_1 / debug_var_2;
                //// DECIDIMOS no hacer nada si un cronenberg usa la PortalGun
                //! CORRECCION
                // Si un cronenberg usa el servicio forzamos a que se produzca una excepcion
                // asi el cronenberg sera desalojado del scheduler
                //TODO: CAMBIAR en el informe, asi funciona a la perfeccion
                break;
        }
    }
    return 0;
}

void mover_Rick(int32_t x, int32_t y){
    //Obtenemos la posicion actual
    posicion pos_actual_rick = posTask_actual[actual_task];
    //Guardamos la Dir Fisica de la posicion actual
    uint32_t dir_fisica_actual_rick = coord_abs_a_dir_fisica(pos_actual_rick.x, pos_actual_rick.y);
    //Primero desmapeamos la tarea rick
    mmu_unmapPage(rcr3(), TASK_CODE);
    mmu_unmapPage(rcr3(), TASK_CODE + PAGE_SIZE);
    
    //Convertimos x e y a dir fisica del mundo cronenberg
    posicion pos_abs_dst = coord_rel_a_abs(pos_actual_rick,x,y);
    uint32_t dir_dst = coord_abs_a_dir_fisica(pos_abs_dst.x, pos_abs_dst.y);
    
    //Mapear la nueva direccion
    mmu_mapPage(rcr3(), TASK_CODE, dir_dst, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage(rcr3(), TASK_CODE + PAGE_SIZE, dir_dst + PAGE_SIZE, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    //Chequear que hay en el destino x e y, buscando en el arreglo de posiciones de las tareas
    for (int i = 0; i < CANT_TASKS; i++){
        //Hay algun rick, algun morty, o una cronemberg en el dst?
        if ((posTask_actual[i].x == pos_abs_dst.x) && (posTask_actual[i].y == pos_abs_dst.y)){
            //Es Rick o Morty?
            if ((type[i]!=type[actual_task]) && (type[i] > 0 && type[i] <= MORTYD248)){
                //Actualizo variables para pantalla
                rick_and_mortys_control[i]=-1;
                endOfGame = true;
            } else{
                //Marco cronenbergs muerto
                cronenbergs[i-4] = -1;
            }
            // // La tarea muere
            // alive_tasks[i] = DEAD; //! CORRECCION: la tarea no debia morir por la portalGun
            break;
        }
    }
    // Movemos el Rick a la nueva posicion de memoria
    move_task(dir_fisica_actual_rick, TASK_CODE, rcr3());
    //Actualizo el array de posiciones
    posTask_actual[actual_task].x = pos_abs_dst.x;
    posTask_actual[actual_task].y = pos_abs_dst.y;
}

void move_task(uint32_t dir_fisica_src, uint32_t dir_virtual_dst, uint32_t cr3){
    //Hacemos un mapeo temporal
    mmu_mapPage(cr3, dir_fisica_src, dir_fisica_src, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage(cr3, dir_fisica_src + PAGE_SIZE, dir_fisica_src + PAGE_SIZE, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    
    //Realizamos la copia
    copy_task(dir_fisica_src, dir_virtual_dst);
    copy_task(dir_fisica_src + PAGE_SIZE, dir_virtual_dst + PAGE_SIZE);
    
    //Desmapeamos lo que acabamos de mapear
    mmu_unmapPage(cr3, dir_fisica_src);
    mmu_unmapPage(cr3, dir_fisica_src + PAGE_SIZE);
}

void mover_Morty(int32_t x, int32_t y, uint32_t cr3){
    // Morty solo o morty con Rick?
    uint32_t actual_task_morty;
    if (cr3 == rcr3()){
        actual_task_morty = actual_task;
    }else{
        actual_task_morty = type[actual_task]==RICKC137 ? 1 : 3; 
    }
    
    //Obtenemos la posicion actual del morty
    posicion pos_actual_morty = posTask_actual[actual_task_morty];
    //Guardamos la Dir Fisica de la posicion del morty actual
    uint32_t dir_fisica_actual_morty = coord_abs_a_dir_fisica(pos_actual_morty.x, pos_actual_morty.y);
    //Primero desmapeamos la tarea morty
    mmu_unmapPage(cr3, TASK_CODE);
    mmu_unmapPage(cr3, TASK_CODE + PAGE_SIZE);
    
    //Convertimos x e y a dir fisica del mundo cronenberg
    posicion pos_abs_dst = coord_rel_a_abs(pos_actual_morty,x,y);
    uint32_t dir_dst = coord_abs_a_dir_fisica(pos_abs_dst.x, pos_abs_dst.y);
    
    //Mapear la nueva direccion
    mmu_mapPage(cr3, TASK_CODE, dir_dst, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage(cr3, TASK_CODE + PAGE_SIZE, dir_dst + PAGE_SIZE, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    //Chequear que hay en el destino x e y, buscando en el arreglo de posiciones de las tareas
    for (int i = 0; i < CANT_TASKS; i++){
        //Hay algun rick, algun morty, o una cronemberg en el dst?
        if ((posTask_actual[i].x == pos_abs_dst.x) && (posTask_actual[i].y == pos_abs_dst.y)){
            //Es Rick o Morty?
            if ((type[i]!=type[actual_task_morty]) && (type[i] > 0 && type[i] <= MORTYD248)){
                //Actualizo variables para pantalla
                rick_and_mortys_control[i]=-1;
                endOfGame = true;
            } else{
                //Marco cronenbergs muerto
                cronenbergs[i-4] = -1;
            }
            // // La tarea muere
            // alive_tasks[i] = DEAD; //! CORRECCION: la tarea no debia morir por la portalGun
            break;
        }
    }
    // Movemos el Morty a la nueva posicion de memoria
    if (cr3 == rcr3()){
        move_task(dir_fisica_actual_morty, TASK_CODE, cr3); 
    }else{
        // COMO SOY kernel puedo hacer lo que se me pinta wacho
        uint32_t cr3_actual = rcr3();
        lcr3(cr3);
        move_task(dir_fisica_actual_morty, TASK_CODE, cr3); 
        lcr3(cr3_actual);
    }
    //Actualizo el array de posiciones
    posTask_actual[actual_task_morty].x = pos_abs_dst.x;
    posTask_actual[actual_task_morty].y = pos_abs_dst.y;
}

uint32_t obtener_cr3_Morty(){  
    //Obtengo el index de la GDT del Morty correspondiente al Rick que lo hace mover
    // |16| Morty C-137
    // |18| Morty D-248
    uint16_t gdt_idx_morty = GDT_IDX_BASE_TASKS + (type[actual_task]==RICKC137 ? 1 : 3);
    
    // Obtengo la entrada de la GDT del Morty
    gdt_entry* gdte_morty = &gdt[gdt_idx_morty];
    // Obtengo la direccion base donde esta la tss
    uint32_t dir_tss = gdte_morty->base_0_15 | gdte_morty->base_23_16 <<16 | gdte_morty->base_31_24<<24;

    // Obtengo la TSS de la tarea morty
    tss* tss_morty = (tss*) dir_tss;
    // Obtengo el CR3 de morty
    uint32_t cr3_morty = tss_morty->cr3;
    
    return cr3_morty;
}

void crear_portal(int32_t x, int32_t y){    
    //Actualizamos el portal - up = universe_of_portal
    uint8_t up = C137*(type[actual_task]==RICKC137 || type[actual_task]==MORTYC137) + 
                 D248*(type[actual_task]==RICKD248 || type[actual_task]==MORTYD248);
    
    //Funciones para el portal en pantalla
    if((posPortal_actual[up].x != PORTAL_NO_VISIBLE)&&(posPortal_actual[up].y != PORTAL_NO_VISIBLE)){
        posPortal_viejas[up].x = posPortal_actual[up].x;
        posPortal_viejas[up].y = posPortal_actual[up].y;
    }
    
    //Obtenemos la posicion actual
    posicion pos_actual = posTask_actual[actual_task];
    
    //Convertir x e y a dir fisica del mundo cronenberg
    posicion pos_abs_portal = coord_rel_a_abs(pos_actual,x,y);    
    uint32_t dir_portal = coord_abs_a_dir_fisica(pos_abs_portal.x, pos_abs_portal.y);
    
    //Actualizamos el portal
    posPortal_actual[up].x = pos_abs_portal.x;
    posPortal_actual[up].y = pos_abs_portal.y;
    
    //PRIMERO desmapear el portal anterior
    mmu_unmapPage(rcr3(), 0x8002000);
    mmu_unmapPage(rcr3(), 0x8002000+PAGE_SIZE);
    
    //Mapear la direccion del portal
    mmu_mapPage(rcr3(),0x08002000, dir_portal, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    mmu_mapPage(rcr3(),0x08002000+PAGE_SIZE, dir_portal+PAGE_SIZE, FLAG_MMU_RW_WRITE, FLAG_MMU_US_USER);
    
}

posicion coord_rel_a_abs(posicion centro, int32_t x, int32_t y){
    uint32_t new_x = 0;
    uint32_t new_y = 0;
    //Paso coordenas relativas a absolutas
    new_x = (centro.x+x)%80;
    new_y = (centro.y+y)%40;
    //TODO: CHEQUEAR SI ES EL VALOR CORRECTO Y CHEQUEAR LA DIR FISICA RESULTANTE
    posicion res = {new_x, new_y};
    return res;
}

uint32_t coord_abs_a_dir_fisica(uint32_t x, uint32_t y){
    return TASKS_MEMORY + (PAGE_SIZE*2)*(y*SIZE_M + x);
}

void rutina_138(uint32_t code){

    if((actual_task>=4) && (cronenbergs[actual_task-4] != -1)){
        cronenbergs[actual_task-4] = 1*(code==0xC137) + 2*(code==0xD248);
    } else{
        //Si es un Rick o Morty (porque actual_task<4)
        rick_and_mortys_control[actual_task] = 0*(code==0xC137) + 1*(code==0xD248);
    }
    // Por como implementamos las estructuras del juego:
    //   en el caso de que la llame un Rick o Morty
    //   no hace nada porque ya llevamos el puntaje en otro lado
}

void rutina_139(uint32_t *x, uint32_t *y){
    if (type[actual_task]==RICKC137 || type[actual_task]==RICKD248){
        //OBS: posTask_actual[actual_task+1] siempre va ser el Morty del Rick correspondiente
        *x = posTask_actual[actual_task+1].x - posTask_actual[actual_task].x; 
        *y = posTask_actual[actual_task+1].y - posTask_actual[actual_task].y;
    }
}

void screen_initTaskClocks(){
    for (uint8_t i = 0; i < 10*4; i+=4)
    {
        print_dec((i/4),2,21+i,42,(C_BG_BLACK | C_FG_DARK_GREY));
        print("|",21+i,43,(C_BG_BLACK | C_FG_WHITE));
        print_dec((i/4)+10,2,21+i,45,(C_BG_BLACK | C_FG_DARK_GREY));
        print("|",21+i,46,(C_BG_BLACK | C_FG_WHITE));
    }
}
void screen_incTaskClocks(){
    uint32_t i = actual_task;
    if((i>3 )&&(i<=13 )){
        ticks_clocks[i-4] = (1+ticks_clocks[i-4])%4;
        if(ticks_clocks[i-4]==0)      {print("|",21+((i-4)*4),43,(C_BG_BLACK | C_FG_WHITE));}
        else if(ticks_clocks[i-4]==1) {print("-",21+((i-4)*4),43,(C_BG_BLACK | C_FG_WHITE));}
        else if(ticks_clocks[i-4]==2) {print("\\",21+((i-4)*4),43,(C_BG_BLACK| C_FG_WHITE));}  
        else if(ticks_clocks[i-4]==3) {print("/",21+((i-4)*4),43,(C_BG_BLACK | C_FG_WHITE));}
    } else if(i>13){ 
        ticks_clocks[i-4] = (1+ticks_clocks[i-4])%4;
        if(ticks_clocks[i-4]==0)      {print("|",21+((i-14)*4),46,(C_BG_BLACK | C_FG_WHITE));}
        else if(ticks_clocks[i-4]==1) {print("-",21+((i-14)*4),46,(C_BG_BLACK | C_FG_WHITE));}
        else if(ticks_clocks[i-4]==2) {print("\\",21+((i-14)*4),46,(C_BG_BLACK| C_FG_WHITE));}  
        else if(ticks_clocks[i-4]==3) {print("/",21+((i-14)*4),46,(C_BG_BLACK | C_FG_WHITE));}
    }
}

void game_checkEndOfGame(){
    if(endOfGame) {
            for (int i = 0; i < 80; i++)
            {
                for (int j = 0; j < 50; j++)
                {   
                    if(score[C137]>score[D248]) print(" ",i,j,(C_BG_BLUE | C_FG_WHITE));
                    else if(score[C137]<score[D248]) print(" ",i,j,(C_BG_RED | C_FG_WHITE));
                    else print(" ",i,j,(C_BG_CYAN | C_FG_CYAN));
                }
            }
        while(1){
                for (int i = 0; i < 50; i++)
                {   
                    if(score[C137]>score[D248]){
                        if(i%2==0)print("GANO RICK C137",50,i,(C_BG_BLUE | C_FG_WHITE));
                        if(i%2==1)print("GANO RICK C137",50,i,(C_BG_BLUE | C_FG_WHITE));
                    }
                    else if(score[C137]<score[D248]) {
                        if(i%2==0)print("GANO RICK D248",50,i,(C_BG_RED | C_FG_WHITE));
                        if(i%2==1)print("              ",50,i,(C_BG_RED | C_FG_WHITE));
                    }
                    else {
                        if(i%2==0)print("TIE",50,i,(C_BG_CYAN | C_FG_WHITE));
                        if(i%2==1)print("TIE",50,i,(C_BG_CYAN | C_FG_CYAN));
                    }
                }
            }
    } 
}

void game_updateScore(){
    uint8_t auxC = 0;
    uint8_t auxD = 0;

    for (uint8_t i = 0; i < 20; i++){
       if(cronenbergs[i] == 1 ){
           auxC++;
       } else if (cronenbergs[i] == 2){
           auxD++;
       }
    }
    for (uint8_t i = 0; i < 4; i++){
       if(rick_and_mortys_control[i] == 0 ){
           auxC++;
       } else if (rick_and_mortys_control[i] == 1){
           auxD++;
       }
    }
    score[C137] = auxC;
    score[D248] = auxD;
    //Criterio fin de juego donde si un Equipo conquisto todas las mentes gana
    if((score[C137]==22) || (score[C137]==22)) {
        endOfGame = true;
    }    
    print_dec(auxC,8,4,44,(C_BG_BLUE | C_FG_WHITE));
    print_dec(auxD,8,68,44,(C_BG_RED | C_FG_WHITE)); 
}

void printRickC(uint8_t x, uint8_t y){
    print("R",x,y+1,(C_BG_BLUE | C_FG_WHITE));
}
void printMortyC(uint8_t x, uint8_t y){
    print("M",x,y+1,(C_BG_BLUE | C_FG_WHITE));
}
void printRickD(uint8_t x, uint8_t y){
    print("R",x,y+1,(C_BG_RED | C_FG_WHITE));
}
void printMortyD(uint8_t x, uint8_t y){
    print("M",x,y+1,(C_BG_RED | C_FG_WHITE));
}
void printRickCXOR(uint8_t x, uint8_t y){
    print("R",x,y+1,(C_BG_BLUE | C_FG_BLACK));
}
void printMortyCXOR(uint8_t x, uint8_t y){
    print("M",x,y+1,(C_BG_BLUE | C_FG_BLACK));
}
void printRickDXOR(uint8_t x, uint8_t y){
    print("R",x,y+1,(C_BG_RED | C_FG_BLACK));
}
void printMortyDXOR(uint8_t x, uint8_t y){
    print("M",x,y+1,(C_BG_RED | C_FG_BLACK));
}
void printCronenbergAlive(uint8_t x, uint8_t y){
    print("+",x,y+1,(C_BG_MAGENTA | C_FG_WHITE));
}
void printCronenbergDead(uint8_t x, uint8_t y,uint8_t i){
    
    if((i>3 )&&(i<=13 )){
        print("X",21+((i-4)*4),43,(C_BG_BLACK | C_FG_RED));
    } else if(i>13){ 
        print("X",21+((i-14)*4),46,(C_BG_BLACK | C_FG_RED));
    }
    print("X",x,y+1,(C_BG_BROWN | C_FG_RED));
}
void printCronenbergC(uint8_t x, uint8_t y){
    print("+",x,y+1,(C_BG_BLUE | C_FG_WHITE));
}
void printCronenbergD(uint8_t x, uint8_t y){
    print("+",x,y+1,(C_BG_RED | C_FG_WHITE));
}

void screen_drawTasks(){
    uint32_t px;
    uint32_t py;
    print(" ______   BORRAR MENSAJE EXCEPCION  _______ ",0,0,(C_FG_BLACK || C_FG_BLACK));
    for(int i=0; i < 2;i++){
        print("*",posPortal_viejas[i].x,posPortal_viejas[i].y+1,(C_BG_BROWN | C_FG_BROWN));
        if((posPortal_actual[i].x != PORTAL_NO_VISIBLE)&&(posPortal_actual[i].y != PORTAL_NO_VISIBLE)){
            print("*",posPortal_actual[i].x,posPortal_actual[i].y+1,(C_FG_CYAN | C_BG_BLUE));
        }
    }
    for (uint8_t i = 0; i < 24; i++)
    {
        //Pintamos la vieja del mapa 
        print_dec(0,1,posTask_viejas[i].x,posTask_viejas[i].y+1,(C_BG_BROWN | C_FG_BROWN));
        posTask_viejas[i] = posTask_actual[i];
        px = posTask_actual[i].x;
        py = posTask_actual[i].y;
        switch (i){
            case 0:
                if ((type[i]==RICKC137)&&(rick_and_mortys_control[i]==0)) printRickC(px,py);
                if ((type[i]==RICKC137)&&(rick_and_mortys_control[i]==1)) printRickCXOR(px,py);
                break;
            case 1:
                if ((type[i]==MORTYC137)&&(rick_and_mortys_control[i]==0)) {printMortyC(px,py);}
                if ((type[i]==MORTYC137)&&(rick_and_mortys_control[i]==1)) {printMortyCXOR(px,py);}
                break;
            case 2:
                if ((type[i]==RICKD248)&&(rick_and_mortys_control[i]==1)) {printRickD(px,py);}
                if ((type[i]==RICKD248)&&(rick_and_mortys_control[i]==0)) {printRickDXOR(px,py);}
                break;
            case 3:
                if ((type[i]==MORTYD248)&&(rick_and_mortys_control[i]==1)) {printMortyD(px,py);}
                if ((type[i]==MORTYD248)&&(rick_and_mortys_control[i]==0)) {printMortyDXOR(px,py);}
                break;
            default:
                if ((type[i]==CRONENBERG)&&(cronenbergs[i-4]==0)) printCronenbergAlive(px,py);
                if ((type[i]==CRONENBERG)&&(cronenbergs[i-4]==-1)) printCronenbergDead(px,py,i);
                if ((type[i]==CRONENBERG)&&(cronenbergs[i-4]==1)) printCronenbergC(px,py);
                if ((type[i]==CRONENBERG)&&(cronenbergs[i-4]==2)) printCronenbergD(px,py);
                break;  
            }
    }
}