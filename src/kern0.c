#include "decls.h"
#include "multiboot.h"


unsigned char __attribute__ ((aligned (4096))) kstack[8192];


void kmain(const multiboot_info_t *mbi) {
    vga_write("kern2 loading.............", 8, 0x70);

    asm("hlt");
}
