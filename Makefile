setup:
	gcc -std=gnu99 -Wall -o smallsh smallsh.c storeCommand.c expandVariables.c exitShell.c changeDirectory.c executeCommand.c

clean:
	rm smallsh
