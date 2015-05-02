#include "dir_monitor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DEL_DIR 1
#define ADD_DIR 2

#define DEL_FILE 3
#define ADD_FILE 4

static int file_add = 0;
static int file_del = 0;
static int dir_add = 0;
static int dir_del = 0;

void add_dir_change_event(int event, DIR_NODE *dir)
{
    switch(event)
    {
        case ADD_DIR:
        {
            dir_add++;
            printf("add dir %s\n", dir->full_name);
            break;
        }
        case DEL_DIR:
        {
            dir_del++;
            printf("delete dir %s\n", dir->full_name);
            break;
        }
        default:
        {
            printf("unknow dir %s\n", dir->full_name);
        }
    }
}

void add_file_change_event(int event, FILE_NODE *file)
{

    switch(event)
    {
        case ADD_FILE:
        {
            file_add++;
            printf("add file %s\n", file->full_name);
            break;
        }
        case DEL_FILE:
        {
            file_del++;
            printf("delete file %s\n", file->full_name);
            break;
        }
        default:
        {
            printf("unknow change %s\n", file->full_name);
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
            add_dir_change_event(DEL_DIR, old_chl_dir);
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
            add_dir_change_event(ADD_DIR, new_chl_dir);
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
           add_file_change_event(DEL_FILE, old_file);
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
            add_file_change_event(ADD_FILE, new_file);
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
    DIR_NODE *old_dir = get_a_new_dir_node(".", ".");
    read_all_dirent(old_dir);
    DIR_NODE *new_dir;
    print_dir(old_dir);
    while (1)
    {
        sleep(3);
        new_dir = get_a_new_dir_node(".", ".");
        read_all_dirent(new_dir);
        dir_changes(old_dir, new_dir);
        printf("file add %d del %d\n", file_add, file_del);
        printf("dir add %d del %d\n", dir_add, dir_del);
        file_add = 0;
        file_del = 0;
        dir_add = 0;
        dir_del = 0;
        free_dir(old_dir);
        old_dir = new_dir;
    }
}