#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uthash.h"
#include "randnum.h"
#include "readfile.h"

struct MemoryNode
{
	int id;
	int referenced;
	int modified;
	int swapped;
	int used;
	UT_hash_handle hh; // This is the hash handler.
} MemoryNode;
typedef struct MemoryNode* MemoryNodePtr;

struct FrameNode
{
	int id;
	struct MemoryNode* node;
	struct FrameNode* next;
} FrameNode;
typedef struct FrameNode* FrameNodePtr;

FrameNodePtr frame_head = NULL;
MemoryNodePtr memory_nodes = NULL;
char curr_algorithm = 0;

int inst = 0;
int unmaps = 0;
int maps = 0;
int ins = 0;
int outs = 0;
int zeros = 0;

int print_process = 0;
int print_memory_table = 0;
int print_frame_table = 0;
int print_summary = 0;

int print_frame_table_after_instruction = 0;

void _print_summary()
{
	long long unsigned int cost = 0;
	cost += (3000*(ins+outs));
	cost += (400*(maps+unmaps));
	cost += (150*zeros);
	cost += (inst);
	printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
		inst, unmaps, maps, ins, outs, zeros, cost);
}

void _print_frames()
{
	FrameNodePtr frame_ptr = frame_head;
	while (frame_ptr != NULL)
	{
		if (frame_ptr->node == NULL)
			printf("* ");
		else
			printf("%d ", frame_ptr->node->id);

		frame_ptr = frame_ptr->next;
	}
	printf("\n");
}

MemoryNodePtr _create_memory(int id)
{
	MemoryNodePtr new_memory = (MemoryNodePtr) malloc(sizeof(struct MemoryNode));
	new_memory->id = id;
	new_memory->referenced = 0;
	new_memory->modified = 0;
	new_memory->swapped = 0;
	new_memory->used = 0;

	HASH_ADD_INT(memory_nodes, id, new_memory);
	return new_memory;
}

MemoryNodePtr _memory_exists(int id)
{
	MemoryNodePtr frame_ptr;
	HASH_FIND_INT(memory_nodes, &id, frame_ptr);
	return frame_ptr;
}

void _print_memory()
{
	int i = 0;
	for (i = 0; i < 64; i++)
	{
		MemoryNodePtr mem_ptr = _memory_exists(i);

		if (mem_ptr == NULL)
		{
			printf("* ");
			continue;
		}

		if (mem_ptr->referenced == 0)
		{
			printf("# ");
			continue;
		}

		printf("%d:", i);

		if (mem_ptr->referenced)
			printf("R");
		else
			printf("-");

		if (mem_ptr->modified)
			printf("M");
		else
			printf("-");

		if (mem_ptr->swapped)
			printf("S");
		else
			printf("-");

		printf(" ");
	}
	printf("\n");
}

MemoryNodePtr _memory_in_frame(int id)
{
	FrameNodePtr frame_node_ptr = frame_head;
	while (frame_node_ptr != NULL)
	{

		if (frame_node_ptr->node != NULL)
			if (frame_node_ptr->node->id == id)
				return frame_node_ptr->node;

		frame_node_ptr = frame_node_ptr->next;
	}

	return NULL;
}

FrameNodePtr _choose_frame_lru()
{
	FrameNodePtr frame_node_ptr = frame_head;
	FrameNodePtr least_frame_ptr = NULL;
	int referenced = 9999999;
	int least = 9999999;

	while (frame_node_ptr != NULL)
	{
		// printf("%d-%d\n", frame_node_ptr->node->used, frame_node_ptr->node->referenced);
		// int used_less = (frame_node_ptr->node->used < least);
		// int used_equally = (frame_node_ptr->node->used == least);
		int referenced_earlier = (frame_node_ptr->node->referenced < referenced);
		if (referenced_earlier)
		{
			least_frame_ptr = frame_node_ptr;
			least = least_frame_ptr->node->used;
			referenced = least_frame_ptr->node->referenced;
		}

		frame_node_ptr = frame_node_ptr->next;
	}

	return least_frame_ptr;
}

FrameNodePtr _choose_frame()
{
	FrameNodePtr frame_node_ptr = frame_head;
	while (frame_node_ptr != NULL)
	{
		if (frame_node_ptr->node == NULL)
			return frame_node_ptr;

		frame_node_ptr = frame_node_ptr->next;
	}

	// if (curr_algorithm == 'l')
		return _choose_frame_lru();
}

void show_frame_table_after_instruction()
{
	print_frame_table_after_instruction = 1;
}

void show_instruction_process()
{
	print_process = 1;
}

void show_memory_table()
{
	print_memory_table = 1;
}

void show_frame_table()
{
	print_frame_table = 1;
}

void show_summary()
{
	print_summary = 1;
}

void choose_algorithm(char algorithm)
{
	curr_algorithm = algorithm;
}

void create_frames(int count)
{
	int i = 0;
	for (i = 0; i < count; i++)
	{
		FrameNodePtr new_frame = (FrameNodePtr) malloc(sizeof(struct FrameNode));
		new_frame->id = count - (i+1);
		new_frame->node = NULL;
		new_frame->next = NULL;

		if (frame_head != NULL)
			new_frame->next = frame_head;

		frame_head = new_frame;
	}
}

void process(const char *file)
{
	char *input = read_file(file);

	char *line_token;
	char *line_tokenizer;

	int count = 1;
	line_token = strtok_r(input, "\n", &line_tokenizer);
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
		int access_type = atoi(token);

		token = strtok_r(NULL, " \t", &tokenizer);
		int id = atoi(token);

		line_token = strtok_r(NULL, "\n", &line_tokenizer);

		if (print_process)
			printf("==> inst: %d %d\n", access_type, id);

		// Lets see if we have a reference
		// of the node already.
		MemoryNodePtr memory_ptr = _memory_in_frame(id);
		if (memory_ptr == NULL)
		{
			FrameNodePtr frame_ptr = _choose_frame();
			if (frame_ptr->node != NULL)
			{
				// Dereference the node. SAY "UNMAP"
				MemoryNodePtr deref_mem = frame_ptr->node;
				frame_ptr->node = NULL;

				if (print_process)
				{
					printf("%d: UNMAP\t%d\t%d\n", (count-1), deref_mem->id, frame_ptr->id);
					unmaps++;
				}

				// See if we were modified
				// If so, write it to the memory. SAY "OUT"
				if (deref_mem->modified > 0)
				{
					// Set the swapped bit.
					deref_mem->swapped = count;

					if (print_process)
					{
						printf("%d: OUT\t\t%d\t%d\n", (count-1), deref_mem->id, frame_ptr->id);
						outs++;
					}
				}

				// Set the bits.
				deref_mem->referenced = 0;
				deref_mem->modified = 0;
				deref_mem->used = 0;
			}

			// Let us see if we already have the memory
			memory_ptr = _memory_exists(id);

			// If it is STILL null, we're going to have to create it.
			if (memory_ptr == NULL || memory_ptr->swapped == 0)
			{
				if (memory_ptr == NULL)
					memory_ptr = _create_memory(id);

				// Say "ZERO"
				if (print_process)
				{
					printf("%d: ZERO\t\t%d\n", (count-1), frame_ptr->id);
					zeros++;
				}
			}
			else
			{
				// Else we say "IN"
				if (print_process)
				{
					printf("%d: IN\t\t%d\t%d\n", (count-1), memory_ptr->id, frame_ptr->id);
					ins++;
				}
			}

			// And we assign it into the memory_ptr
			frame_ptr->node = memory_ptr;

			if (print_process)
			{
				maps++;
				printf("%d: MAP\t\t%d\t%d\n", (count-1), memory_ptr->id, frame_ptr->id);
			}
		}

		// And set the bits compared to access_type
		memory_ptr->referenced = count;
		memory_ptr->used = memory_ptr->used + 1;
		if (access_type == 1)
			memory_ptr->modified = count;

		if (print_frame_table_after_instruction)
			_print_frames();

		// Increment instruction
		count++;
	}

	if (print_memory_table)
		_print_memory();

	if (print_frame_table)
		_print_frames();

	inst = count-1;

	if (print_summary)
		_print_summary();
}