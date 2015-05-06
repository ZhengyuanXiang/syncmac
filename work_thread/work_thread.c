#include "syncmac.h"
#include "work_thread.h"
#include "dir.h"
#include "dir_monitor.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>


#define DIR_LOG_IDENT "WORK_THREAD"


static int get_rand(unsigned int seed)
{
    unsigned int state;
    srand(seed);
    return rand() % 10 + 1;
}

static void do_task(SYNC_TASK *task)
{
    pthread_t thr_id = pthread_self();
    int sleep_sec = get_rand(thr_id);

    switch (task->type)
    {
        case DEL_DIR:
        {
            PRINT("[DEL_DIR] %d %s %lld\n", sleep_sec, task->name, task->size);
            break;
        }
        case ADD_DIR:
        {
            PRINT("[ADD_DIR] %d %s %lld\n", sleep_sec, task->name, task->size);
            break;
        }
        case ADD_FILE:
        {
            PRINT("[ADD_FILE] %d %s %lld\n", sleep_sec, task->name, task->size);
            break;
        }
        case DEL_FILE:
        {
            PRINT("[DEL_FILE] %d %s %lld\n", sleep_sec, task->name, task->size);
            break;
        }
        case MOD_FILE:
        {
            PRINT("[MOD_FILE] %d %s %lld\n", sleep_sec, task->name, task->size);
            break;
        }
    }
    sleep(sleep_sec);
}

void *work_thr_clinet(void *arg)
{
    int ret;
    SYNC_TASK *task;

    while (1)
    {
        task = fetch_task();
        if (NULL == task)
            break;
        do_task(task);
        free_task(task);
    }
    pthread_exit(0);
}

int init_work_thr_pool(void *(*work_thr)(void *))
{
    int thr_idx;
    pthread_t thr_id[WORK_THREAD_NUM];
    int flag = 0;

    for (thr_idx = 0; thr_idx < WORK_THREAD_NUM; thr_idx++)
    {
        if (!pthread_create(&thr_id[thr_idx], NULL, work_thr, NULL))
        {
            flag = 1;
        }
    }
    for (thr_idx = 0; thr_idx < WORK_THREAD_NUM; thr_idx++)
    {
        pthread_join(thr_id[thr_idx], NULL);
    }
    return flag == 1 ? OK : ERR;
}

int start_work()
{
    return init_work_thr_pool(work_thr_clinet);
}