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

#define DEBUGGING 0

pid_t childProcesses[100];
int foreground_mode = 1;

void handle_SIGTSTP(int signo) {
    if (foreground_mode == 1) {
        char* message = "\nEntering foreground-only mode (& is now ignored)\n: ";
        write(STDOUT_FILENO, message, 52);
        foreground_mode = 0;
    } else if (foreground_mode == 0) {
        char* message = "\nExiting foreground-only mode\n: ";
        write(STDOUT_FILENO, message, 32);
        foreground_mode = 1;
    } else {
        perror("SIGTSTP signal handler");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    // getline() functionality adapted from https://c-for-dummies.com/blog/?p=1112
    // Initialize signal handler structs
    int* exit_status = malloc(sizeof(int));
    *exit_status = 0;
    int* termination_signal = malloc(sizeof(int));
    *termination_signal = 0;

    struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};

    // Assign and register the handler for SIGINT
    SIGINT_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &SIGINT_action, NULL);

    // Assign handler for SIGTSTP
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    // Block all catchable signals while handle_SIGTSTP is running
    sigfillset(&SIGTSTP_action.sa_mask);
    // Set no flags 
    SIGTSTP_action.sa_flags = SA_RESTART;
    // Register SIGTSTP_action
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);


    while(1) {
        char *buffer;
        size_t buffsize = 2048;

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
        getline(&buffer, &buffsize, stdin);
        
        if (DEBUGGING == 1) {
            // Debug getline()
            printf("You typed: '%s'\n", buffer);
        }
        
        /*if (strcmp(buffer, "\n") == 0) {
            if (DEBUGGING == 1) {
                printf("I think this is a blank line.\n");
            }
            continue;
        } else {*/
            // Get the pid of smallsh and convert it to a string
            char pid_string[50];
            sprintf(pid_string, "%d", getpid());
            fflush(stdout);

            // Expand any $$ variables
            buffer = expandVariables(buffer, "$$", pid_string);
            
            // Store the user's command line input
            struct commandLine *command = storeCommand(buffer);
            
            if (DEBUGGING == 1) {
                // Debug correct elements stored in command struct
                printf("First argument stored is: %s\n", command->arguments[0]);
                fflush(stdout);
                printf("Second argument stored is: %s\n", command->arguments[1]);
                fflush(stdout);
                printf("Input_file: %s\n", command->input_file);
                fflush(stdout);
                printf("Output_file: %s\n", command->output_file);
                fflush(stdout);
                printf("Ampersand: %s\n", command->ampersand);
                fflush(stdout);
            }

            // Check for comment
            /*if (command->arguments[0][0] == '#') {
                continue;
            }*/

            // Remove newline characters from the command
            if (command->arguments[0][0] != '\n') {
                command->arguments[0][strcspn(command->arguments[0], "\n")] = '\0';
            }
            int i = 1;
            while (command->arguments[i] != NULL) {
                command->arguments[i][strcspn(command->arguments[i], "\n")] = '\0';
                i += 1;
            }
            if (command->input_file != NULL) {
                command->input_file[strcspn(command->input_file, "\n")] = '\0';
            }
            if (command->output_file != NULL) {
                command->output_file[strcspn(command->output_file, "\n")] = '\0';
            }
            if (command->ampersand != NULL) {
                command->ampersand[strcspn(command->ampersand, "\n")] = '\0';
            }
            
            // Check for foreground only mode
            if (foreground_mode == 0) {
                command->ampersand = NULL;
            }

            // Check for built-in commands
            if (strcmp(command->arguments[0], "exit") == 0) {
                exitShell();
            } else if (strcmp(command->arguments[0], "cd") == 0) {
                changeDirectory(command->arguments[1]);
            } else if (strcmp(command->arguments[0], "status") == 0) {
                if (*termination_signal == 0) {
                    printf("exit value %d\n", *exit_status);
                    fflush(stdout);
                } else {
                    printf("terminated by signal %d\n", *termination_signal);
                    fflush(stdout);
                }
            } else {
                executeCommand(*command, childProcesses, exit_status, termination_signal);
                if (DEBUGGING == 1) {
                    // Debugging globals that keep up with exit and termination status
                    printf("exit_status after executeCommand: %d\n", *exit_status);
                    fflush(stdout);
                    printf("termination_signal after executeCommand: %d\n", *termination_signal);
                    fflush(stdout);
                }
            }

            // Clear memory of the command struct for the next loop
            free(buffer);
            free(command);
        //}
    }
}
