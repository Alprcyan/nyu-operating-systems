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
	int id;
	int arrival_time;
	int total_cpu_time;
	int cpu_burst;
	int io_burst;
	int arrived;
	struct ProcNode *next;
} ProcNode;
typedef struct ProcNode* ProcNodePtr;

typedef struct SetNode
{
	int cpu_time_left;
	int cpu_burst_left;
	int io_time_left;
	int wait_time;
	int io_time;
	struct SetNode *next;
	struct ProcNode *proc;
} SetNode;
typedef struct SetNode* SetNodePtr;

void print_set(SetNodePtr set)
{
	printf("\t[ID: %d, REM: %d]", set->proc->id, set->cpu_time_left);
}

void print_sets(SetNodePtr set_head)
{
	SetNodePtr set_ptr = set_head;
	printf("\n\n");
	while (set_ptr != NULL)
	{
		print_set(set_ptr);
		set_ptr = set_ptr->next;
	}
	printf("\n\n");
}

void print_out(ProcNodePtr proc_head, SetNodePtr set_head, int total_io_wait_time)
{
	int ended = 0;
	int total_cpu_time = 0;
	int total_turnaround_time;
	int number_of_sets = 0;
	int total_cpu_waiting = 0;

	ProcNodePtr proc_ptr = proc_head;
	while (proc_ptr != NULL)
	{
		SetNodePtr set_ptr = set_head;
		while(set_ptr != NULL)
		{
			if (set_ptr->proc == proc_ptr)
				break;

			set_ptr = set_ptr->next;
		}

		int this_set_ended =
			proc_ptr->arrival_time
			+ proc_ptr->total_cpu_time
			+ set_ptr->wait_time
			+ set_ptr->io_time;

		int this_total_time =
			proc_ptr->total_cpu_time
			+ set_ptr->wait_time
			+ set_ptr->io_time;

		if (this_set_ended > ended)
			ended = this_set_ended;

		total_cpu_time += this_total_time - (set_ptr->io_time + set_ptr->wait_time);
		total_turnaround_time += this_total_time;
		total_cpu_waiting += set_ptr->wait_time;

		// print out stuff here
		printf(
			"%04d:\t%d\t%d\t%d\t%d\t|\t%d\t%d\t%d\t%d\n",
			proc_ptr->id,
			proc_ptr->arrival_time,
			proc_ptr->total_cpu_time,
			proc_ptr->cpu_burst,
			proc_ptr->io_burst,
			this_set_ended,
			this_total_time,
			set_ptr->io_time,
			set_ptr->wait_time
		);

		proc_ptr = proc_ptr->next;
		number_of_sets++;
	}

	printf("SUM:\t%d\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.3lf\n",
		ended,
		(float) total_cpu_time/ended * 100,
		(float) total_io_wait_time/ended * 100,
		(float) total_turnaround_time/number_of_sets,
		(float) total_cpu_waiting/number_of_sets,
		(float) number_of_sets / (float) ended * 100
	);
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
	int c = 0;
	while (token != NULL)
	{
		int numeric_token = atoi(token);;

		if (curr_type == AT)
		{
			ProcNodePtr new_proc = (ProcNodePtr) malloc(sizeof(struct ProcNode));
			new_proc->arrived = 0;
			new_proc->next = NULL;
			new_proc->id = c;
			c++;

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

int is_completed(ProcNodePtr proc_head, SetNodePtr set_head)
{
	int set_count = 0;
	SetNodePtr set_ptr = set_head;
	while(set_ptr != NULL)
	{
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

SetNodePtr add_to_end(SetNodePtr head, SetNodePtr node)
{
	SetNodePtr set_ptr = head;
	SetNodePtr prev_set_ptr = NULL;
	while (set_ptr != NULL)
	{
		prev_set_ptr = set_ptr;
		set_ptr = set_ptr->next;
	}

	if (prev_set_ptr == NULL)
		head = node;
	else
		prev_set_ptr->next = node;

	node->next = NULL;

	return head;
}

void fcfs(ProcNodePtr proc_head)
{
	int cpu_count = 0;

	int time_spent_in_io = 0;

	SetNodePtr ready_set_head = NULL;
	SetNodePtr blocked_set_head = NULL;
	SetNodePtr done_set_head = NULL;
	SetNodePtr running_set = NULL;

	while (is_completed(proc_head, done_set_head) == 0)
	{
		// Finding all things that are ready.
		ProcNodePtr proc_ptr = proc_head;
		// printf("(%d)\t", cpu_count);

		while (proc_ptr != NULL)
		{
			if (proc_ptr->arrival_time == cpu_count)
			{
				SetNodePtr new_set = (SetNodePtr) malloc(sizeof(struct SetNode));
				new_set->cpu_time_left = proc_ptr->total_cpu_time;
				new_set->io_time_left = 0;
				new_set->wait_time = 0;
				new_set->io_time = 0;
				new_set->proc = proc_ptr;
				new_set->next = NULL;

				ready_set_head = add_to_end(ready_set_head, new_set);
				// printf("\tREADY: %d", proc_ptr->id);
			}

			proc_ptr = proc_ptr->next;
		}

		// This is for caveat purposes, need to detect how much
		// time we've spent in IO waiting.
		if (blocked_set_head != NULL)
			time_spent_in_io++;

		// Decrease io_wait remaining on all blocked.
		SetNodePtr blocked_set_ptr = blocked_set_head;
		SetNodePtr prev_blocked_set_ptr = NULL;
		while (blocked_set_ptr != NULL)
		{
			blocked_set_ptr->io_time_left--;
			blocked_set_ptr->io_time++;

			if (blocked_set_ptr->io_time_left == 0)
			{
				SetNodePtr next = blocked_set_ptr->next;

				if (prev_blocked_set_ptr == NULL)
					blocked_set_head = next;
				else
					prev_blocked_set_ptr->next = next;

				ready_set_head = add_to_end(ready_set_head, blocked_set_ptr);
				// printf("\tREADY: %d", blocked_set_ptr->proc->id);
				blocked_set_ptr = next;
				continue;
			}

			prev_blocked_set_ptr = blocked_set_ptr;
			blocked_set_ptr = blocked_set_ptr->next;
		}

		// Decrease cpu time on current
		if (running_set != NULL)
		{
			running_set->cpu_burst_left--;
			running_set->cpu_time_left--;

			if (running_set->cpu_burst_left == 0 || running_set->cpu_time_left == 0)
			{

				if (running_set->cpu_time_left == 0)
				{
					// printf("\tDONE: %d", running_set->proc->id);
					done_set_head = add_to_end(done_set_head, running_set);
				}
				else
				{
					// printf("\tEXPIRED: %d", running_set->proc->id);
					// add it to the blocked queue
					// printf("\tREM: %d", running_set->cpu_time_left);
					running_set->io_time_left = rand_num(running_set->proc->io_burst);
					// printf("\tIO: %d", running_set->io_time_left);
					blocked_set_head = add_to_end(blocked_set_head, running_set);
				}

				running_set = NULL;
			}
		}

		// Lets pop one off the ready_set_head
		if (running_set == NULL)
		{
			if (ready_set_head != NULL)
			{
				running_set = ready_set_head;
				ready_set_head = ready_set_head->next;
				running_set->next = NULL;

				int randnum = rand_num(running_set->proc->cpu_burst);
				running_set->cpu_burst_left = randnum;
				// printf("\tCPU: %d", running_set->cpu_burst_left);
				// printf("\tRUNNING: %d", running_set->proc->id);
			}
		}

		// Increment ready state wait.
		SetNodePtr ready_set_ptr = ready_set_head;
		while (ready_set_ptr != NULL)
		{
			ready_set_ptr->wait_time++;
			ready_set_ptr = ready_set_ptr->next;
		}

		// printf("\n");
		cpu_count++;
	}

	print_out(proc_head, done_set_head, time_spent_in_io);
}

void lcfs(ProcNodePtr proc_head)
{
	int cpu_count = 0;

	int time_spent_in_io = 0;

	SetNodePtr ready_set_head = NULL;
	SetNodePtr blocked_set_head = NULL;
	SetNodePtr done_set_head = NULL;
	SetNodePtr running_set = NULL;

	while (is_completed(proc_head, done_set_head) == 0)
	{
		// Finding all things that are ready.
		ProcNodePtr proc_ptr = proc_head;
		// printf("(%d)\t", cpu_count);

		while (proc_ptr != NULL)
		{
			if (proc_ptr->arrival_time == cpu_count)
			{
				SetNodePtr new_set = (SetNodePtr) malloc(sizeof(struct SetNode));
				new_set->cpu_time_left = proc_ptr->total_cpu_time;
				new_set->io_time_left = 0;
				new_set->wait_time = 0;
				new_set->io_time = 0;
				new_set->proc = proc_ptr;
				new_set->next = NULL;

				ready_set_head = add_to_end(ready_set_head, new_set);
				// printf("\tREADY: %d", proc_ptr->id);
			}

			proc_ptr = proc_ptr->next;
		}

		// This is for caveat purposes, need to detect how much
		// time we've spent in IO waiting.
		if (blocked_set_head != NULL)
			time_spent_in_io++;

		// Decrease io_wait remaining on all blocked.
		SetNodePtr blocked_set_ptr = blocked_set_head;
		SetNodePtr prev_blocked_set_ptr = NULL;
		while (blocked_set_ptr != NULL)
		{
			blocked_set_ptr->io_time_left--;
			blocked_set_ptr->io_time++;

			if (blocked_set_ptr->io_time_left == 0)
			{
				SetNodePtr next = blocked_set_ptr->next;

				if (prev_blocked_set_ptr == NULL)
					blocked_set_head = next;
				else
					prev_blocked_set_ptr->next = next;

				ready_set_head = add_to_end(ready_set_head, blocked_set_ptr);
				// printf("\tREADY: %d", blocked_set_ptr->proc->id);
				blocked_set_ptr = next;
				continue;
			}

			prev_blocked_set_ptr = blocked_set_ptr;
			blocked_set_ptr = blocked_set_ptr->next;
		}

		// Decrease cpu time on current
		if (running_set != NULL)
		{
			running_set->cpu_burst_left--;
			running_set->cpu_time_left--;

			if (running_set->cpu_burst_left == 0 || running_set->cpu_time_left == 0)
			{

				if (running_set->cpu_time_left == 0)
				{
					// printf("\tDONE: %d", running_set->proc->id);
					done_set_head = add_to_end(done_set_head, running_set);
				}
				else
				{
					// printf("\tEXPIRED: %d", running_set->proc->id);
					// add it to the blocked queue
					// printf("\tREM: %d", running_set->cpu_time_left);
					running_set->io_time_left = rand_num(running_set->proc->io_burst);
					// printf("\tIO: %d", running_set->io_time_left);
					blocked_set_head = add_to_end(blocked_set_head, running_set);
				}

				running_set = NULL;
			}
		}

		// Lets pop one off from the end. LCFS.
		if (running_set == NULL)
		{
			if (ready_set_head != NULL)
			{
				SetNodePtr running_set_ptr = ready_set_head;
				SetNodePtr prev_running_set_ptr = NULL;
				while (running_set_ptr->next != NULL)
				{
					prev_running_set_ptr = running_set_ptr;
					running_set_ptr = running_set_ptr->next;
				}

				if (prev_running_set_ptr == NULL)
					ready_set_head = NULL;
				else
					prev_running_set_ptr->next = NULL;

				running_set = running_set_ptr;
				running_set->next = NULL;
				running_set->cpu_burst_left = rand_num(running_set->proc->cpu_burst);
			}
		}

		// Increment ready state wait.
		SetNodePtr ready_set_ptr = ready_set_head;
		while (ready_set_ptr != NULL)
		{
			ready_set_ptr->wait_time++;
			ready_set_ptr = ready_set_ptr->next;
		}

		// printf("\n");
		cpu_count++;
	}

	print_out(proc_head, done_set_head, time_spent_in_io);
}

void sjf(ProcNodePtr proc_head)
{
	int cpu_count = 0;

	int time_spent_in_io = 0;

	SetNodePtr ready_set_head = NULL;
	SetNodePtr blocked_set_head = NULL;
	SetNodePtr done_set_head = NULL;
	SetNodePtr running_set = NULL;

	while (is_completed(proc_head, done_set_head) == 0)
	{
		// Finding all things that are ready.
		ProcNodePtr proc_ptr = proc_head;
		// printf("(%d)\t", cpu_count);

		while (proc_ptr != NULL)
		{
			if (proc_ptr->arrival_time == cpu_count)
			{
				SetNodePtr new_set = (SetNodePtr) malloc(sizeof(struct SetNode));
				new_set->cpu_time_left = proc_ptr->total_cpu_time;
				new_set->io_time_left = 0;
				new_set->wait_time = 0;
				new_set->io_time = 0;
				new_set->proc = proc_ptr;
				new_set->next = NULL;

				ready_set_head = add_to_end(ready_set_head, new_set);
				// printf("\tREADY: %d", proc_ptr->id);
			}

			proc_ptr = proc_ptr->next;
		}

		// This is for caveat purposes, need to detect how much
		// time we've spent in IO waiting.
		if (blocked_set_head != NULL)
			time_spent_in_io++;

		// Decrease io_wait remaining on all blocked.
		SetNodePtr blocked_set_ptr = blocked_set_head;
		SetNodePtr prev_blocked_set_ptr = NULL;
		while (blocked_set_ptr != NULL)
		{
			blocked_set_ptr->io_time_left--;
			blocked_set_ptr->io_time++;

			if (blocked_set_ptr->io_time_left == 0)
			{
				SetNodePtr next = blocked_set_ptr->next;

				if (prev_blocked_set_ptr == NULL)
					blocked_set_head = next;
				else
					prev_blocked_set_ptr->next = next;

				ready_set_head = add_to_end(ready_set_head, blocked_set_ptr);
				// printf("\tREADY: %d", blocked_set_ptr->proc->id);
				blocked_set_ptr = next;
				continue;
			}

			prev_blocked_set_ptr = blocked_set_ptr;
			blocked_set_ptr = blocked_set_ptr->next;
		}

		// Decrease cpu time on current
		if (running_set != NULL)
		{
			running_set->cpu_burst_left--;
			running_set->cpu_time_left--;

			if (running_set->cpu_burst_left == 0 || running_set->cpu_time_left == 0)
			{

				if (running_set->cpu_time_left == 0)
				{
					// printf("\tDONE: %d", running_set->proc->id);
					done_set_head = add_to_end(done_set_head, running_set);
				}
				else
				{
					// printf("\tEXPIRED: %d", running_set->proc->id);
					// add it to the blocked queue
					// printf("\tREM: %d", running_set->cpu_time_left);
					running_set->io_time_left = rand_num(running_set->proc->io_burst);
					// printf("\tIO: %d", running_set->io_time_left);
					blocked_set_head = add_to_end(blocked_set_head, running_set);
				}

				running_set = NULL;
			}
		}

		// Lets pop one off from the end. SJF.
		if (running_set == NULL)
		{
			if (ready_set_head != NULL)
			{
				// print_sets(ready_set_head);

				SetNodePtr shortest_set = ready_set_head;
				SetNodePtr prev_shortest_set = NULL;
				int shortest_time = shortest_set->cpu_time_left;

				SetNodePtr scanning_set_ptr = ready_set_head;
				SetNodePtr prev_scanning_set_ptr = NULL;
				while (scanning_set_ptr != NULL)
				{
					if (scanning_set_ptr->cpu_time_left < shortest_time)
					{
						shortest_set = scanning_set_ptr;
						prev_shortest_set = prev_scanning_set_ptr;
						shortest_time = scanning_set_ptr->cpu_time_left;
					}

					prev_scanning_set_ptr = scanning_set_ptr;
					scanning_set_ptr = scanning_set_ptr->next;
				}

				if (prev_shortest_set == NULL)
					ready_set_head = ready_set_head->next;
				else
					prev_shortest_set->next = shortest_set->next;

				running_set = shortest_set;
				running_set->next = NULL;
				running_set->cpu_burst_left = rand_num(running_set->proc->cpu_burst);
				// printf("\tRUNNING: %d", running_set->proc->id);
				// printf("\tCPU: %d", running_set->cpu_burst_left);
				// print_set(running_set);
			}
		}

		// Increment ready state wait.
		SetNodePtr ready_set_ptr = ready_set_head;
		while (ready_set_ptr != NULL)
		{
			ready_set_ptr->wait_time++;
			ready_set_ptr = ready_set_ptr->next;
		}

		// printf("\n");
		cpu_count++;
	}

	print_out(proc_head, done_set_head, time_spent_in_io);
}

void rr(ProcNodePtr proc_head, int interval)
{

}