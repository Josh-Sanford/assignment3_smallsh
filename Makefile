setup:
	gcc -std=gnu99 -Wall -o smallsh smallsh.c storeCommand.c expandVariables.c exitShell.c changeDirectory.c

clean:
	rm smallsh
