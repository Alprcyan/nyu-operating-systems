#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "io.h"

char* substr(const char* str, size_t begin, size_t len)
{
 	return strndup(str + begin, len);
}

int main(int argc, char **argv)
{
	char *input_file = NULL;
	char alg = 0;
	int debug = 0;

	int i;
    for (i = 1; i < argc; i++)  /* Skip argv[0] (program name). */
    {
        if (strcmp(argv[i], "-v") == 0)  /* Process optional arguments. */
    		debug = 1;
        else if (strcmp(argv[i], "-sf") == 0)  /* Process optional arguments. */
        	alg = 'f';
        else if (strcmp(argv[i], "-ss") == 0)
        	alg = 's';
        else if (strcmp(argv[i], "-sS") == 0)
        	alg = 'S';
        else if (strcmp(argv[i], "-sC") == 0)
        	alg = 'C';
        else if (strcmp(argv[i], "-sF") == 0)
			alg = 'F';
        else
	    	input_file = argv[i];
    }

    if (input_file == NULL || alg == 0)
    {
		printf("Missing arguments.\n");
		printf("Usage: ./iosched -s[f,s,S,C,F] [-v] <input_file>\n");
		return 0;
	}

	set_algorithm(alg);
	set_debug(debug);
	process(input_file);

  	return 1;
}
