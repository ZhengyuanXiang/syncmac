#include "task.h"
#include "syncmac.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>


static SYNC_TASK dir_task_head = {0};
static SYNC_TASK file_task_head = {0};

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

SYNC_TASK *fetch_dir_task()
{
    pthread_mutex_lock(&task_mutex);
    SYNC_TASK *task = dir_task_head.next;
    if (task == NULL)
    {
        pthread_mutex_unlock(&task_mutex);
        return NULL;
    }
    
    dir_task_head.next = task->next;
    pthread_mutex_unlock(&task_mutex);
    return task;
}

SYNC_TASK *fetch_file_task()
{
    pthread_mutex_lock(&task_mutex);
    SYNC_TASK *task = file_task_head.next;
    if (task == NULL)
    {
        pthread_mutex_unlock(&task_mutex);
        return NULL;
    }
    
    file_task_head.next = task->next;
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

static void add_dir_task(SYNC_TASK *new_task)
{
    SYNC_TASK *task = &dir_task_head;
    while(task->next)
        task = task->next;
    task->next = new_task;
}

static void add_file_task(SYNC_TASK *task)
{
    SYNC_TASK *curr = file_task_head.next;
    SYNC_TASK *pre_curr = &file_task_head;

    if (curr == NULL)
        file_task_head.next = task;

    while (curr)
    {
        if (curr == NULL)
            break;
        if (task->type == ADD_FILE || task->type == MOD_FILE)
        {
            if (curr->type == DEL_FILE)
                break;
            if (curr->size < task->size)
                break;
        }
        else if (task->type == DEL_FILE)
        {
            if (curr->type == DEL_FILE)
            {
                if (curr->size < task->size)
                    break;
            }
        }
        else
        {
            DEBUG_ERR
        }
        pre_curr = curr;
        curr = curr->next;
    }
    pre_curr->next = task;
}

void add_task(SYNC_TASK *new_task)
{
    pthread_mutex_lock(&task_mutex);
    switch(new_task->type)
    {
        case DEL_DIR:
        case ADD_DIR:
        {
            add_dir_task(new_task);
            break;
        }
        case DEL_FILE:
        case ADD_FILE:
        case MOD_FILE:
        {
            add_file_task(new_task);
            break;
        }
        default:
        {
            PRINT("unknow task");
        }
    }
    task_cnt++;
    pthread_mutex_unlock(&task_mutex);
}

void proc_all_task(void (*proc)(SYNC_TASK *))
{
    /*SYNC_TASK *task = fetch_task();
    while(task)
    {
        proc(task);
        free_task(task);
        task = fetch_task();
    }*/
}