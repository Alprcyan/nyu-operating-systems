#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "randnum.h"
#include "mmu.h"

char* substr(const char* str, size_t begin, size_t len)
{
  return strndup(str + begin, len);
}

int main(int argc, char **argv)
{
	char alg = 0;
    int num_of_frames = 0;
	char *input_file = NULL;
	char *rand_file = NULL;

	int i;
    for (i = 1; i < argc; i++)  /* Skip argv[0] (program name). */
    {
        if (strcmp(argv[i], "-al") == 0)  /* Process optional arguments. */
        	alg = 'l';
        else if (strcmp(argv[i], "-ar") == 0)
        	alg = 'r';
        else if (strcmp(argv[i], "-af") == 0)
        	alg = 'f';
        else if (strcmp(argv[i], "-as") == 0)
        	alg = 's';
        else if (strcmp(argv[i], "-ac") == 0)
			alg = 'a';
        else if (strcmp(argv[i], "-aN") == 0)
			alg = 'N';
        else if (strcmp(argv[i], "-aC") == 0)
			alg = 'C';
        else if (strcmp(argv[i], "-aA") == 0)
			alg = 'A';
		else if (strstr(argv[i], "-o"))
		{
            // OPFS
		}
        else if (strstr(argv[i], "-f"))
        {
            num_of_frames = atoi(substr(argv[i], 2, strlen(argv[i])));
        }
        else
        	if (input_file == NULL)
	        	input_file = argv[i];
	        else
	        	rand_file = argv[i];
    }

    if (input_file == NULL || rand_file == NULL || alg == 0 || num_of_frames == 0)
    {
		printf("Missing arguments.\n");
		printf("Usage: ./mmu -a[N,l,r,f,s,c,C,a,A] -o[O,P,F,S] -f[int] <input_file> <rand_file>\n");
		return 0;
	}

    // Rand file.
    read_rand_file(rand_file);

    // Memory alg.
    choose_algorithm(alg);
    create_frames(num_of_frames);
    process(input_file);

	return 1;
}
