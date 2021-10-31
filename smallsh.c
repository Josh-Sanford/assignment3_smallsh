#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "storeCommand.h"
#include "expandVariables.h"
#include "exitShell.h"
#include "changeDirectory.h"
#include "getStatus.h"
#include "executeCommand.h"


void handle_SIGINT(int signo) {
    char* message = "Caught SIGINT, sleeping for 10 seconds\n";
	write(STDOUT_FILENO, message, 39);
	// Sleep for 10 seconds
	sleep(10);
}

int wstatus = 0;
pid_t childProcesses[100];

int main(int argc, char *argv[]) {
    // getline() functionality adapted from https://c-for-dummies.com/blog/?p=1112
    /*
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = handle_SIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);*/

    while(1) {
        char *buffer;
        size_t buffsize = 2048;
        size_t input = 0;

        buffer = (char *)malloc(buffsize * sizeof(char));
        if (buffer == NULL) {
            perror("Unable to allocate buffer");
            exit(1);
        }

        /*printf("Current child process: %d\n", childProcesses[0]);
        fflush(stdout);
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
                    printf("background pid %d is done\n", checkPid);
                }
                index += 1;
            }
        }*/
        printf(": ");
        fflush(stdout);
        // getline() returns the number of characters read
        input = getline(&buffer, &buffsize, stdin);
        
        /* Debug getline()
        printf("%zu characters were read.\n", input);
        printf("You typed: '%s'\n", buffer);
        */
        
        if (strcmp(buffer, "\n") == 0) {
            //printf("I think this is a blank line.\n");
            continue;
        } else {
            // Get the pid of smallsh and convert it to a string
            char pid_string[50];
            sprintf(pid_string, "%d", getpid());
            fflush(stdout);

            // Expand any $$ variables
            printf("Before expandVariables\n");
            fflush(stdout);
            buffer = expandVariables(buffer, "$$", pid_string);
            printf("After expandVariables\n");
            fflush(stdout);
            // Store the user's command line input
            struct commandLine command = storeCommand(buffer);
            // Debug correct elements stored in command struct
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
            command.arguments[0][strcspn(command.arguments[0], "\n")] = '\0';
            int i = 1;
            while (command.arguments[i] != NULL) {
                command.arguments[i][strcspn(command.arguments[i], "\n")] = '\0';
                i += 1;
            }
            if (command.input_file != NULL) {
                command.input_file[strcspn(command.input_file, "\n")] = '\0';
            }
            if (command.output_file != NULL) {
                command.output_file[strcspn(command.output_file, "\n")] = '\0';
            }
            if (command.ampersand != NULL) {
                command.ampersand[strcspn(command.ampersand, "\n")] = '\0';
            }
            // Check for built-in commands
            if (strcmp(command.arguments[0], "exit") == 0) {
                exitShell();
            } else if (strcmp(command.arguments[0], "cd") == 0) {
                changeDirectory(command.arguments[1]);
            } else if (strcmp(command.arguments[0], "status") == 0) {
                getStatus(&wstatus);
            } else {
                wstatus = executeCommand(command, childProcesses);
                //printf("wstatus after executeCommand: %d\n", wstatus);
            }

            // Clear memory of the command struct for the next loop
            free(buffer);
            memset(&command, 0, sizeof(command));
        }
    }
}
