.globl breakpoint
breakpoint:
        // (1) Guardar registros.
        push %eax
        push %ecx
        push %edx

        // (2) Preparar argumentos de la llamada.
        push $0x07
        push $20
        push $breakpoint_msg

        // (3) Invocar a vga_write2()
        call vga_write
        // (4) Restaurar registros.
        pop %edx
        pop %ecx
        pop %eax

        // (5) Finalizar ejecución del manejador.
        iret

.data
breakpoint_msg:
        .asciz "Hello, breakpoint"
