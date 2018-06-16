#include "decls.h"
#define VGABUF ((volatile char *) 0xb8000)

void vga_write(const char *s, int8_t linea, uint8_t color) {
    linea = linea % 24;
    int row_offset = linea * 160;
    volatile char* buf = VGABUF;
    for(int i = 0; s[i] != '\0'; i++) {
        buf[row_offset + 2 * i] = s[i];
        buf[row_offset + 2 * i + 1] = color; 
    }

}

static size_t int_width(uint64_t val) {
    size_t result = 1;
    while (val /= 10) {
        result++;
    }

    return result;
}

bool fmt_int(uint64_t val, char *s, size_t bufsize) {
    size_t l = int_width(val);

    if (l >= bufsize)
        return false;

    s += l;
    
    uint8_t zero = '0';

    while (val) {
        *--s = zero + (val % 10);
        val /= 10;
    }
    return true;
}


void __attribute__((regparm(2)))
vga_write_cyan(const char *s, int8_t linea) {
    vga_write(s, linea, 0xB0);
}
