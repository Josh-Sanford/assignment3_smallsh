#ifndef STORECOMMAND_H
#define STORECOMMAND_H

struct commandLine {
    //char *command;
    char *arguments[512];
    char *input_file;
    char *output_file;
    char *ampersand;
};

/* Separates the user's command line input by spaces and stores
 * the components in the commandLine struct
 */
struct commandLine storeCommand(char *buffer);

#endif