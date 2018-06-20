.text
.globl task_exec
task_exec:
    // preambulo estandar 
    push %ebp
    movl %esp, %ebp 

    movl 12(%ebp), %esp
    call *8(%ebp)

/* restauro stack */
movl %ebp, %esp
    leave
    ret

.globl vga_write2
    vga_write2:
    push %ebp
    movl %esp, %ebp

    // ...
    movl 12(%esp), %eax
    movl 8(%esp), %ebx
    movl 4(%esp), %ecx
    call vga_write

    leave
    ret
