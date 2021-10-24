setup:
	gcc -std=gnu99 -Wall -o smallsh smallsh.c storeCommand.c expandVariables.c

clean:
	rm smallsh
