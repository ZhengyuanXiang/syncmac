#ifndef _DIR_H_
#define _DIR_H_

#define DIR_MAX 20
#define FILE_MAX 30

#define DIR_LEN_MAX 500

#define DIRNODE SIF_DIR
#define FILENODE SIF_REG

typedef struct tag_file_node
{
    char *name;
    struct tag_file_node *next_file;
    struct tag_dir_node *parent_dir;
}FILE_NODE;

typedef struct tag_dir_node
{
    char *name;
    struct tag_dir_node *next_bro_dir;
    struct tag_dir_node *next_chl_dir;
    struct tag_dir_node *parent_dir;
    struct tag_file_node *next_file;
}DIR_NODE;

DIR_NODE * get_a_new_dir_node(char *name);
int read_all_dirent(DIR_NODE *dir);
FILE_NODE * get_a_new_file_node(char *name);
void insert_a_chl_dir(DIR_NODE *curr_dir, DIR_NODE *new_dir);
void insert_a_file(DIR_NODE *curr_dir, FILE_NODE *new_file);
void print_dir(DIR_NODE *dir);

int remove_a_file(DIR_NODE *curr_dir, char *file_name);
void free_file(FILE_NODE *file);
int remove_a_dir(DIR_NODE *curr_dir, char *dir_name);
void free_dir(DIR_NODE *dir);
void get_dir_full_name(DIR_NODE* dir, char *name);
void get_file_full_name(FILE_NODE* file, char *name);
int filter(char *name);

int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file);
int is_same_dir(DIR_NODE *old_dir, DIR_NODE *new_dir);

#endif