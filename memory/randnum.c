#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

int* rand_nums;
int count = 0;

int rand_num(int burst)
{
   return 1 + (rand_nums[count++] % burst);
}

void read_rand_file(const char* input_file)
{
   char *contents = read_file(input_file);

   char *token;
   char *tokenizer;

   token = strtok_r(contents, " \n\t", &tokenizer);
   int num_of_rands = atoi(token);
   int init[num_of_rands];
   rand_nums = init;

   token = strtok_r(NULL, " \n\t", &tokenizer);
   int pos = 0;
   while (token != NULL)
   {
      rand_nums[pos] = atoi(token);

      pos++;
      token = strtok_r(NULL, " \n\t", &tokenizer);
   }
}