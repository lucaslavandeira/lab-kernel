# Ej: kern2-task
``` 27         // Registros para apuntar a stack1 y stack2.                            
 28         mov $stack1_top, %eax                                                   
 29         mov $stack2_top, %ecx   // Decidir qué registro usar. 
```
Se decidió usar el registro ecx, ya que el resto de los registros disponibles 
para uso general no acompañaban el correcto funcionamiento de la tarea.
``` 31         // Cargar argumentos a ambos stacks en paralelo. Ayuda:                 
 32         // usar offsets respecto a %eax ($stack1_top), y lo mismo               
 33         // para el registro usado para stack2_top.                              
 34         movl $0x17, -4(%eax)                                                    
 35         movl $0x90, -4(%edi)                                                    
 36                                                                                 
 37         movl $12, -8(%eax)                                                      
 38         movl $13, -8(%edi)                                                      
 39                                                                                 
 40         movl $msg1, -12(%eax)                                                   
 41         movl $msg2, -12(%edi) 
```
Para cargar los argumentos, se mueve el dato a pasar como argumento al stack,
restando 4 bytes cada vez.

```  49         // Restaurar stack original. ¿Es %ebp suficiente?                       
 50         movl %ebp, %esp 
```
`%ebp` es suficiente ya que....... [completar]

# Ej: kern2-exec
# Ej: kern2-exit
> En la función contador_run() del ejercicio anterior, se configuran ambos contadores con el mismo número de iteraciones. Reducir ahora el número de iteraciones del segundo contador, y describir qué tipo de error ocurre.
qemu se rompe con el siguiente mensaje: *qemu: fatal: Trying to execute code outside RAM or ROM at 0x21fc0039*. 


