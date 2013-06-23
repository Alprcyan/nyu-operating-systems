#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"
#include "randnum.h"

typedef struct ProcNode
{
	int arrival_time;
	int total_cpu_time;
	int cpu_burst;
	int io_burst;
	struct ProcNode *next;
} ProcNode;
typedef struct ProcNode* ProcNodePtr;

ProcNodePtr read_proc_file(const char *filename);

#endif