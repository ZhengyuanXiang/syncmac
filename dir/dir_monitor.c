#include "dir_monitor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include "syncmac.h"

#define DEL_DIR 1
#define ADD_DIR 2
#define DEL_FILE 3
#define ADD_FILE 4

#define DIR_LOG_IDENT "dir_monitor"

static void test_print_task(SYNC_TASK *task)
{
    switch(task->type)
    {
        case DEL_DIR:
        {
            printf("[TASK] delete dir %s\n", task->full_name);
            break;
        }
        case ADD_DIR:
        {
            printf("[TASK] ADD dir %s\n", task->full_name);
            break;
        }
        case DEL_FILE:
        {
            printf("[TASK] delete file %s\n", task->full_name);
            break;
        }
        case ADD_FILE:
        {
            printf("[TASK] ADD file %s\n", task->full_name);
            break;
        }
        default:
        {
            printf("[TASK] unknow\n");
        }
    }
}

SYNC_TASK task_head = {0};

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

void free_task(SYNC_TASK *task)
{
    free(task->full_name);
    free(task->name);
    free(task);
}

void add_task(SYNC_TASK *new_task)
{
    new_task->next = task_head.next;
    task_head.next = new_task;
}

SYNC_TASK *fetch_task()
{
    SYNC_TASK *task = task_head.next;
    if (task == NULL)
        return NULL;
    task_head.next = task->next;
    return task;
}

SYNC_TASK *get_new_sync_task(char type, char *full_name, char *name)
{
    SYNC_TASK *task = malloc(sizeof(SYNC_TASK));
    task->type = type;

    task->full_name = malloc(strlen(full_name) + 1);
    strcpy(task->full_name, full_name);
    task->full_name[strlen(full_name)] = 0;

    task->name = malloc(strlen(name) + 1);
    strcpy(task->name, name);
    task->name[strlen(name)] = 0;

    task->next = NULL;
    return task;
}

static int file_add = 0;
static int file_del = 0;
static int dir_add = 0;
static int dir_del = 0;

void new_event(char event, void* data)
{
    SYNC_TASK *task;
    DIR_NODE *dir;
    FILE_NODE *file;

    switch(event)
    {
        case ADD_DIR:
        {
            dir = (DIR_NODE *)data;
            task = get_new_sync_task(event, dir->full_name, dir->name);
            add_task(task);
            dir_add++;
            //printf("add dir %s\n", dir->full_name);
            break;
        }
        case DEL_DIR:
        {
            dir = (DIR_NODE *)data;
            task = get_new_sync_task(event, dir->full_name, dir->name);
            add_task(task);
            //printf("delete dir %s\n", dir->full_name);
            break;         
        }
        case ADD_FILE:
        {
            file = (FILE_NODE *)data;
            task = get_new_sync_task(event, file->full_name, file->name);
            add_task(task);
            file_add++;
            //printf("add file %s\n", file->full_name);
            break;
        }
        case DEL_FILE:
        {
            file = (FILE_NODE *)data;
            task = get_new_sync_task(event, file->full_name, file->name);
            add_task(task);
            file_del++;
            //printf("delete file %s\n", file->full_name);
            break;
        }
        default:
        {
            printf("unknow change\n");
        }
    }
}


int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file)
{
    if ( 0 == strcmp(old_file->name, new_file->name)
         && 0 == strcmp(old_file->full_name, new_file->full_name))
        return OK;
    return ERR;
}

int is_same_dir(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    if ( 0 == strcmp(old_dir->name, new_dir->name)
         && 0 == strcmp(old_dir->full_name, new_dir->full_name))
        return OK;
    return ERR;
}

void chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    DIR_NODE *old_chl_dir = old_dir->next_chl_dir;
    DIR_NODE *new_chl_dir;

    while (old_chl_dir != NULL)
    {
        new_chl_dir = new_dir->next_chl_dir;
        while (new_chl_dir != NULL)
        {
            if (OK == is_same_dir(old_chl_dir, new_chl_dir))
            {
                dir_changes(old_chl_dir, new_chl_dir);
                break;
            }
            new_chl_dir = new_chl_dir->next_bro_dir;
        }
        if (new_chl_dir == NULL)
            new_event(DEL_DIR, (void*)old_chl_dir);
        old_chl_dir = old_chl_dir->next_bro_dir;
    }

    new_chl_dir = new_dir->next_chl_dir;
    while (new_chl_dir != NULL)
    {
        old_chl_dir = old_dir->next_chl_dir;
        while (old_chl_dir != NULL)
        {
            if (OK == is_same_dir(old_chl_dir, new_chl_dir))
            {
                break;
            }
            old_chl_dir = old_chl_dir->next_bro_dir;
        }
        if (old_chl_dir == NULL)
            new_event(ADD_DIR, (void *)new_chl_dir);
        new_chl_dir = new_chl_dir->next_bro_dir;
    }
    return;
}



void file_change(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    FILE_NODE *old_file = old_dir->next_file;
    FILE_NODE *new_file;

    while (old_file != NULL)
    {
        new_file = new_dir->next_file;
        while (new_file != NULL)
        {
            if (OK == is_same_file(old_file, new_file))
                break;
            new_file = new_file->next_file;
        }
        if (NULL == new_file)
           new_event(DEL_FILE, (void *)old_file);
       old_file = old_file->next_file;
    }

    new_file = new_dir->next_file;
    while (new_file)
    {
        old_file = old_dir->next_file;
        while (old_file != NULL)
        {
            if (OK == is_same_file(old_file, new_file))
                break;
            old_file = old_file->next_file;
        }
        if (NULL == old_file)
            new_event(ADD_FILE, (void *)new_file);
        new_file = new_file->next_file;
    }
    return;
}

void dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    chl_dir_change(old_dir, new_dir);
    file_change(old_dir, new_dir);
}

void monitor()
{
    openlog(DIR_LOG_IDENT, LOG_PID, LOG_LOCAL0);
    DIR_NODE *old_dir = get_a_new_dir_node(".", ".");
    read_all_dirent(old_dir);
    DIR_NODE *new_dir;
    while (1)
    {
        printf("--------------------\n");
        sleep(CHECK_INTERVAL);
        new_dir = get_a_new_dir_node(".", ".");
        read_all_dirent(new_dir);
        dir_changes(old_dir, new_dir);
        syslog(LOG_INFO, "file add %d del %d\n", file_add, file_del);
        //printf("file add %d del %d\n", file_add, file_del);
        //printf("dir add %d del %d\n", dir_add, dir_del);
        syslog(LOG_INFO, "dir add %d del %d\n", dir_add, dir_del);
        file_add = 0;
        file_del = 0;
        dir_add = 0;
        dir_del = 0;
        free_dir(old_dir);
        old_dir = new_dir;
        proc_all_task(test_print_task);
        printf("--------------------\n");
    }
    closelog();
}