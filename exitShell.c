#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include "exitShell.h"

/* 
 * Kills all processes and exits the shell
 */
void exitShell() {
    kill(0, SIGKILL);
    exit(0); 
}