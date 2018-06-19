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
