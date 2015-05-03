#include "syncmac.h"
#include "work_thread.h"
#include "dir.h"
#include "dir_monitor.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

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
            PRINT("%ld [DEL_DIR] %d %s %d", thr_id, sleep_sec, task->full_name);
            break;
        }
        case ADD_DIR:
        {
            PRINT("%ld [ADD_DIR] %d %s %d", thr_id, sleep_sec, task->full_name);
            break;
        }
        case ADD_FILE:
        {
            PRINT("%ld [ADD_FILE] %d %s", thr_id, sleep_sec, task->full_name);
            break;
        }
        case DEL_FILE:
        {
            PRINT("%ld [DEL_FILE] %d %s", thr_id, sleep_sec, task->full_name);
            break;
        }
    }
    sleep(sleep_sec);
}

void set_sembuf(struct sembuf* sembuf_p, unsigned short sem_num, 
                    short sem_op, short sem_flg)
{
    sembuf_p->sem_num = sem_num;
    sembuf_p->sem_op = sem_op;
    sembuf_p->sem_flg = sem_flg;
}

void clinet_wait()
{
    int sem_id;
    int ret;
    struct sembuf sem_buf;

    sem_id = semget(MY_SEM_KEY, 0, IPC_CREAT);

    set_sembuf(&sem_buf, SEM_WORK_ID, -1, SEM_UNDO);

    ret = semop(sem_id, &sem_buf, 1);
    if (ret != 0)
    {
        PRINT("ERROR %d\n", __LINE__);
    }
}

void *work_thr_clinet(void *arg)
{
    while (1)
    {
        clinet_wait();
        PRINT("work---\n");
    }
}



int init_work_thr_pool(void *(*work_thr)(void *))
{
    int thr_cnt = 0;
    pthread_t thr_id;
    int sem_id;
    int flag = 0;
    openlog(DIR_LOG_IDENT, LOG_PID, LOG_LOCAL0);

    union semun sem_args;
    struct sembuf sem_buf[2];

    sem_id = semget(MY_SEM_KEY, 2, IPC_CREAT | MY_SEM_PERM);
    sem_args.val = 0;
    semctl(sem_id, SEM_WORK_ID, SETVAL, sem_args);

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
    int i = 2;
    int ret;
    while (i <= 10)
    {
        sleep(1);
        set_sembuf(&sem_buf[1], SEM_WORK_ID, 2, SEM_UNDO);
        ret = semop(sem_id, &sem_buf[1], 1);
        if (ret != 0)
        {
            PRINT("ERROR %d\n", __LINE__);
        }
        i = i + 2;
    }
    PRINT("task over\n");
    closelog();

    return flag == 1 ? OK : ERR;
}