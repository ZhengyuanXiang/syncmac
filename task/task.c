#include "task.h"
#include "syncmac.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

SYNC_TASK task_head = {0};

int task_cnt = 0;

static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;

SYNC_TASK *get_new_sync_task(char type, char *name, off_t size)
{
    SYNC_TASK *task = malloc(sizeof(SYNC_TASK));
    task->type = type;

    task->name = malloc(strlen(name) + 1);
    strcpy(task->name, name);
    task->name[strlen(name)] = 0;
    task->size = size;
    task->next = NULL;
    return task;
}

SYNC_TASK *fetch_task()
{
    pthread_mutex_lock(&task_mutex);
    SYNC_TASK *task = task_head.next;
    if (task == NULL)
    {
        pthread_mutex_unlock(&task_mutex);
        return NULL;
    }
    
    task_head.next = task->next;
    pthread_mutex_unlock(&task_mutex);
    return task;
}

void free_task(SYNC_TASK *task)
{
    pthread_mutex_lock(&task_mutex);
    task_cnt--;
    pthread_mutex_unlock(&task_mutex);

    free(task->name);
    free(task);
}

void add_task(SYNC_TASK *new_task)
{
    pthread_mutex_lock(&task_mutex);
    SYNC_TASK *task = &task_head;
    while(task->next)
        task = task->next;
    task->next = new_task;
    task_cnt++;
    pthread_mutex_unlock(&task_mutex);
}

void proc_all_task(void (*proc)(SYNC_TASK *))
{
    SYNC_TASK *task = fetch_task();
    while(task)
    {
        proc(task);
        free_task(task);
        task = fetch_task();
    }
}