#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "uthash.h"
#include "readfile.h"

struct RequestNode
{
	int id;
	int arrival_time;
	int track_number;
	int start_time;
	int completion_time;
	struct RequestNode* next;
} RequestNode;
typedef struct RequestNode* RequestNodePtr;

RequestNodePtr request_head = NULL;

struct ProcessNode
{
	struct RequestNode* node;
	struct ProcessNode* next;
} ProcessNode;
typedef struct ProcessNode* ProcessNodePtr;

ProcessNodePtr process_head = NULL;

char alg = 'f';
int debug = 0;

int total_time = 0;
int total_movement = 0;

void _print_requests_table()
{
	RequestNodePtr request_ptr = request_head;
	printf("IOREQS INFO\n");
	while(request_ptr != NULL)
	{
		printf(
			"\t%d:\t%d\t%d\t%d\n",
			request_ptr->id,
			request_ptr->arrival_time,
			request_ptr->start_time,
			request_ptr->completion_time
		);

		request_ptr = request_ptr->next;
	}
}

void _print_summary()
{
	int total_wait_time = 0;
	int max_wait_time = 0;
	int total_processing_time = 0;
	int number_of_requests = 0;

	RequestNodePtr request_ptr = request_head;
	while (request_ptr != NULL)
	{
		int io_time = request_ptr->completion_time - request_ptr->start_time;
		int total_time = request_ptr->completion_time - request_ptr->arrival_time;
		int wait_time = total_time - io_time;

		if (wait_time > max_wait_time)
		{
			max_wait_time = wait_time;
		}

		total_wait_time += wait_time;
		total_processing_time += total_time;
		number_of_requests++;

		request_ptr = request_ptr->next;
	}

	printf(
		"SUM: %d %d %.2lf %.2lf %d\n",
		total_time,
		total_movement,
		(float) total_processing_time / number_of_requests,
		(float) total_wait_time / number_of_requests,
		max_wait_time
	);
}

int _is_done()
{
	RequestNodePtr request_ptr = request_head;
	while (request_ptr != NULL)
	{
		if (request_ptr->completion_time <= 0)
			return 0;

		request_ptr = request_ptr->next;
	}

	return 1;
}

void set_algorithm(char selected_alg)
{
	alg = selected_alg;
}

void set_debug(int activate)
{
	debug = activate;
}

ProcessNodePtr _select_node_fcfs()
{
	return process_head;
}

ProcessNodePtr _select_node_sstf(int track)
{
	unsigned int least_tracks = ~0;
	ProcessNodePtr least_process = process_head;

	ProcessNodePtr process_ptr = process_head;
	while (process_ptr != NULL)
	{
		unsigned int distance = abs(process_ptr->node->track_number - track);
		if (distance < least_tracks)
		{
			least_tracks = distance;
			least_process = process_ptr;
		}

		process_ptr = process_ptr->next;
	}

	return least_process;
}

ProcessNodePtr _select_node(int count, int track)
{
	ProcessNodePtr selected_process = NULL;

	if (alg == 'f')
		selected_process = _select_node_fcfs();
	else if (alg == 's')
		selected_process = _select_node_sstf(track);

	if (selected_process != NULL)
	{
		selected_process->node->start_time = count;

		ProcessNodePtr process_ptr = process_head;
		ProcessNodePtr process_prev = NULL;
		while (process_ptr != NULL)
		{
			if (process_ptr	== selected_process)
				break;

			process_prev = process_ptr;
			process_ptr = process_ptr->next;
		}

		if (process_prev == NULL)
			process_head = process_head->next;
		else
			process_prev->next = process_ptr->next;

		if (debug)
		{
			printf(
				"%d:\t%d issue %d %d\n",
				count,
				selected_process->node->id,
				selected_process->node->track_number,
				track
			);
		}

		// If we're already here, don't bother to send it
		// back a second time. Just give it off here.
		if (selected_process->node->track_number == track)
		{
			selected_process->node->completion_time = count;

			if (debug)
			{
				printf(
					"%d:\t%d finish %d\n",
					count,
					selected_process->node->id,
					selected_process->node->completion_time - selected_process->node->arrival_time
				);
			}

			return _select_node(count, track);
		}
	}

	return selected_process;
}

void _create_process_node(RequestNodePtr request_node)
{
	ProcessNodePtr new_process = (ProcessNodePtr) malloc(sizeof(struct ProcessNode));
	new_process->node = request_node;
	new_process->next = NULL;

	if (process_head == NULL)
	{
		process_head = new_process;
	}
	else
	{
		ProcessNodePtr process_ptr = process_head;
		while (process_ptr->next != NULL)
		{
			process_ptr = process_ptr->next;
		}

		process_ptr->next = new_process;
	}
}

void _create_request_node(int id, int arrival, int track)
{
	RequestNodePtr new_request = (RequestNodePtr) malloc(sizeof(struct RequestNode));
	new_request->id = id;
	new_request->arrival_time = arrival;
	new_request->track_number = track;
	new_request->completion_time = 0;
	new_request->next = NULL;

	if (request_head == NULL)
	{
		request_head = new_request;
	}
	else
	{
		RequestNodePtr request_ptr = request_head;
		while (request_ptr->next != NULL)
		{
			request_ptr = request_ptr->next;
		}

		request_ptr->next = new_request;
	}
}

void _process_file(const char *file)
{
	// Proccess the file
	char *input = read_file(file);

	char *line_token;
	char *line_tokenizer;

	line_token = strtok_r(input, "\n", &line_tokenizer);
	int i = 0;
	while (line_token != NULL)
	{
		if (strstr(line_token, "#"))
		{
			line_token = strtok_r(NULL, "\n", &line_tokenizer);
			continue;
		}

		char *token;
		char *tokenizer;

		token = strtok_r(line_token, " \t", &tokenizer);
		int arrival = atoi(token);

		token = strtok_r(NULL, " \t", &tokenizer);
		int track_position = atoi(token);

		_create_request_node(i, arrival, track_position);

		line_token = strtok_r(NULL, "\n", &line_tokenizer);
		i++;
	}
}

void _add_new_processes(int count)
{
	RequestNodePtr request_ptr = request_head;
	while (request_ptr != NULL)
	{
		if (request_ptr->arrival_time == count)
		{
			if (debug)
			{
				printf(
					"%d:\t%d add %d\n",
					count,
					request_ptr->id,
					request_ptr->track_number
				);
			}
			_create_process_node(request_ptr);
		}

		request_ptr = request_ptr->next;
	}
}

void _process_requests()
{
	int count = 1;
	int curr_track_pos = 0;
	int track_movement = 0;

	if (debug)
	{
		printf("TRACE\n");
	}

	ProcessNodePtr curr_process = NULL;
	while (_is_done() == 0)
	{
		// See if we need to add in any new
		// processes to our stack.
		_add_new_processes(count);

		if (curr_process != NULL
			&& curr_track_pos == curr_process->node->track_number)
		{
			curr_process->node->completion_time = count;
			if (debug)
			{
				printf(
					"%d:\t%d finish %d\n",
					count,
					curr_process->node->id,
					curr_process->node->completion_time - curr_process->node->arrival_time
				);
			}

			curr_process = NULL;
		}

		if (curr_process == NULL)
		{
			curr_process = _select_node(count, curr_track_pos);
		}

		if (curr_process != NULL)
		{
			if (curr_track_pos > curr_process->node->track_number)
			{
				curr_track_pos--;
			}
			else
			{
				curr_track_pos++;
			}

			track_movement++;
		}

		count++;
	}

	total_time = count;
	total_movement = track_movement;
}

void process(const char *file)
{
	_process_file(file);
	_process_requests();

	if (debug)
	{
		_print_requests_table();
	}

	_print_summary();
}
