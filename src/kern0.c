#include "types.h"
#define VGABUF ((volatile char *) 0xb8000)

static void
vga_write(const char *s, int8_t linea, uint8_t color) {
    linea = linea % 24;
    int row_offset = linea * 160;
    volatile char* buf = VGABUF;
    for(int i = 0; s[i] != '\0'; i++) {
        buf[row_offset + 2 * i] = s[i];
        buf[row_offset + 2 * i + 1] = color; 
    }

}

void comienzo(void) {

    vga_write("despacito 2", 26, 47);

    while (1)
        asm("hlt");
}

#include <stdlib.h>

