#include "syncmac.h"
#include "work_thread.h"
#include "dir.h"
#include "dir_monitor.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

#define WORK_THREAD_CNT 10
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
            printf("%ld [DEL_DIR] %d %s %d", thr_id, sleep_sec, task->full_name);
            break;
        }
        case ADD_DIR:
        {
            printf("%ld [ADD_DIR] %d %s %d", thr_id, sleep_sec, task->full_name);
            break;
        }
        case ADD_FILE:
        {
            printf("%ld [ADD_FILE] %d %s", thr_id, sleep_sec, task->full_name);
            break;
        }
        case DEL_FILE:
        {
            printf("%ld [DEL_FILE] %d %s", thr_id, sleep_sec, task->full_name);
            break;
        }
    }
    sleep(sleep_sec);
}

void *work_thr_clinet(void *arg)
{

}

int init_work_thr_pool(void *(*work_thr)(void *))
{
    int thr_cnt = 0;
    pthread_t thr_id;
    int flag = 0;
    openlog(DIR_LOG_IDENT, LOG_PID, LOG_LOCAL0);

    for (; thr_cnt < WORK_THREAD_CNT; thr_cnt++)
    {
        if (!pthread_create(&thr_id, NULL, work_thr, NULL))
        {
            flag = 1;
        }
        else
        {
            syslog(LOG_ERR, "create work thread failed\n");
        }
    }
    closelog();

    return flag == 1 ? OK : ERR;
}