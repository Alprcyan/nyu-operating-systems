#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"
#include "randnum.h"

enum {AT, CT, CB, IB};

typedef struct ProcNode
{
	int arrival_time;
	int total_cpu_time;
	int cpu_burst;
	int io_burst;
	struct ProcNode *next;
} ProcNode;
typedef struct ProcNode* ProcNodePtr;

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