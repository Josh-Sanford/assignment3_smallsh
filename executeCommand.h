#ifndef EXECUTECOMMAND_H
#define EXECUTECOMMAND_H

/*
 * Execute the command input by the user
 * Returns the status of the child process
 */
void executeCommand(struct commandLine command, pid_t childProcesses[100], int* exit_status, int* termination_signal);

#endif