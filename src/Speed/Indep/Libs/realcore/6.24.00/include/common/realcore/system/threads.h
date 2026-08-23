#ifndef REALCORE_COMMON_SYSTEM_THREADS_H
#define REALCORE_COMMON_SYSTEM_THREADS_H

struct THREAD {
    int reserved[198];
};

void THREAD_init();
bool THREAD_create(THREAD *hThread, int (*entryproc)(void *), void *param, void *stack_addr, int stack_size, int prio);
void THREAD_destroy(THREAD *thread);
void THREAD_yield(int dur);
bool THREAD_iscurrent(THREAD *thread);
bool THREAD_setpriority(THREAD *thread, int priority);
bool THREAD_testexit(THREAD *thread);
bool THREAD_waitexit(THREAD *thread, int);

#endif
