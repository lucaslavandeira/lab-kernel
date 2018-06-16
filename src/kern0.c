#include "decls.h"
#include "multiboot.h"
#include "string.h"

unsigned char __attribute__ ((aligned (4096))) kstack[8192];


void kmain(const multiboot_info_t *mbi) {
    vga_write("kern2 loading.............", 8, 0x70);

    if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
        char buf[256] = "cmdline: ";
        char *cmdline = (void *) mbi->cmdline;

        strlcat(buf, cmdline, 256);
        vga_write(buf, 9, 0x07);
    }

    asm("hlt");
}
