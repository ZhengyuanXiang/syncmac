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


#define DIR_LOG_IDENT "WORK_THREAD"

extern int sem_id;

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

#define CONSUME_TASK 1
#define CONSUME_WORK 2
#define PRODUCT_WORK 3

#define OP_BLOCK 1
#define OP_UNBLOCK 2

void set_sembuf(struct sembuf* sembuf_p, unsigned short sem_num, 
                    short sem_op, short sem_flg)
{
    sembuf_p->sem_num = sem_num;
    sembuf_p->sem_op = sem_op;
    sembuf_p->sem_flg = sem_flg;
}

int op_task_sem(int op_type, int block, int num)
{
    struct sembuf sem_buf;
    short sem_op = (block == OP_BLOCK ? SEM_UNDO : (SEM_UNDO | IPC_NOWAIT));
    
    switch (op_type)
    {
        case CONSUME_TASK:
        {
            set_sembuf(&sem_buf, SEM_TASK_ID, num, sem_op);
            break;
        }
        case CONSUME_WORK:
        {
            set_sembuf(&sem_buf, SEM_WORK_ID, num, sem_op);
            break;
        }
        case PRODUCT_WORK:
        {
            set_sembuf(&sem_buf, SEM_WORK_ID, num, sem_op);
            break;
        }
        default:
        {
            return ERR;
        }
    }
    return ret = semop(sem_id, &sem_buf, 1);
}

void *work_thr_clinet(void *arg)
{
    int ret;
    SYNC_TASK *task;

    while (1)
    {
        ret = op_task_sem(CONSUME_TASK, OP_UNBLOCK, -1);
        {
            if (ret == EAGAIN)
            {
                break;
            }
            task = fetch_task();
            do_task(task);
        }
    }
    pthread_exit(0);
}



int init_work_thr_pool(void *(*work_thr)(void *))
{
    int thr_idx = 0;
    pthread_t thr_id[WORK_THREAD_NUM];
    int flag = 0;

    op_task_sem(CONSUME_WORK, OP_BLOCK, -(WORK_THREAD_NUM));

    for (; thr_idx < WORK_THREAD_NUM; thr_idx++)
    {
        if (!pthread_create(&thr_id[thr_idx], NULL, work_thr, NULL))
        {
            flag = 1;
        }
    }


    return flag == 1 ? OK : ERR;
}