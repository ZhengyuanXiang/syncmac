#include "dir.h"

int main(int argc, char **args)
{
    DIR_NODE *program_root_node = get_a_new_dir_node(".", ".");
    read_all_dirent(program_root_node);
}