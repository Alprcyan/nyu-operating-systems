#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "linker.h"

int main(int argc, char **argv)
{
   if (argc < 3)
   {
      printf("Missing arguments.\n");
      printf("Usage: ./linker <input_file> <output_file>\n");
      return 0;
   }

   char * input_file = argv[1];
   char * output_file = argv[2];

   // do something with the linker here.
   parse_file(input_file, output_file);

   return 1;
}
