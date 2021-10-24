#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "storeCommand.h"
#include "expandVariables.h"

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
            buffer = expandVariables(buffer, "$$", pid_string);
            printf("After expansion: %s\n", buffer);

            // Store the user's command line input
            struct commandLine command = storeCommand(buffer);
            // Debug correct elements stored in command struct
            printf("Command stored in command.command: %s\n", command.command);
            int i = 0;
            while (command.arguments[i] != NULL) {
                printf("command.arguments[%d] = %s\n", i, command.arguments[i]);
                i += 1;
            }
            printf("Input_file: %s\n", command.input_file);
            printf("Output_file: %s\n", command.output_file);
            printf("Ampersand: %s\n", command.ampersand);

            // Check for comment
            if (strcmp(command.command, "#") == 0) {
                continue;
            }
        }
    }
}
