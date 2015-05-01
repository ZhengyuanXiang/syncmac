#include "dir.h"

#define DEL_DIR 1
#define ADD_DIR 2

#define DEL_FILE 3
#define ADD_FILE 4

void add_dir_change_event(int event)
{

}

void add_file_change_event(int event)
{

}

void chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    if (old_dir == NULL)
    {
        add_dir_change_event(ADD_DIR, new_dir);
        return;
    }

    if (new_dir == NULL)
    {
        add_dir_change_event(DEL_DIR, old_dir);
        return;
    }
}

int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file)
{
    if ( 0 == strcmp(old_file->name, new_file->name)
         && 0 == strcmp(old_file->full_name, new_file->full_name)
        return OK;
    return ERR;
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
            if (OK == is_same_file())
                break;
        }
        if (NULL == new_file)
           add_file_change_event() 
    }
}

void dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    if ((old_dir == NULL) && (new_dir == NULL))
    {
        return;
    }
    chl_dir_change(old_dir, new_dir);
    if (old_dir != NULL & new_dir != NULL)
    {
        file_change(old_dir, new_dir);
    }

}

int main(int argc, char **args)
{
    DIR_NODE *program_root_node = get_a_new_dir_node(".", ".");
    read_all_dirent(program_root_node);
}