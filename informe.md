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



## Interrupciones: reloj y teclado

### kern2-idt

>¿Cuántos bytes ocupa una entrada en la IDT?

Según la especificación de la arquitectura x86 referenciada, cada entrada de es de 8 bytes.

>¿Cuántas entradas como máximo puede albergar la IDT?

La IDT es un array de, a lo sumo, 256 entradas. Las primeras 32 están reservadas por la arquitectura, de la 32 en adelante son usables para interrupciones por software (las que nos interesan).

>¿Cuál es el valor máximo aceptable para el campo limit del registro IDTR?

`limit` del registro IDTR tiene un tamaño de 16 bytes, y actúa como un offset a una dirección base de la IDT, de manera similar a las páginas de memoria (frame + offset). Para una IDT de N entradas, el campo limit puede valer a lo sumo 8N - 1, para acceder al último byte de la última entrada.

>Indicar qué valor exacto tomará el campo limit para una IDT de 64 descriptores solamente.

N = 64, 8N = 512 bytes, a lo sumo el campo limit tendrá el valor 8N - 1, 511

>Consultar la sección 6.1 y explicar la diferencia entre interrupciones (§6.3) y excepciones (§6.4).

Las interrupciones son eventos programados a través de la IDT, son señales que el hardware es programado para enviar al programa en ejecución actual y frenarlo para manejarlas. Pueden ser y son manejadas por software. Las excepciones son similares pero tienen como única fuente a las condiciones de error que pueden lanzar las instrucciones, como por ejemplo una división por cero. No pueden ser lanzadas manualmente por software, aunque sí se puede decidir qué hacer frente a ellas (asignarles un handler).

### kern2-isr

Versión A (usando `iret`):

Antes de la interrupción:

Disassembly:
```
   ...
   0x001006d5 <+196>:	lea    0x14c(%esp),%eax
   0x001006dc <+203>:	push   %eax
   0x001006dd <+204>:	call   0x1004fd <vga_write>
   0x001006e2 <+209>:	call   0x10007c <idt_init>
=> 0x001006e7 <+214>:	int3   
   0x001006e8 <+215>:	hlt    
   ...
```
Estamos parados en `kmain` justo antes de lanzar la interrupción.

Valores de registros:
```
(gdb) print $esp
$1 = (void *) 0x104d98
(gdb) x/xw $esp
0x104d98:	0x00104ee8
(gdb) print $cs
$2 = 8
(gdb) print $eflags
$3 = [ PF AF ]
(gdb) print/x $eflags
$4 = 0x16
```

Luego ejecuto la instrucctión de interrupt:
```
(gdb) stepi
breakpoint () at idt_entry.S:4
4	        test %eax, %eax
1: x/i $pc
=> 0x10002f <breakpoint+1>:	test   %eax,%eax
(gdb) print $esp
$5 = (void *) 0x104d8c
```

El stack avanzó 0x104d98 - 0x104d8c = 12 bytes, o 3 posiciones de 4 bytes (1 word). Vemos como se pasó a ejecutar el handler asignado a la interrupción `3` de manera "atómica", sin instrucción intermedia. Vemos los contenidos del stack.

```
(gdb) x/3wx $sp
0x104d8c:	0x001006e8	0x00000008	0x00000016
```
Según la especificación de la arquitectura: 
"b. The processor then saves the current state of the EFLAGS, CS, and EIP registers on the new stack"

En efecto, podemos ver los valores 0x16 (flags), 0x8 (CS), y 0x001006e8 (la próxima instrucción a ejecutar luego del `int`) fueron pusheados al lanzarse la interrupción.

Paso siguiente ejecutamos el `test` de la función `breakpoint`:
```
(gdb) stepi
5	        iret
1: x/i $pc
=> 0x100031 <breakpoint+3>:	iret   
(gdb) print $eflags
$15 = [ PF ]
(gdb) print/x $eflags
$16 = 0x6
```

Podemos apreciar como cambió el registro de flags luego del `test`. Luego, ejecutamos la instrucción de retorno:

```
(gdb) stepi
kmain (mbi=0x9500) at kern0.c:32
32	    asm("hlt");
1: x/i $pc
=> 0x1006e8 <kmain+215>:	hlt    
(gdb) print $eflags
$17 = [ PF AF ]
(gdb) print/x $eflags
$18 = 0x16
(gdb) print $sp
$19 = (void *) 0x104d98

```

Podemos comprobar que se volvió al estado anterior de la interrupción, levantando del stack los valores guardados de los flags.

Ahora si usamos `ret` en vez de `iret` se puede ver que estos valores (`$eflags` + `$cs`) puestos en el stack por la interrupción no son levantados a la vuelta de la ejecución de `breakpoint`, es decir los flags no fueron reiniciados a sus valores anteriores, y de hecho siguen en el stack:

```
breakpoint () at idt_entry.S:4
4	        test %eax, %eax
(gdb) stepi
breakpoint () at idt_entry.S:5
5	        ret
(gdb) stepi
kmain (mbi=0x107000) at kern0.c:32
32	    asm("hlt");
(gdb) print $eflags
$1 = [ PF ]
(gdb) print/x $eflags
$2 = 0x6
(gdb) x/3wx $sp
0x104d90:	0x00000008	0x00000016	0x00104ee8
```

`iret` es la instrucción "RETurn from Interrupt", y hace exactamente lo que podemos ver, levantar la instrucción de retorno del tope del stack, y también los valores de los registros $eflags, y $cs. `ret` es el return normal, y solo levanta la instrucción de retorno.

### kern2-div

>Explicar el funcionamiento exacto de la línea asm(...) del punto anterior:

>¿qué cómputo se está realizando?

La instrucción `div` agarra el número representado por los 64 bits entre `eax` y `edx` en ese orden, siendo los 32 bits más significicativos de `eax` y los 32 menos de `edx`, y los usa como dividendo en una operación de división entera, siendo el divisor el registro argumento de entrada. El cociente de la división es escrita a `eax`, y el resto a `edx`. En esta instrucción se divide por `%4`, que contando los argumentos en orden, corresponde al registro `b` (`ebx`).

>¿de dónde sale el valor de la variable color?
Las variables son dadas un valor de la salida del `asm`. El componente `: "=a"(linea), "=c"(color)` se interpreta como: guardar el valor final de los registros `a` (`eax`) en la variable `linea` de C, y el de `c` (`ecx`) en `color`.

>¿por qué se da valor 0 a %edx?
Para inicializarlo a un valor conocido, puesto que es un registro no utiliazado por la función actual, puede llegar a contener algún valor basura que nos altere la instrucción que queremos ejecutar.

