#include "storeCommand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/* Separates the user's command line input by spaces and stores
 * the components in the commandLine struct
 */
struct commandLine storeCommand(char *buffer) {
    struct commandLine command;

    //printf("The buffer passed to parseCommand() is %s\n", buffer);
    
    char *token = strtok(buffer, " ");
    //printf("The first token is %s\n", token);

    // Store the first token as the command in the struct
    //command.command = token;
    command.arguments[0] = token;

    // Check first token for # to see if it is a comment
    if (strcmp(token, "#") == 0) {
        return command;
    }

    // Add command to first index of arguments since it seems
    // that's how execvp() wants the arguments array to be
    //command.arguments[0] = token;

    // Progress to next token
    //token = strtok(NULL, " ");

    // Store arguments until < or > or & is encountered
    //printf("Storing these arguments:\n");
    int argumentIndex = 0;
    while (token != NULL && strcmp(token, "<") != 0 && strcmp(token, ">") != 0 && strcmp(token, "&") != 0
    && strcmp(token, "&\n") != 0) {
        //printf("%s\n", token);
        command.arguments[argumentIndex] = token;
        argumentIndex += 1;
        token = strtok(NULL, " ");
    }
    // Add null pointer to end of arguments array for use with execvp later
    command.arguments[argumentIndex] = NULL;
    //printf("End of arguments.\n");
    
    // Get the rest of the tokens
    while (token != NULL) {
        //printf("%s\n", token);

        // Check for <
        if (strcmp(token, "<") == 0) {
            // Move token to next to get input_file
            token = strtok(NULL, " ");
            if (token == NULL) {
                perror("No input file provided");
                break;
            }
            //printf("input file found: %s\n", token);
            command.input_file = token; // TODO: os1 puts printf in here
        }

        if (strcmp(token, ">") == 0) {
            // Move token to next to get output_file
            token = strtok(NULL, " ");
            if (token == NULL) {
                perror("No output file provided.");
                break;
            }
            //printf("output file found: %s\n", token);
            command.output_file = token;
        }

        if (strcmp(token, "&") == 0 || strcmp(token, "&\n") == 0) {
            //printf("ampersand found: %s\n", token);
            command.ampersand = token;
        }
        token = strtok(NULL, " ");
    }

    return command;

}