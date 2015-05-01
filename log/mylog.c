#include <syslog.h>

void print_log(char *ident, const char *msg, int level)
{
    openlog(ident, LOG_PID, LOG_LOCAL0);
    syslog(LOG_INFO, msg);
    closelog();
}

int main(int argc, char **args)
{
    print_log("xiang_test_log", "test_log", LOG_ERR);
}