#include "boot.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_now(void) {
    struct TASK_LEVEL *tl = &taskctl->level[taskctl->now_lv];
    return tl->tasks[tl->now];
}

void task_add(struct TASK *task) {
    struct TASK_LEVEL *tl = &taskctl->level[task->level];
    tl->tasks[tl->running] = task;
    tl->running++;
    task->flags = 2;
    return;
}

void task_remove(struct TASK *task) {
    int i;
    struct TASK_LEVEL *tl = &taskctl->level[task->level];

    for (i = 0; i < tl->running; i++) {
        if (tl->tasks[i] == task) {
            break;
        }
    }

    tl->running--;
    if (i < tl->now) {
        tl->now--;
    }
    if (tl->now >= tl->running) {
        tl->now = 0;
    }
    task->flags = 1;

    for (; i < tl->running; i++) {
        tl->tasks[i] = tl->tasks[i + 1];
    }

    return;
}

void task_switchsub(void) {
    int i;

    for (i = 0; i < MAX_TASK_LEVELS; i++) {
        if (taskctl->level[i].running > 0) {
            break;
        }
    }
    taskctl->now_lv = i;
    taskctl->lv_change = 0;
    return;
}

void task_idle(void) {
    for (;;) {
        asm_io_hlt();
    }
}

struct TASK *task_init(struct MEMORY_MANAGEMENT *memory_management) {
    int i;
    struct TASK *task, *idle;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;

    taskctl = (struct TASKCTL *)memory_management_alloc_4k(memory_management, sizeof(struct TASKCTL));
    for (i = 0; i < MAX_TASKS; i++) {
        taskctl->tasks0[i].flags = 0;
        taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
        taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
        // clang-format off
        set_segment_descriptor(gdt + TASK_GDT0 + i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);
        set_segment_descriptor(gdt + TASK_GDT0 + MAX_TASKS + i, 15, (int) taskctl->tasks0[i].ldt, AR_LDT);
        // clang-format on
    }
    for (i = 0; i < MAX_TASK_LEVELS; i++) {
        taskctl->level[i].running = 0;
        taskctl->level[i].now = 0;
    }

    task = task_alloc();
    task->flags = 2;
    task->priority = 2;
    task->level = 0;
    task_add(task);
    task_switchsub();
    asm_load_tr(task->sel);
    task_timer = timer_alloc();
    timer_settime(task_timer, task->priority);

    idle = task_alloc();
    idle->tss.esp = memory_management_alloc_4k(memory_management, 64 * 1024) + 64 * 1024;
    idle->tss.eip = (int)&task_idle;
    idle->tss.es = 1 * 8;
    idle->tss.cs = 2 * 8;
    idle->tss.ss = 1 * 8;
    idle->tss.ds = 1 * 8;
    idle->tss.fs = 1 * 8;
    idle->tss.gs = 1 * 8;
    task_run(idle, MAX_TASK_LEVELS - 1, 1);

    return task;
}

struct TASK *task_alloc(void) {
    int i;
    struct TASK *task;
    for (i = 0; i < MAX_TASKS; i++) {
        if (taskctl->tasks0[i].flags == 0) {
            task = &taskctl->tasks0[i];
            task->flags = 1;
            task->tss.eflags = 0x00000202;
            task->tss.eax = 0;
            task->tss.ecx = 0;
            task->tss.edx = 0;
            task->tss.ebx = 0;
            task->tss.ebp = 0;
            task->tss.esi = 0;
            task->tss.edi = 0;
            task->tss.es = 0;
            task->tss.ds = 0;
            task->tss.fs = 0;
            task->tss.gs = 0;
            task->tss.iomap = 0x40000000;
            task->tss.ss0 = 0;
            return task;
        }
    }
    return 0;
}

void task_run(struct TASK *task, int level, int priority) {
    if (level < 0) {
        level = task->level;
    }
    if (priority > 0) {
        task->priority = priority;
    }

    if (task->flags == 2 && task->level != level) {
        task_remove(task);
    }
    if (task->flags != 2) {
        task->level = level;
        task_add(task);
    }

    taskctl->lv_change = 1;
    return;
}

void task_sleep(struct TASK *task) {
    struct TASK *now_task;
    if (task->flags == 2) {
        now_task = task_now();
        task_remove(task);
        if (task == now_task) {
            task_switchsub();
            now_task = task_now();
            asm_far_jmp(0, now_task->sel);
        }
    }
    return;
}

void task_switch(void) {
    struct TASK_LEVEL *tl = &taskctl->level[taskctl->now_lv];
    struct TASK *new_task, *now_task = tl->tasks[tl->now];
    tl->now++;
    if (tl->now == tl->running) {
        tl->now = 0;
    }
    if (taskctl->lv_change != 0) {
        task_switchsub();
        tl = &taskctl->level[taskctl->now_lv];
    }
    new_task = tl->tasks[tl->now];
    timer_settime(task_timer, new_task->priority);
    if (new_task != now_task) {
        asm_far_jmp(0, new_task->sel);
    }
    return;
}
