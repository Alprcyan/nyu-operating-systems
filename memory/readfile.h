#ifndef READFILE_H
#define READFILE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// pretty standard operation on reading files
// into memory. fopen, fseek, etc.
char* read_file(const char *filename);

#endif
