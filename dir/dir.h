#ifndef _DIR_H_
#define _DIR_H_

#define DIR_MAX 20
#define FILE_MAX 30
#define ERR 1
#define OK 0
#define DIR_LEN_MAX 300

#define DIRNODE SIF_DIR
#define FILENODE SIF_REG

typedef struct tag_file_node
{
    char *relate_name;
    char *name;
    struct tag_file_node *next_file;
}FILE_NODE;

typedef struct tag_dir_node
{
    char *relate_name;
    char *name;
    struct tag_dir_node *next_dir;
    struct tag_file_node *next_file;
}DIR_NODE;

DIR_NODE * get_a_new_dir_node(char *relate_name, char *name);
int read_all_dirent(DIR_NODE *dir_node);
FILE_NODE * get_a_new_file_node(char *relate_name, char *name);
void inset_a_dir(DIR_NODE *curr_dir, DIR_NODE *new_dir);
void inset_a_file(DIR_NODE *curr_dir, FILE_NODE *new_file);
void print_dir(DIR_NODE *dir_node);

int remove_a_file(DIR_NODE *curr_dir, char *file_name);
void free_file_node(FILE_NODE *file_node);
int remove_a_dir(DIR_NODE *curr_dir, char *dir_name);
void free_dir_node(DIR_NODE *dir_node);

#endif