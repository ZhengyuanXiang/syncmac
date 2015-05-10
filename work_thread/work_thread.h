#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_


int work_thr_pool(void *(*work_thr)(void *));
void *work_thr_clinet_dir(void *arg);
void *work_thr_clinet_file(void *arg);
int start_work();

#endif