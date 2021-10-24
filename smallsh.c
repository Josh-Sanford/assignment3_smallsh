/* main.c */
/* 0 copyright/licensing */
/* 1 includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "storeCommand.h"
/* 2 defines */
/* 3 external declarations */
/* 4 typedefs */
/* 5 global variable declarations */
/* 6 function prototypes */

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

struct commandLine {
    char *command;
    char *arguments[512];
    char *input_file;
    char *output_file;
    char *ampersand;
};

int main(int argc, char *argv[]) {
    /* getline() functionality adopted from https://c-for-dummies.com/blog/?p=1112
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
        printf("%zu characters were read.\n", input);
        printf("You typed: '%s'\n", buffer);
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
    }
}

/* 8 function declarations */