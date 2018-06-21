#include "decls.h"

#define COUNTLEN 20
#define TICKS (1ULL << 15)
#define DELAY(x) (TICKS << (x))
#define USTACK_SIZE 4096

static volatile char *const VGABUF = (volatile void *) 0xb8000;

static uintptr_t esp;
static uint8_t stack1[USTACK_SIZE] __attribute__((aligned(4096)));
static uint8_t stack2[USTACK_SIZE] __attribute__((aligned(4096)));

static void exit() {
    uintptr_t tmp = esp;
    esp = 0;
    if (tmp)
        task_swap(&tmp);
}

static void yield() {
    if (esp)
        task_swap(&esp);
}

static void contador_yield(unsigned lim, uint8_t linea, char color) {
    char counter[COUNTLEN] = {'0'};  // ASCII digit counter (RTL).

    while (lim--) {
        char *c = &counter[COUNTLEN];
        volatile char *buf = VGABUF + 160 * linea + 2 * (80 - COUNTLEN);

        unsigned p = 0;
        unsigned long long i = 0;

        while (i++ < DELAY(6))  // Usar un entero menor si va demasiado lento.
            ;

        while (counter[p] == '9') {
            counter[p++] = '0';
        }

        if (!counter[p]++) {
            counter[p] = '1';
        }

        while (c-- > counter) {
            *buf++ = *c;
            *buf++ = color;
        }

        yield();
    } 
//    exit();
}

void contador_run() { 
    // Inicializar al *tope* de cada pila. 
    uintptr_t *a = (uintptr_t*) (stack1 + USTACK_SIZE);
    uintptr_t *b = (uintptr_t*) (stack2 + USTACK_SIZE);

    *(--a) = 0x2F;
    *(--a) = 0;
    *(--a) = 100;

    *(--b) = 0x4F;
    *(--b) = 1;
    *(--b) = 10;
    *(--b) = (uintptr_t) exit;
    *(--b) = (uintptr_t) contador_yield;
    *(--b) = 0;
    *(--b) = 0;
    *(--b) = 0;
    *(--b) = 0;


    esp = (uintptr_t) b;
    
    task_exec((uintptr_t) contador_yield,(uintptr_t)a);
}
