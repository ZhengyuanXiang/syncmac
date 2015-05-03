#include "dir.h"
#include "dir_monitor.h"
#include "syncmac.h"

#include <stdlib.h>
#include <stdio.h>

extern int task_cnt;

int main()
{
    char *full_name = "/private/tmp/Books";
    char *name = "Books";
    DIR_NODE *old_dir = get_a_new_dir_node(full_name, name);
    DIR_NODE *new_dir;
    int change_flag = UNCHANGED;
    read_all_dirent(old_dir);

    while (1)
    {
        sleep(CHECK_INTERVAL);
        if (task_cnt != 0)
        {
            PRINT("ERR %s %d\n", __FILE__, __LINE__);
            return ERR;
        }

        new_dir = get_a_new_dir_node(full_name, name);
        read_all_dirent(new_dir);
        change_flag = dir_changes(old_dir, new_dir);
        free_dir(old_dir);
        old_dir = new_dir;
        TEST
        if (CHANGED == change_flag)
        {
            TEST
            start_work();
        }
    }
    return OK;
}