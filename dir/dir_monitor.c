#include "dir_monitor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include "syncmac.h"
#include "task.h"

#define DIR_LOG_IDENT "dir_monitor"

extern int task_cnt;

int dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    int change_flag = UNCHANGED;
    if (CHANGED == chl_dir_change(old_dir, new_dir))
    {
        change_flag = CHANGED;
    }
    if (CHANGED == file_change(old_dir, new_dir))
    {
        change_flag = CHANGED;
    }
    return change_flag;
}

void monitor()
{
    char *name = "/private/tmp/Books";
    DIR_NODE *old_dir = get_a_new_dir_node(name, 0, 0);
    DIR_NODE *new_dir;
    read_all_dirent(old_dir);

    while (1)
    {
        sleep(CHECK_INTERVAL);
        if (task_cnt != 0)
        {
            PRINT("ERR %s %d\n", __FILE__, __LINE__);
            return;
        }

        new_dir = get_a_new_dir_node(name, 0, 0);
        read_all_dirent(new_dir);
        dir_changes(old_dir, new_dir);
        free_dir(old_dir);
        old_dir = new_dir;

    }
}



static void test_print_task(SYNC_TASK *task)
{
    switch(task->type)
    {
        case DEL_DIR:
        {
            PRINT("[TASK] delete dir %s\n", task->name);
            break;
        }
        case ADD_DIR:
        {
            PRINT("[TASK] ADD dir %s\n", task->name);
            break;
        }
        case DEL_FILE:
        {
            PRINT("[TASK] delete file %s\n", task->name);
            break;
        }
        case ADD_FILE:
        {
            PRINT("[TASK] ADD file %s\n", task->name);
            break;
        }
        case MOD_FILE:
        {
            PRINT("[TASK] MOD file %s\n", task->name);
            break;
        }
        default:
        {
            PRINT("[TASK] unknow\n");
        }
    }
}

void add_dir_to_task(DIR_NODE *dir)
{
    FILE_NODE *file = dir->next_file;
    DIR_NODE *dir_chl = dir->next_chl_dir;
    while (file)
    {
        new_event(ADD_FILE, (void *)file);
        file = file->next_file;
    }
    while (dir_chl)
    {
        new_event (ADD_DIR, (void *)dir_chl);
        dir_chl = dir_chl->next_bro_dir;
    }
}

void new_event(char event, void* data)
{
    SYNC_TASK *task;
    DIR_NODE *dir;
    FILE_NODE *file;
    char full_name[DIR_LEN_MAX] = {0};

    switch(event)
    {
        case ADD_DIR:
        case DEL_DIR:
        {
            dir = (DIR_NODE *)data;
            get_dir_full_name(dir, full_name);
            task = get_new_sync_task(event, full_name, dir->size);
            add_task(task);
            if (ADD_DIR == event)
            {
                add_dir_to_task(dir);
            }
            break;
        }
        case ADD_FILE:
        case DEL_FILE:
        case MOD_FILE:
        {
            file = (FILE_NODE *)data;
            get_file_full_name(file, full_name);
            task = get_new_sync_task(event, full_name, file->size);
            add_task(task);
            break;
        }
        default:
        {
            PRINT("unknow change\n");
        }
    }
}

int chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    DIR_NODE *old_chl_dir = old_dir->next_chl_dir;
    DIR_NODE *new_chl_dir;

    int change_flag = UNCHANGED;

    while (old_chl_dir != NULL)
    {
        new_chl_dir = new_dir->next_chl_dir;
        while (new_chl_dir != NULL)
        {
            if (OK == is_same_dir(old_chl_dir, new_chl_dir))
            {
                if (CHANGED == dir_changes(old_chl_dir, new_chl_dir))
                {
                    change_flag = CHANGED;
                }
                break;
            }
            new_chl_dir = new_chl_dir->next_bro_dir;
        }
        if (new_chl_dir == NULL)
        {
            change_flag = CHANGED;
            new_event(DEL_DIR, (void*)old_chl_dir);
        }
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
        {
            change_flag = CHANGED;
            new_event(ADD_DIR, (void *)new_chl_dir);
        }
        new_chl_dir = new_chl_dir->next_bro_dir;
    }
    return change_flag;
}



int file_change(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    FILE_NODE *old_file = old_dir->next_file;
    FILE_NODE *new_file;

    int change_flag = UNCHANGED;
    /*get deleted or modified files*/
    while (old_file != NULL)
    {
        new_file = new_dir->next_file;
        while (new_file != NULL)
        {
            if (OK == is_same_file(old_file, new_file))
            {
                /*get modifid files*/
                if (OK == is_file_changed(old_file, new_file))
                {
                    new_event(MOD_FILE, (void *)new_file);
                    change_flag = CHANGED;
                }
                break;
            }
            new_file = new_file->next_file;
        }
        if (NULL == new_file)
        {
            change_flag = CHANGED;
            new_event(DEL_FILE, (void *)old_file);
        }
       old_file = old_file->next_file;
    }

    new_file = new_dir->next_file;
    /*get new files*/
    while (new_file)
    {
        old_file = old_dir->next_file;
        while (old_file != NULL)
        {
            /*don't need get modifid files*/
            if (OK == is_same_file(old_file, new_file))
                break;
            old_file = old_file->next_file;
        }
        if (NULL == old_file)
        {
            change_flag = CHANGED;
            TEST
            new_event(ADD_FILE, (void *)new_file);
        }
        new_file = new_file->next_file;
    }
    return change_flag;
}