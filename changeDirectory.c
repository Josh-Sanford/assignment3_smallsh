#include "changeDirectory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEBUGGING 0

/*
 * Changes the current directory to the path specified by
 * argument, or to the HOME variable if no arguments is specified.
 */
void changeDirectory(char *path) {
    // See if argument is specified
    if (path == NULL) {
        path = getenv("HOME");
    }

    // Remove any newlines from path
    path[strcspn(path, "\n")] = 0;
    
    if (DEBUGGING == 1) {
        // Debug path
        printf("Value stored in path is: %s\n", path);
        fflush(stdout);
        // Check if argument is still NULL ?
    }

    if (DEBUGGING == 1) {
    // Print working directory for debugging
        char s[100];
        printf("%s\n", getcwd(s, 100));
        fflush(stdout);
    }
    
    // Change the directory
    int change = chdir(path);
    if (change != 0) {
        perror("Change of directory not successful.");
    }

    if (DEBUGGING == 1) {
    // Print directory changed to for debugging
        char n[100];
        printf("%s\n", getcwd(n, 100));
        fflush(stdout);
    }
}