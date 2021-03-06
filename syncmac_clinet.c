#include "dir.h"
#include "dir_monitor.h"
#include "syncmac.h"
#include "work_thread.h"

#include <stdlib.h>
#include <stdio.h>

extern int task_cnt;

int main()
{
    char *name = "/private/tmp/Books";
    struct timespec time = {0};
    DIR_NODE *old_dir = get_a_new_dir_node(name, 0, &time);
    DIR_NODE *new_dir;
    int change_flag = UNCHANGED;
    read_all_dirent(old_dir);

    while (1)
    {
        sleep(CHECK_INTERVAL);
        if (task_cnt != 0)
        {
            PRINT("ERR %s %d %d\n", __FILE__, __LINE__, task_cnt);
            return ERR;
        }

        new_dir = get_a_new_dir_node(name, 0, &time);
        read_all_dirent(new_dir);
        change_flag = dir_changes(old_dir, new_dir);
        free_dir(old_dir);
        old_dir = new_dir;

        if (CHANGED == change_flag)
        {
            printf("---------------\n");
            start_work();
            printf("---------------\n");
        }
    }
    return OK;
}