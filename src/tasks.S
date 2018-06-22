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

    push %ecx
    push %edx
    push %eax

    call vga_write

    leave
    ret

.globl task_swap
// Pone en ejecución la tarea cuyo stack está en `*esp`, cuyo
// valor se intercambia por el valor actual de %esp. Guarda y
// restaura todos los callee-saved registers.
//void task_swap(uintptr_t *esp);

/* Implementar en tasks.S la función task_swap(). Como se indicó arriba, 
esta función recibe como parámetro la ubicación en memoria de la variable 
esp que apunta al stack de la tarea en "stand-by". La responsabilidad de 
esta función es:

	guardar, en el stack de la tarea actual, los registros que son callee-saved
        Segun https://wiki.skullsecurity.org/index.php?title=Registers, los 
        callee-saved regs son ebx, esi, edi, ebp.

	cargar en %esp el stack de la nueva tarea, y guardar en la variable 
	esp el valor previo de %esp

	restaurar, desde el nuevo stack, los registros que fueron guardados por 
	una llamada previa a task_swap(), y retornar (con la instrucción ret) 
	a la nueva tarea.

Para esta función, se recomienda no usar el preámbulo, esto es, no modificar 
el registro %ebp al entrar en la función.
*/
task_swap:
    push %ebx
    push %esi
    push %edi
    push %ebp
    
    mov  20(%esp), %edx 
    mov  %esp, %eax
    mov  (%edx), %esp
    mov  %eax, (%edx)

    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
    ret

