// boot.S

#include "multiboot.h"

#define KSTACK_SIZE 8192

.align 4
multiboot:
    .long MULTIBOOT_HEADER_MAGIC
    .long 0
    .long -(MULTIBOOT_HEADER_MAGIC)

.globl _start
_start:
    // Paso 1: Configurar el stack antes de llamar a kmain.
    movl $0, %ebp
    movl $kstackend, %esp
    push %ebp

    // Paso 2: pasar la información multiboot a kmain. Si el
    // kernel no arrancó vía Multiboot, se debe pasar NULL.
    //
    // Usar una instrucción de comparación (TEST o CMP) para
    // comparar con MULTIBOOT_BOOTLOADER_MAGIC, pero no usar
    // un salto a continuación, sino una instrucción CMOVcc
    // (copia condicional).
    // ...

    /* from multiboot.h: 
    ** This should be in %eax. 
    ** #define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002 
    */

    movl $0, %edx
    test MULTIBOOT_HEADER_MAGIC, %eax
    cmove %ebx, %edx
    push %edx
    call kmain
    
halt:
    hlt
    jmp halt

.data
.p2align 12
kstack:
    .space KSTACK_SIZE
kstackend:
