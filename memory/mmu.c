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
char curr_algorithm = NULL;

MemoryNodePtr _create_memory(int id)
{
	MemoryNodePtr new_memory = (MemoryNodePtr) malloc(sizeof(struct MemoryNode));
	new_memory->id = id;
	new_memory->referenced = 0;
	new_memory->modified = 0;
	new_memory->swapped = 0;

	HASH_ADD_INT(memory_nodes, id, new_memory);
	return new_memory;
}

MemoryNodePtr _memory_exists(int id)
{
	MemoryNodePtr frame_ptr;
	HASH_FIND_INT(memory_nodes, &id, frame_ptr);
	return frame_ptr;
}

MemoryNodePtr _memory_in_frame(int id)
{
	FrameNodePtr frame_node_ptr = NULL;
	while (frame_node_ptr != NULL)
	{
		if (frame_node_ptr->node->id == id)
			return frame_node_ptr->node;

		frame_node_ptr = frame_node_ptr->next;
	}

	return NULL;
}

FrameNodePtr _choose_frame()
{
	FrameNodePtr frame_node_ptr = NULL;
	while (frame_node_ptr != NULL)
	{
		if (frame_node_ptr->node == NULL)
			return frame_node_ptr;
	}

	// Figure it out.
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

	char *token;
	char *tokenizer;

	int count = 1;
	token = strtok_r(input, " \n\t", &tokenizer);
	while (token != NULL)
	{
		int access_type = atoi(token);
		token = strtok_r(NULL, " \n\t", &tokenizer);

		int id = atoi(token);
		token = strtok_r(NULL, " \n\t", &tokenizer);

		printf("==> inst:\t%d\t%d\n", access_type, id);

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
				printf("%d: UNMAP\t%d\t%d\n", (count-1), deref_mem->id, frame_ptr->id);

				// See if swapped bit is set.
				// If not, add it to the memory. SAY "OUT"
				if (deref_mem->swapped == 0)
				{
					// Set the swapped bit.
					deref_mem->swapped = 1;
					printf("%d: OUT\t%d\t%d\n", (count-1), deref_mem->id, frame_ptr->id);
				}

				// Set the bits.
				deref_mem->referenced = 0;
				deref_mem->modified = 0;
			}

			// Let us see if we already have the memory
			memory_ptr = _memory_exists(id);

			// If it is STILL null, we're going to have to create it.
			if (memory_ptr == NULL)
			{
				// Say "ZERO"
				printf("%d: ZERO\t\t%d\n", (count-1), frame_ptr->id);
				memory_ptr = _create_memory(id);
			}
			else
			{
				// Else we say "IN"
				printf("%d: IN\t%d\t%d\n", (count-1), memory_ptr->id, frame_ptr->id);
			}

			// And we assign it into the memory_ptr
			frame_ptr->node = memory_ptr;
			printf("%d: MAP\t%d\t%d\n", (count-1), memory_ptr->id, frame_ptr->id);
		}

		// And set the bits compared to access_type
		memory_ptr->referenced = count;
		if (access_type == 1)
			memory_ptr->modified = count;

		// Increment instruction
		count++;
	}
}