#include "decls.h"
#include "multiboot.h"
#include "string.h"
#include "interrupts.h"

#define USTACK_SIZE 4096

static uint8_t stack1[USTACK_SIZE] __attribute__((aligned(4096)));
static uint8_t stack2[USTACK_SIZE] __attribute__((aligned(4096)));

void two_stacks_c() {
    // Inicializar al *tope* de cada pila.
    uintptr_t *a = (uintptr_t*) (stack1 + USTACK_SIZE);
    uintptr_t *b = (uintptr_t*) (stack2 + USTACK_SIZE);

    // Preparar, en stack1, la llamada:
//    vga_write("vga_write() from stack1", 15, 0x57);

    *(--a) = 0x57;
    *(--a) = 15;
    *(--a) = (uintptr_t) "vga_write() from stack1";

    // AYUDA 1: se puede usar alguna forma de pre- o post-
    // incremento/decremento, según corresponda:
    //
    //     *(a++) = ...
    //     *(++a) = ...
    //     *(a--) = ...
    //     *(--a) = ...

    // AYUDA 2: para apuntar a la cadena con el mensaje,
    // es suficiente con el siguiente cast:
    //
    //   ... a ... = (uintptr_t) "vga_write() from stack1";

    // Preparar, en s2, la llamada:
   // vga_write("vga_write() from stack2", 16, 0xD0);

    // AYUDA 3: para esta segunda llamada, usar esta forma de
    // asignación alternativa:
    b -= 3;
    b[0] = (uintptr_t) "vga_write() from stack2";
    b[1] = 16;
    b[2] = 0xD0;

    // Primera llamada usando task_exec().
    task_exec((uintptr_t) vga_write, (uintptr_t) a);

    // Segunda llamada con ASM directo. Importante: no
    // olvidar restaurar el valor de %esp al terminar, y
    // compilar con: -fasm -fno-omit-frame-pointer.
    asm("movl %0, %%esp; call *%1; movl %%ebp, %%esp"
        : /* no outputs */
        : "r"(b), "r"(vga_write));
}

void kmain(const multiboot_info_t *mbi) {
    int8_t linea;
    uint8_t color;
    
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

    /* A remplazar por una llamada a two_stacks(), 
     * definida en stacks.S. 
     * */
    /*
    vga_write("vga_write() from stack1", 12, 0x17);
    vga_write("vga_write() from stack2", 13, 0x90);
    */
    two_stacks();
    two_stacks_c();

    contador_run();

    vga_write("antes del 2", 18, 0xE0);
    vga_write2("Funciona vga_write2?", 18, 0xE0);

    // Código ejercicio kern2-idt.
    idt_init();   // (a)
    irq_init();
    asm("int3");  // (b)

    asm("div %4"
    : "=a"(linea), "=c"(color)
    : "0"(18), "1"(0xE0), "b"(1), "d"(0));

    vga_write("Funciona vga_write2?", linea, color);

    asm("hlt");
}

