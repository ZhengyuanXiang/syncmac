#include "dir.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args)
{
    struct timespec time = {0};
    DIR_NODE *program_root_node = get_a_new_dir_node(".", 0, &time);

    read_all_dirent(program_root_node);
    print_dir(program_root_node);
    printf("-------------------\n");
    printf("-------------------\n");
    printf("-------------------\n");
    //sleep(2);
    remove_a_dir(program_root_node, "apue.2e");
    print_dir(program_root_node);
}