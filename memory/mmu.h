#ifndef MMU_H
#define MMU_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uthash.h"
#include "randnum.h"
#include "readfile.h"

void show_instruction_process();
void show_memory_table();
void show_frame_table();
void show_summary();

void show_frame_table_after_instruction();

void choose_algorithm(char algorithm);

void create_frames(int count);

void process(const char *file);

#endif
