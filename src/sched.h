void sched_init();

enum TaskStatus {
    FREE = 0,
    READY,
    RUNNING,
    DYING,
};

struct TaskFrame {
    // Ver ejercicio kern2-spawn más abajo.
};

struct Task {
    uint8_t stack[4096];
    enum TaskStatus status;
    struct TaskFrame *frame;
};
