#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "storeCommand.h"

#define DEBUGGING 0

/*
 * Execute the command input by the user
 * Returns the status of the child process
 */
void executeCommand(struct commandLine command, pid_t childProcesses[100], int* exit_status, int* termination_signal) {
    int childStatus = -5;
    if (DEBUGGING == 1) {
        printf("Parent process' pid = %d\n", getpid());
    }

    // fork a child process. fork() returns the child's pid
    pid_t firstChild = fork();
    if (firstChild == -1) {
        perror("fork() failed!");
        exit(1);
    } else if (firstChild == 0) {
        // The first child process will execute this
        if (command.arguments[0][0] == '#' || command.arguments[0][0] == '\n') {
            exit(0);
        }
        // Set all child processes to ignore SIGTSTP
        struct sigaction ignore_SIGTSTP = {0};
        ignore_SIGTSTP.sa_handler = SIG_IGN;
        sigaction(SIGTSTP, &ignore_SIGTSTP, NULL);
        
        // Check if background process. If not, reset SIGINT to default
        if (command.ampersand == NULL) {
            struct sigaction SIGINT_action = {0};
            SIGINT_action.sa_handler = SIG_DFL;
            sigaction(SIGINT, &SIGINT_action, NULL);
        }
        // Input redirection
        if (command.input_file != NULL) {
            //printf("There's an input file in the command: %s\n", command.input_file);
            //printf("Attempting input redirection...\n");
            // Open source file
            int sourceFD = open(command.input_file, O_RDONLY);
            if (sourceFD == -1) {
                //perror("source open()");
                printf("cannot open %s for input\n", command.input_file);
                exit(1);
            }
            
            if (DEBUGGING == 1) {
                // Written to terminal
                printf("sourceFD == %d\n", sourceFD);
                fflush(stdout);
            }

            // Redirect stdin to source file
            int input_result = dup2(sourceFD, 0);
            if (input_result == -1) {
                perror("source dup2()");
                exit(2);
            }
        } else if (command.ampersand != NULL) {
            if (strcmp(command.ampersand, "&") == 0) {
                // Redirect stdin to /dev/null
                if (DEBUGGING == 1) {
                    printf("Redirecting stdin to /dev/null\n");
                }
                int sourceFD = open("/dev/null", O_WRONLY);
                if (sourceFD == -1) {
                    //perror("source open()");
                    printf("cannot open %s for input\n", command.input_file);
                    exit(1);
                }

                int stdin_result = dup2(sourceFD, 0);
                if (stdin_result == -1) {
                    perror("source dup2()");
                    exit(2);
                }
                if (DEBUGGING == 1) {
                    printf("/dev/null sourceFD == %d\n", sourceFD);
                }
            }
        }

        // Output redirection
        if (command.output_file != NULL) {
            // Open target file
            int targetFD = open(command.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (targetFD == -1) {
                //perror("target open()");
                printf("cannot open %s for output\n", command.input_file);
                exit(1);
            }
            if (DEBUGGING == 1) {
                printf("targetFD == %d\n", targetFD);
            }

            // Redirect stdout to target file
            int output_result = dup2(targetFD, 1);
            if (output_result == -1) {
                perror("target dup2()");
                exit(2);
            }
        } else if (command.ampersand != NULL) {
            if (strcmp(command.ampersand, "&") == 0) {
                // Redirect stdout to /dev/null
                int targetFD = open("/dev/null", O_WRONLY);
                if (targetFD == -1) {
                    //perror("target open()");
                    printf("cannot open %s for output\n", command.input_file);
                    exit(1);
                }

                int stdout_result = dup2(targetFD, 1);
                if (stdout_result == -1) {
                    perror("target dup2()");
                    exit(2);
                }
                if (DEBUGGING == 1) {
                    printf("/dev/null targetFD == %d\n", targetFD);
                }
            }
        }

        if (DEBUGGING == 1) {
            printf("CHILD(%d) running %s command\n", getpid(), command.arguments[0]);
        }
        execvp(command.arguments[0], command.arguments);
        //perror("excevp"); // execv only returns if there is an error
        printf("%s: no such file or directory\n", command.arguments[0]);
        exit(1);
    } else {
        // Parent process executes this to
        if (command.ampersand != NULL) {
            if (strcmp(command.ampersand, "&") == 0) {
                // background process so don't wait for child process
                if (DEBUGGING == 1) {
                    printf("Run in background. Child's pid = %d\n", firstChild);
                }
                printf("background pid is %d\n", firstChild);
                fflush(stdout);
                pid_t childPid = waitpid(firstChild, &childStatus, WNOHANG);
                if (childPid == 0) {
                    childProcesses[0] = firstChild;
                } else if (childProcesses[0] != NULL) {
                    childProcesses[2] = firstChild;
                }
                if (DEBUGGING == 1) {
                    printf("In the parent process waitpid returned value %d\n", childPid);
                }
                if (childPid == 0) {
                    childStatus = childPid;
                }
            }
        } else {
            // wait for the child process
            pid_t childPid = waitpid(firstChild, &childStatus, 0);
            if (WIFSIGNALED(childStatus)) {
                //printf("terminated by signal %d\n", WTERMSIG(childStatus));
                printf("terminated by signal %d\n", WTERMSIG(childStatus));
                fflush(stdout);
                if (DEBUGGING == 1) {
                    printf("Setting termination signal to %d\n", WTERMSIG(childStatus));
                    fflush(stdout);
                }
                *termination_signal = WTERMSIG(childStatus);
            } else {
                if (DEBUGGING == 1) {
                    printf("Setting exit status to %d\n", WEXITSTATUS(childStatus));
                }
                *exit_status = WEXITSTATUS(childStatus);
                *termination_signal = 0;
            }
            if (DEBUGGING == 1) {
                printf("Parent process' pid = %d\n", getpid());
                printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), childPid);
            }
        }

        if (DEBUGGING == 1) {
            printf("Current child process: %d\n", childProcesses[0]);
            fflush(stdout);
        }
        if (childProcesses[0] == NULL) {
            childProcesses[0] = 1;
        }
        // Check for completed background child processes
        if (childProcesses != NULL) {
            int index = 0;
            while(childProcesses[index] != NULL) {
                int status;
                pid_t checkPid = waitpid(childProcesses[index], &status, WNOHANG);
                if (checkPid == childProcesses[index]) {
                    if (WIFSIGNALED(status)) {
                        printf("background pid %d is done: terminated by signal %d\n", checkPid, WTERMSIG(status));
                        fflush(stdout);    
                    } else {
                        printf("background pid %d is done: exit value %d\n", checkPid, WEXITSTATUS(status));
                        fflush(stdout);
                    }
                }
                index += 1;
            }
        }
    }
}