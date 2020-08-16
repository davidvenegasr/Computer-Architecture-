/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "sched.h"

void sched_init() {
    
    //Inicializo las TSSs
    tss_init_others();
    
    //Inicializo otros parametros
    for(int i=0; i < CANT_TASKS;i++){
        posTask_actual[i].x = getRandomX(i);
        posTask_actual[i].y = getRandomY(i);  
        posTask_viejas[i].x = getRandomX(i);
        posTask_viejas[i].y = getRandomY(i);  
    }
    for(int i=0; i < 2;i++){
        posPortal_actual[i].x = PORTAL_NO_VISIBLE;
        posPortal_actual[i].y = PORTAL_NO_VISIBLE;   
    }
    for(int i=0; i < 2;i++){
        posPortal_viejas[i].x = 1;
        posPortal_viejas[i].y = 1;  
    }

    for (int i = 0; i < 24; i++){
        switch (i){
            case 0:
                type[i] = RICKC137;
                break;
            case 1:
                type[i] = MORTYC137;
                break;
            case 2:
                type[i] = RICKD248;
                break;
            case 3:
                type[i] = MORTYD248;
                break;
            default:
                type[i] = CRONENBERG;
                break;
        }
    }
    //Inicializar valor OBS: se setean en game.c
    for(int i=0; i < 24;i++){
        alive_tasks[i]= 0;
    }
    
    score[C137] = 2;
    score[D248] = 2;
    
    //Comienza en la ultima asi el sched puede iniciar en la primera
    actual_task = 23;
    debug_ON = 0;
    
    contadorMorty[C137]=0;
    contadorMorty[D248]=0;
}

int16_t sched_nextTask() {
    // ACTUALIZO actual_task al correspondiente
    actual_task = (actual_task + 1) % 24;
    int j = 0;
    // SI esta muerta..
    while (!alive_tasks[actual_task]){
        //Pruebo con el siguiente indice
        actual_task = (actual_task + 1) % 24;
        
        j++;
        if (j>24) return 14;
    }
    int16_t selector = ((GDT_IDX_BASE_TASKS + actual_task)<<3) | 0b11;
    return selector; // devolvemos el indice de la GDT
}

void debugger(uint8_t scanCode){
        //breakpoint();
        if(!(scanCode & 0x80)){
        if (0x15 ==scanCode){ //Revisamos si es igual a Y
            //Si esta en 0 activa el modo debug pasandolo a 1 
            char* msg = "DEBUG MODE ON";
            if(debug_ON==0){
                debug_ON = 1;
                print(msg, 34, 49, C_FG_WHITE | C_BG_BLACK);   //! CORREGIDO: no se indicaba si el modo debug estaba activado
            }else{ //Si esta activado, lo desactiva pasando a 0
                debug_ON = 0;
                print(msg, 34, 49, C_FG_BLACK | C_BG_BLACK);  //Esconde cartelito modo debug
            } //Si esta el debugger en pantalla lo maneja una rutina de asm
        }
    }
}

void kill_task(){
    alive_tasks[actual_task] = DEAD;
}

//! CORRECCION: el modo debug no mostraba bien los registros
uint8_t has_errorcode(uint32_t isr){
    uint8_t res = 0;
    switch(isr){
        case 8:case 10:case 11:case 12:case 13:case 14:case 17:case 30:
            res = 1;
            break;
        default:
            res = 0;
            break;
    }
    return res;
}
