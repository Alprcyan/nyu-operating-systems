#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

long unsigned int *rand_nums;
int count = 0;
int num_of_rands = 0;

int rand_num(int burst)
{
	int number = 1 + (rand_nums[count++] % burst);

	if (count >= num_of_rands)
		count = 0;

	return number;
}

void read_rand_file(const char* input_file)
{
	char *contents = read_file(input_file);

	char *token;
	char *tokenizer;

	token = strtok_r(contents, " \n\t", &tokenizer);
	num_of_rands = atoi(token);
	rand_nums = malloc(sizeof(long unsigned int) * num_of_rands);

	token = strtok_r(NULL, " \n\t", &tokenizer);
	int pos = 0;
	while (token != NULL)
	{
		rand_nums[pos] = atoi(token);

		pos++;
		token = strtok_r(NULL, " \n\t", &tokenizer);
	}
}
