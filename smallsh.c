/* main.c */
/* 0 copyright/licensing */
/* 1 includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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
    char ampersand;
};

int main(int argc, char *argv[]) {
    /* 7 command-line parsing */
    while(1) {
        char input[2048];
        printf(":");
        scanf("%s", input);
    }
}

/* 8 function declarations */