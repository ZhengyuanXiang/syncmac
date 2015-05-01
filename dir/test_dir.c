#include "dir.h"

int main(int argc, char **args)
{
    DIR_NODE *program_root_node = get_a_new_dir_node(".", ".");

    read_all_dirent(program_root_node);
    print_dir(program_root_node);
    printf("-------------------\n");
    printf("-------------------\n");
    printf("-------------------\n");
    sleep(2);
    remove_a_dir(program_root_node, "apue.2e");
    print_dir(program_root_node);
}