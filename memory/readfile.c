#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// pretty standard operation on reading files
// into memory. fopen, fseek, etc.
char* read_file(const char *filename)
{
   // open file operation
   FILE *file = fopen(filename, "r");
   if (file != NULL) // null = no such file
   {
      // seek to end to detect how big
      // the file is.
      fseek(file, 0, SEEK_END);
      long size = ftell(file)+1;
      fseek(file, 0, SEEK_SET);

      // if the file is empty
      // it is also very useless
      // to us.
      if (size == 0)
      {
         fclose(file);
         return NULL;
      }

      // take the file contents and place it into a pointer.
      char* contents = calloc(size, sizeof(char));
      fread(contents, size, 1, file);

      // we have to duplicate the contents because when
      // we close the file, the contents of where we were
      // pointing too also gets freed, which is bad for us.
      char* output = strdup(contents);

      // free the contents, close the file.
      free(contents);
      fclose(file);

      return output;
   }

   return NULL;
}