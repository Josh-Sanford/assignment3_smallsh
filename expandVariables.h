#ifndef EXPANDVARIABLES_H
#define EXPANDVARIABLES_H

/* Searches the commandLine input for instances of $$ and, if found,
 * expands each $$ instance to the process ID of smallsh
 * Adapted from: https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
 */
char* expandVariables(const char* s, const char* oldW, const char* newW);

#endif