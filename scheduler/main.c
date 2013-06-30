#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scheduler.h"
#include "randnum.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Missing arguments.\n");
		printf("Usage: ./sched -[vs] <input_file> <rand_file>\n");
		return 0;
	}

	// F: First come first serve
	// L: Last come first serve
	// S: Shortest Job first

	char alg = 'F';
	char *input_file;
	char *rand_file;

	int i;
    for (i = 1; i < argc; i++)  /* Skip argv[0] (program name). */
    {
        if (strcmp(argv[i], "-sF") == 0)  /* Process optional arguments. */
        	alg = 'F';
        else if (strcmp(argv[i], "-sL") == 0)
        	alg = 'L';
        else if (strcmp(argv[i], "-sS") == 0)
        	alg = 'S';
        else if (strstr(argv[i], "-sR"))
        	alg = 'R';
        else
        	if (input_file == NULL)
	        	input_file = argv[i];
	        else
	        	rand_file = argv[i];
    }

    // printf("FILE: %s\n", input_file);
    // printf("RAND: %s\n", rand_file);

    // printf("Reading rand file...\n");
	read_rand_file(rand_file);

	// printf("Reading proc file...\n");
	ProcNodePtr head_proc = read_proc_file(input_file);

	SetNodePtr head_set = NULL;
	if (alg == 'L')
	{
		printf("LCFS\n");
		head_set = lcfs(head_proc);
	}
	else if (alg == 'F')
	{
		printf("FCFS\n");
		head_set = fcfs(head_proc);
	}


	read_out(head_proc, head_set);

	return 1;
}
