#define PIC1 0x20
#define ACK_IRQ 0x20


.globl breakpoint
breakpoint:
        // (1) Guardar registros.
        push %eax
        push %edx
        push %ecx

        // (4) Restaurar registros.
        pop %ecx
        pop %edx
        pop %eax

        // (5) Finalizar ejecución del manejador.
        iret


.globl ack_irq
ack_irq:
        // Indicar que se manejó la interrupción.
        movl $ACK_IRQ, %eax
        outb %al, $PIC1
        iret


.data
breakpoint_msg:
        .asciz "Hello, breakpoint"

.globl timer_asm
timer_asm:
        // (1) Guardar registros.
        push %eax
        push %edx
        push %ecx

        call timer

        // (4) Restaurar registros.
        pop %ecx
        pop %edx
        pop %eax
        jmp ack_irq
