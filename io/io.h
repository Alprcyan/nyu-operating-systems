#ifndef IOSCHED_H
#define IOSCHED_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "uthash.h"
#include "readfile.h"

void set_algorithm(char selected_alg);
void set_debug(int activate);
void process();

#endif
