#include "decls.h"
#include "multiboot.h"
#include "string.h"
#include "interrupts.h"


void kmain(const multiboot_info_t *mbi) {
    vga_write("kern2 loading.............", 8, 0x70);

    if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
        char buf[256] = "cmdline: ";
        char *cmdline = (void *) mbi->cmdline;

        strlcat(buf, cmdline, 256);
        vga_write(buf, 9, 0x07);
    }

    char mem[256] = "Physical memory: ";
    char tmp[64] = "";

    if (fmt_int(mbi->mem_upper - mbi->mem_lower, tmp, sizeof tmp)) {
        strlcat(mem, tmp, sizeof mem);
        strlcat(mem, "MiB total", sizeof mem);
    }

    vga_write(mem, 10, 0x07);

    // Código ejercicio kern2-idt.
    idt_init();   // (a)
    asm("int3");  // (b)

    asm("hlt");
}
