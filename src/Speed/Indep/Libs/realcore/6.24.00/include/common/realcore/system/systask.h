#ifndef REALCORE_COMMON_SYSTEM_SYSTASK_H
#define REALCORE_COMMON_SYSTEM_SYSTASK_H

void SYNCTASK_add(void (*taskfunc)(void *, int), int rate, int delay, void *param);
void SYNCTASK_del(void (*taskfunc)(void *, int));
void SYNCTASK_run();

#endif
