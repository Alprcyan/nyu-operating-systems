#ifndef MMU_H
#define MMU_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uthash.h"
#include "randnum.h"
#include "readfile.h"

void choose_algorithm(char algorithm);

void create_frames(int count);

void process(const char *file);

#endif
