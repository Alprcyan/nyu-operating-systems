#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scheduler.h"

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      printf("Missing arguments.\n");
      printf("Usage: ./scheduler <input_file>\n");
      return 0;
   }

   char * input_file = argv[1];

   // do something with the linker here.
   parse_file(input_file);

   return 1;
}
