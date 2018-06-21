## Lab kern2

Lucas Lavandeira
Matías Rozanec

## Creación de stacks en el kernel

### kern2-stack

>Explicar: ¿qué significa “estar alineado”?

Una variable o array de variables está alineada en memoria a X bits cuando su posición inicial es un múltiplo de X. Por ejemplo estar alineado a 4 bytes significa que su dirección de memoria es un múltiplo de 4 (0, 4, 8, ...). Esto por lo general se hace para tomar ventaja de como el hardware maneja posiciones de memoria (la lectura de posiciones alineadas al tamaño de la palabra de la arquitectura suele ser más eficiente). 

Alinear una variable por lo general significa agregar suficientes bytes con valor nulo hasta que quede la posición incial de la misma en donde la queramos. 

>Mostrar la sintaxis de C/GCC para alinear a 32 bits el arreglo kstack anterior.

GCC provee el _atributo_ `aligned` para especificar a cuantos bytes (no bits) se quiere alinear alguna variable. Para alinear `kstack` a 32 bits, se declararía de la siguiente manera:

`unsigned char __attribute__ ((aligned (4))) kstack[8192];`

>¿A qué valor se está inicializando kstack? ¿Varía entre la versión C y la versión ASM? (Leer la documentación de as sobre la directiva .space.)

Según la directiva `.space` de x86, `.space 8192` toma los próximos 8192 bytes y los inicializa a cero. Declarar el arreglo en C de la manera que lo hicimos no inicializa los contenidos, y deja basura en el stack. Esto es aceptable puesto que el stack se lee solo a través del stack pointer que siempre apunta a la dirección con el último valor escrito.

>Explicar la diferencia entre las directivas .align y .p2align de as, y mostrar cómo alinear el stack del kernel a 4 KiB usando cada una de ellas.

`.align X` declara la posición de la siguiente instrucción a estar alineada a X bytes. `.p2align` declara la posición de la siguiente instrucción a estar alineada a 2^X bytes.

El stack alineado a 4KiB con `.align`:
```
.align 4096
kstack:
    .space 8192
```

Con `.p2align`
```
.p2align 12
kstack:
    .space 8192
```


### kern2-cmdline

>Mostrar cómo implementar la misma concatenación, de manera correcta, usando strncat(3).

Se implementaría la función de la misma manera (???)

>Explicar cómo se comporta strlcat(3) si, erróneamente, se declarase buf con tamaño 12. ¿Introduce algún error el código?

Termina ocurriendo que se concatena la línea de comandos al buffer hasta llegar a los 12 caracteres. No ocurre comportamiento no deseado como sería un buffer overflow.

>Compilar el siguiente programa, y explicar por qué se imprimen dos líneas distintas, en lugar de la misma dos veces:

La función `static void printf_sizeof_buf(char buf[256])` acepta un parámetro de tipo `char[]`, que deceptivamente en C el compilador trata como si fuera equivalente a declararlo como `char*`. `sizeof` de un argumento de tipo `char[]` es el tamaño del array, pero `sizeof` de un argumento de tipo `char*` es el tamaño del puntero, es decir 4 bytes en arquitecturas de 32 bits, u 8 en 64 bits. En el `printf` de la llamada a la función, se imprimen solamente esa cantidad de bytes, debido a `sizeof` devolviendo el tamaño del puntero en vez del del arreglo.
