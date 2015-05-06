#include "work_thread.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **args)
{
    init_work_thr_pool(work_thr_clinet);
    pause();
}