#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "getStatus.h"

/*
 * Prints the exit status or the terminating signal of
 * the last foreground process that ran
 */
void getStatus(int *wstatus) {
    if (*wstatus == 0) {
        printf("exit status %d\n", *wstatus);
    } else if (WIFEXITED(wstatus)) {
        printf("exit status %d\n", WEXITSTATUS(wstatus));
    /*} else if (WIFSIGNALED(wstatus)) {
        printf("termination signal %d\n", WTERMSIG(wstatus));*/
    } else {
        printf("exit value 1\n");
        //perror("Status could not be found.\n");
    }
}