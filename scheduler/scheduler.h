#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

// pretty standard operation on reading files
// into memory. fopen, fseek, etc.
char* read_file(const char *filename);

#endif
