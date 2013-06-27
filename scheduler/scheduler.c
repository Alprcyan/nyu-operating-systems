#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"
#include "randnum.h"

enum {AT, CT, CB, IB};
enum {STATE_RUNNING, STATE_READY, STATE_BLOCKED, STATE_COMPLETED};

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

void read_out(ProcNodePtr proc_head, SetNodePtr set_head)
{

}

int is_completed(ProcNodePtr proc_head, SetNodePtr set_head)
{
	int set_count = 0;
	SetNodePtr set_ptr = set_head;
	while(set_ptr != NULL)
	{
		if (set_ptr->state != STATE_COMPLETED)
			return 0;

		set_count++;
		set_ptr = set_ptr->next;
	}

	int proc_count = 0;
	ProcNodePtr proc_ptr = proc_head;
	while(proc_ptr != NULL)
	{
		proc_count++;
		proc_ptr = proc_ptr->next;
	}

	if (proc_count == set_count)
		return 1;

	return 0;
}

SetNodePtr fcfs(ProcNodePtr proc_head)
{
	int cpu_clock = 0;

	SetNodePtr set_head = NULL;
	SetNodePtr curr_set = NULL;

	int cpu_burst = 0;

	while(is_completed(proc_head, set_head) == 0)
	{
		ProcNodePtr proc_ptr = proc_head;
		while(proc_ptr != NULL)
		{
			if (proc_ptr->arrival_time >= cpu_clock)
			{
				SetNodePtr new_set = (SetNodePtr) malloc(sizeof(struct SetNode));
				new_set->cpu_time_left = proc_ptr->total_cpu_time;
				new_set->wait_time = 0;
				new_set->io_time = 0;
				new_set->state = STATE_READY;
				new_set->proc = proc_ptr;

				if (curr_set != NULL)
					curr_set->next = new_set;

				curr_set = new_set;

				if (set_head == NULL)
					set_head = curr_set;
			}

			proc_ptr = proc_ptr->next;
		}

		SetNodePtr set_ptr = set_head;
		while(set_ptr != NULL)
		{
			if (set_ptr->cpu_time_left >= 0 && set_ptr->io_time_left <= 0)
				break;

			set_ptr = set_ptr->next;
		}

		if (set_ptr == NULL)
		{
			cpu_burst++;
			continue;
		}

		SetNodePtr running_set = set_ptr;

		cpu_burst = rand_num(running_set->proc->cpu_burst);
		if (running_set->cpu_time_left > cpu_burst)
		{
			running_set->cpu_time_left -= cpu_burst;
			running_set->io_time_left = rand_num(running_set->proc->io_burst);
			running_set->state = STATE_BLOCKED;
		}
		else
		{
			cpu_burst = running_set->cpu_time_left;
			running_set->cpu_time_left = 0;
			running_set->state = STATE_COMPLETED;
		}

		set_ptr = set_head;
		while(set_ptr != NULL)
		{
			if (set_ptr == running_set)
				continue;

			if (set_ptr->io_time_left > cpu_burst)
			{
				set_ptr->io_time_left -= cpu_burst;
			}
			else
			{
				set_ptr->wait_time += (cpu_burst - set_ptr->io_time_left);
				set_ptr->io_time_left = 0;
				set_ptr->state = STATE_READY;
			}
		}
	}

	return set_head;
}

ProcNodePtr read_proc_file(const char *input_file)
{
	ProcNodePtr head = NULL;
	ProcNodePtr curr = NULL;

	char *token;
	char *tokenizer;

	int curr_type = AT;
	char *contents = read_file(input_file);
	token = strtok_r(contents, " \n\t", &tokenizer);
	while (token != NULL)
	{
		int numeric_token = atoi(token);

		if (curr_type == AT)
		{
			ProcNodePtr new_proc = (ProcNodePtr) malloc(sizeof(struct ProcNode));

			if (curr != NULL)
				curr->next = new_proc;

			curr = new_proc;

			if (head == NULL)
				head = curr;

			curr->arrival_time = numeric_token;
		}
		else if (curr_type == CT)
		{
			curr->total_cpu_time = numeric_token;
		}
		else if (curr_type == CB)
		{
			curr->cpu_burst = numeric_token;
		}
		else if (curr_type == IB)
		{
			curr->io_burst = numeric_token;
			curr_type = -1;
		}

		curr_type++;
		token = strtok_r(NULL, " \n\t", &tokenizer);
	}

	return head;
}