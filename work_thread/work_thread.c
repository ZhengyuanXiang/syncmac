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

static void *work_thr(void *arg)
{
    pthread_t thr_id = pthread_self();
    printf("work thread %d created\n", thr_id);
    SYNC_TASK *task = fetch_task();
}

int init_work_thr_pool()
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