#ifndef _DIR_MONITOR_H_
#define _DIR_MONITOR_H_

#include "dir.h"

typedef struct tag_sync_task
{
    char type;
    char rev[3];
    char *full_name;
    char *name;
    struct tag_sync_task *next;
}SYNC_TASK;

void new_event(char event, void* data);
int chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir);
int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file);
int is_same_dir(DIR_NODE *old_dir, DIR_NODE *new_dir);
int file_change(DIR_NODE *old_dir, DIR_NODE *new_dir);
int dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir);
void monitor();
SYNC_TASK *get_new_sync_task(char type, char *full_name, char *name);
void free_task(SYNC_TASK *task);
void add_task(SYNC_TASK *new_task);
SYNC_TASK *fetch_task();
void proc_all_task(void (*proc)(SYNC_TASK *));

#endif