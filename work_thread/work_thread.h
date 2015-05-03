#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

int init_work_thr_pool(void *(*work_thr)(void *));
void *work_thr_clinet(void *arg);
int start_work();

#endif