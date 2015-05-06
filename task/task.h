#ifndef _TASK_H_
#define _TASK_H_

#include <sys/types.h>

typedef struct tag_sync_task
{
    char type;
    char rev[2];
    off_t size;
    char *name;
    struct tag_sync_task *next;
}SYNC_TASK;

SYNC_TASK *get_new_sync_task(char type, char *name, off_t size);
void free_task(SYNC_TASK *task);
void add_task(SYNC_TASK *new_task);
SYNC_TASK *fetch_file_task();
SYNC_TASK *fetch_dir_task();
void proc_all_task(void (*proc)(SYNC_TASK *));
#endif