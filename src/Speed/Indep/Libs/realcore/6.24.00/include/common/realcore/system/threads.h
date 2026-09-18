#ifndef REALCORE_COMMON_SYSTEM_THREADS_H
#define REALCORE_COMMON_SYSTEM_THREADS_H

struct THREAD;

void THREAD_yield(int a);
bool THREAD_iscurrent(THREAD *thread);

#endif
