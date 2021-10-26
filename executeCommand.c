#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "storeCommand.h"

/*
 * Execute the command input by the user
 * Returns the status of the child process
 */
int executeCommand(struct commandLine command) {
    int childStatus = -5;
    printf("Parent process' pid = %d\n", getpid());

    // fork a child process. fork() returns the child's pid
    // fork() returns 0 in the child process, and the child's
    // pid in the parent process
    pid_t firstChild = fork();
    if (firstChild == -1) {
        perror("fork() failed!");
        exit(1);
    } else if (firstChild == 0) {
        // The first child process will execute this
        printf("CHILD(%d) running %s command\n", getpid(), command.arguments[0]);
        execvp(command.arguments[0], command.arguments);
        perror("excevp"); // execv only returns if there is an error
        exit(2);
    } else {
        // Parent process executes this to
        // wait for the child process
        pid_t childPid = waitpid(firstChild, &childStatus, 0);
        printf("Parent process' pid = %d\n", getpid());
        printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), childPid);
        //printf("The parent is done waiting. The pid of child that terminated is %d\n", childPid);
    }
    return childStatus;
}