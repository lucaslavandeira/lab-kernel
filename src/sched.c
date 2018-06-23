#include "decls.h"
#include "sched.h"

#define MAX_TASK 10

static struct Task Tasks[MAX_TASK];
static struct Task *current;

void sched_init() {
    current = &Tasks[0];

    for (int i = 0; i < MAX_TASK; i++) {
        Tasks[i].status = FREE;
        Tasks[i].frame = 0;
    }

    current->status = RUNNING;
}

void spawn(void (*entry)(void)) {
    for(int i = 0; i < MAX_TASK; i ++) {
        if (Tasks[i].status == FREE) {
            Tasks[i].status = READY;

            size_t frame_size = sizeof(struct TaskFrame);
            uint8_t* stack_top = &Tasks[i].stack[4096];

            Tasks[i].frame = stack_top - frame_size;

            Tasks[i].frame->edi = 0;
            Tasks[i].frame->esi = 0;
            Tasks[i].frame->ebp = 0;
            Tasks[i].frame->esp = 0;
            Tasks[i].frame->eax = 0;
            Tasks[i].frame->ecx = 0;
            Tasks[i].frame->edx = 0;
            Tasks[i].frame->ebx = 0;

            Tasks[i].frame->eflags = 0x0200;  // flag IF = 1

            Tasks[i].frame->cs = 8;     // Multiboot siempre pone '8' 
                                        // como CS (ver interrupts.c)
            Tasks[i].frame->eip = entry;
            return;
        }
    }
}

void sched(struct TaskFrame *tf) {
    struct Task *new = 0;
    struct Task *old = current;

    int running_pos = 0;
    for (int i = 0; i < MAX_TASK; i++) {
        if (Tasks[i].status == RUNNING) {
            running_pos = i;
            break;
        }
    }

    int pos = running_pos;
    while (!new) {
        if (Tasks[pos].status == READY) {
            new = &Tasks[pos];
        }
        pos++;

        if (pos == MAX_TASK) {
            pos = 0;
        }
    }

    old->status = READY;
    old->frame = tf;

    new->status = RUNNING;
    current = new;
    asm("movl %0, %%esp\n"
    "popa\n"
    "iret\n"
    :
    : "g"(current->frame)
    : "memory");
}
