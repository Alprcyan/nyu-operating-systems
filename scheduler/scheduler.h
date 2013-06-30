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

typedef struct SetNode
{
	int cpu_time_left;
	int io_time_left;
	int wait_time;
	int io_time;
	int completed_time;
	int state;
	struct SetNode *next;
	struct ProcNode *proc;
} SetNode;
typedef struct SetNode* SetNodePtr;

void read_out(ProcNodePtr proc_head, SetNodePtr set_head);
ProcNodePtr read_proc_file(const char *filename);
SetNodePtr fcfs(ProcNodePtr proc_head);
SetNodePtr lcfs(ProcNodePtr proc_head);

#endif