/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "screen.h"

void printScanCode(uint8_t scanCode){
    if(!(scanCode & 0x80)){
        if ((0x2 <= scanCode) & (scanCode <= 0xA) ){
            print_hex(scanCode-1,2,80-2,0, C_BG_BLACK| C_FG_WHITE);
        }
    }
}

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    int32_t i;
    for (i = 0; text[i] != 0; i++) {
        p[y][x].c = (uint8_t) text[i];
        p[y][x].a = (uint8_t) attr;
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    uint32_t i;
    uint8_t letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        uint32_t resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    int32_t i;
    uint8_t hexa[8];
    uint8_t letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void screen_drawBox(uint32_t fInit,
                    uint32_t cInit,
                    uint32_t fSize,
                    uint32_t cSize,
                    uint8_t character,
                    uint8_t attr ) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    uint32_t f;
    uint32_t c;
    for (f = fInit; f < fInit+fSize; f++) {
    for (c = cInit; c < cInit+cSize; c++) {
          p[f][c].c = character;
          p[f][c].a = attr;
    }}
}



void screen_drawBrown(){
    screen_drawBox(1,0,40,80,'A', (C_FG_BROWN | C_BG_BROWN));
}

void pintarDebugger(uint32_t exception_id, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint32_t esp, uint32_t eip, uint32_t cs, uint32_t ds, uint32_t es, uint32_t fs, uint32_t gs, uint32_t ss, uint32_t cr0, uint32_t cr1, uint32_t cr2, uint32_t cr3, uint32_t cr4, uint32_t eflags){
    
    char * excepcion = "UNKNOWN EXCEPTION";                            
    char * excepciones[] = {"0x0 - Divide-by-zero Error", 
                            "0x1 - Debug", "0x2 - Non-maskable Interrupt", 
                            "0x3 - Breakpoint", 
                            "0x4 - Overflow", 
                            "0x5 - Bound Range Exceeded", 
                            "0x6 - Invalid Opcode", 
                            "0x7 - Device Not Available",
                            "0x8 - Double Fault",
                            "0x9 - Coprocessor Segment Overrun", 
                            "0xA - Invalid TSS", "0xB - Segment Not Present", 
                            "0xC - Stack-Segment Fault", 
                            "0xD - General Protection Fault", 
                            "0xE - Page Fault", 
                            "0xF - (Reserved)", 
                            "0x10 - x87 Floating-Point Exception",
                            "0x11 - Alignment Check",
                            "0x12 - Machine Check",
                            "0x13 - SIMD Floating-Point Exception",
                            "0x14 - Virtualization Exception"};

    if (exception_id < CANT_EXCEPCIONES) excepcion = excepciones[exception_id];
    
    screen_drawBox(1, 22, 40, 36, 0, C_BG_BLACK);
    screen_drawBox(3, 23, 37, 34, 0, C_BG_LIGHT_GREY);
    screen_drawBox(1, 23, 1, 34, 0, C_BG_RED);
    print(excepcion, 23, 1, C_FG_WHITE| C_BG_RED);

    print("eax:", 24, 4, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(eax, 8, 29, 4, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("ebx:", 24, 6, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(ebx, 8, 29, 6, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("ecx:", 24, 8, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(ecx, 8, 29, 8, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("edx:", 24, 10, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(edx, 8, 29, 10, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("esi:", 24, 12, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(esi, 8, 29, 12, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("edi:", 24, 14, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(edi, 8, 29, 14, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("ebp:", 24, 16, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(ebp, 8, 29, 16, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("esp:", 24, 18, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(esp, 8, 29, 18, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("eip:", 24, 20, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(eip, 8, 29, 20, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("cs:", 25, 22, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(cs,8, 29, 22, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("ds:", 25, 24, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(ds, 8, 29, 24, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("es:", 25, 26, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(es, 8, 29, 26, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("fs:", 25, 28, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(fs, 8, 29, 28, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("gs:", 25, 30, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(gs, 8, 29, 30, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("ss:", 25, 32, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(ss, 8, 29, 32, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("eflags:", 25, 35, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(eflags, 8, 33, 35, C_FG_WHITE | C_BG_LIGHT_GREY);

    // derecha
    print("cr0:",39, 9, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(cr0, 8, 44, 9, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("cr2:", 39, 11, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(cr2, 8, 44, 11, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("cr3:", 39, 13, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(cr3, 8, 44, 13, C_FG_WHITE | C_BG_LIGHT_GREY);

    print("cr4:", 39, 15, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(cr4, 8, 44, 15, C_FG_WHITE | C_BG_LIGHT_GREY);

    // printeamos stack
    uint32_t * stack = (uint32_t*) esp;
    print("stack:", 39, 26, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(*stack, 8, 39, 28, C_FG_WHITE | C_BG_LIGHT_GREY);
    print_hex(*(stack+0x20), 8, 39, 29, C_FG_WHITE | C_BG_LIGHT_GREY);
    print_hex(*(stack+0x40), 8, 39, 30, C_FG_WHITE | C_BG_LIGHT_GREY);
    print_hex(*(stack+0x60), 8, 39, 31, C_FG_WHITE | C_BG_LIGHT_GREY);

}
