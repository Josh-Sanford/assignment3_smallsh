#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "storeCommand.h"
#include "expandVariables.h"
#include "exitShell.h"
#include "changeDirectory.h"
#include "getStatus.h"
#include "executeCommand.h"

/*
pid_t getpid(void);

pid_t fork(void);

void exit(int status);

pid_t wait(int *wstatus);

pid_t waitpid(pid_t pid, int *wstatus, int options);
wstatus: WIFEXITED(wstatus), WEXITSTATUS(wstatus), WIFSIGNALED(wstatus), WTERMSIG(wstatus)
options: WNOHANG

int execl(const char *pathname, const char *arg, ...  (char  *) NULL );
int execlp(const char *filename, const char *arg, ...  (char  *) NULL );
int execle(const char *pathname, const char *arg, ...  , (char *) NULL, char * const envp[]);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *filename, char *const argv[]);
int execve(const char *pathname, char *const argv[], char *const envp[]);

command [arg1 arg2 ...] [< input_file] [> output_file] [&]
*/


int main(int argc, char *argv[]) {
    /* getline() functionality adapted from https://c-for-dummies.com/blog/?p=1112
     */
    while(1) {
        char *buffer;
        size_t buffsize = 2048;
        size_t input;
        int wstatus = 0;

        buffer = (char *)malloc(buffsize * sizeof(char));
        if (buffer == NULL) {
            perror("Unable to allocate buffer");
            exit(1);
        }

        printf(": ");
        // getline() returns the number of characters read
        input = getline(&buffer, &buffsize, stdin);
        
        /* Debug getline()
        printf("%zu characters were read.\n", input);
        printf("You typed: '%s'\n", buffer);
        */
        
        if (strcmp(buffer, "\n") == 0) {
            printf("I think this is a blank line.\n");
        } else {
            // Get the pid of smallsh and convert it to a string
            char pid_string[50];
            sprintf(pid_string, "%d", getpid());
            fflush(stdout);

            // Expand any $$ variables
            //buffer = expandVariables(buffer, "$$", pid_string);
            //printf("After expansion: %s\n", buffer);

            // Store the user's command line input
            struct commandLine command = storeCommand(buffer);
            // Debug correct elements stored in command struct
            //printf("Command stored in command.command: %s\n", command.command);
            printf("First argument stored is: %s\n", command.arguments[0]);
            printf("Second argument stored is: %s\n", command.arguments[1]);
            /*int i = 0;
            while (command.arguments[i] != NULL) {
                printf("command.arguments[%d] = %s\n", i, command.arguments[i]);
                i += 1;
            }*/
            printf("Input_file: %s\n", command.input_file);
            printf("Output_file: %s\n", command.output_file);
            printf("Ampersand: %s\n", command.ampersand);

            // Check for comment
            if (strcmp(command.arguments[0], "#") == 0) {
                continue;
            }

            // Remove newline characters from the command
            //command.command[strcspn(command.command, "\n")] = 0;
            command.arguments[0][strcspn(command.arguments[0], "\n")] = 0;
            if (command.arguments[1] != NULL) {
                command.arguments[1][strcspn(command.arguments[1], "\n")] = 0;
            }
            // Check for built-in commands
            if (strcmp(command.arguments[0], "exit") == 0) {
                exitShell();
            } else if (strcmp(command.arguments[0], "cd") == 0) {
                changeDirectory(command.arguments[1]);
            } else if (strcmp(command.arguments[0], "status") == 0) {
                getStatus(&wstatus);
            } else {
                wstatus = executeCommand(command);
                printf("wstatus after executeCommand: %d\n", wstatus);
            }

            // Clear memory of the command struct for the next loop
            memset(&command, 0, sizeof(command));
        }
    }
}
