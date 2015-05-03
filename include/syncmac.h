#ifndef _SYNCMAC_H_
#define _SYNCMAC_H_

#define MY_DEBUG 1
#if MY_DEBUG
    #define PRINT(fmt, args...) printf(fmt, ##args)
#else
    #define PRINT(fmt, args...) 
#endif

#define ERR 1
#define OK 0

#define DEL_DIR 1
#define ADD_DIR 2
#define DEL_FILE 3
#define ADD_FILE 4

#define CHECK_INTERVAL 15

#define MY_SEM_KEY 1234
#define MY_SEM_MAX 2048
#define MY_SEM_PERM 0666

#define SEM_TASK_ID 0
#define SEM_WORK_ID 1

#endif