#ifndef STORECOMMAND_H
#define STORECOMMAND_H

/* Separates the user's command line input by spaces and stores
 * the components in the commandLine struct
 */
struct commandLine storeCommand(char *buffer);

#endif