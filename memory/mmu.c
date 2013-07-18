#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uthash.h"
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
	int pos;
	struct MemoryNode* node;
	struct FrameNode* next;
} FrameNode;
typedef struct FrameNode* FrameNodePtr;

FrameNodePtr frame_head = NULL;
MemoryNodePtr memory_nodes = NULL;

void create_nodes(int count)
{
	int i = 0;
	for (i = 0; i < count; i++)
	{
		FrameNodePtr new_frame = (FrameNodePtr) malloc(sizeof(struct FrameNode));
		new_frame->pos = count - (i+1);
		new_frame->node = NULL;
		new_frame->next = NULL;

		if (frame_head != NULL)
			new_frame->next = frame_head;

		frame_head = new_frame;
	}
}

MemoryNodePtr create_memory(int frame)
{
	MemoryNodePtr new_memory = (MemoryNodePtr) malloc(sizeof(struct MemoryNode));
	new_memory->id = frame;
	new_memory->referenced = 0;
	new_memory->modified = 0;
	new_memory->swapped = 0;

	// HASH_ADD_INT(memory_nodes, id, new_memory);
	return new_memory;
}

MemoryNodePtr memory_exists(int frame)
{
	MemoryNodePtr frame_ptr;
	HASH_FIND_INT(memory_nodes, &frame, frame_ptr);
	return frame_ptr;
}

MemoryNodePtr frame_exists(int frame)
{
	FrameNodePtr frame_node_ptr = NULL;
	while (frame_node_ptr != NULL)
	{
		if (frame_node_ptr->node->id == frame)
			return frame_node_ptr->node;

		frame_node_ptr = frame_node_ptr->next;
	}

	return NULL;
}

FrameNodePtr choose_frame()
{
	FrameNodePtr frame_node_ptr = NULL;
	while (frame_node_ptr != NULL)
	{
		if (frame_node_ptr->node == NULL)
			return frame_node_ptr;
	}

	// Figure it out.
}

void process(const char *file)
{
	char *input = read_file(file);

	char *token;
	char *tokenizer;

	int count = 0;
	token = strtok_r(input, " \n\t", &tokenizer);
	while (token != NULL)
	{
		int access_type = atoi(token);
		token = strtok_r(NULL, " \n\t", &tokenizer);

		int frame = atoi(token);
		token = strtok_r(NULL, " \n\t", &tokenizer);

		// Lets see if we have a reference
		// of the node already.
		MemoryNodePtr memory_ptr = frame_exists(frame);
		if (memory_ptr != NULL)
		{
			memory_ptr->referenced = count;
			continue;
		}

		frame_ptr = choose_frame();
		if (frame_ptr->node != NULL)
		{
			// Dereference the node. SAY "UNMAP"

			// See if swapped bit is set.

			// If not, add it to the memory. SAY "OUT"

			// Set the swapped bit.
		}

		// If we don't, lets see if we have it
		// stored somewhere else.
		memory_ptr = memory_exists(frame);

		// If it is STILL null, we're going to have to create it.
		if (memory_ptr == NULL)
		{
			// Say "ZERO"
			memory_ptr = create_memory(frame);
		}
		else
		{
			// Else we say "IN"
		}

		// And we assign it into the memory_ptr

		// And set the bits compared to access_type

		count++;
	}
}