#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

enum {COUNT_STATE, PARSE_STATE};
enum {NEW_LINE, DEFINITION_LINE, SYMBOLS_LINE, INSTRUCTION_LINE, END_OF_LINE};
enum {DEFINITION_NAME, DEFINITION_VALUE};
enum {INSTRUCTION_TYPE, INSTRUCTION_VALUE};

typedef struct BlockNode
{
   int size;
   int mem_addr;
   struct SymbolNode *symbols;
   struct InstructionNode *instructions;
   struct BlockNode *next;
   int num;
} BlockNode;
typedef struct BlockNode* BlockNodePtr;

typedef struct SymbolNode
{
   char *name;
   int val;
   struct SymbolNode *next;
   int used;
   int block_num;
   int retried;
   int too_big;
   int too_big_size;
   int too_big_val;
} SymbolNode;
typedef struct SymbolNode* SymbolNodePtr;

typedef struct InstructionNode
{
   char *instruction;
   int upperVal;
   int lowerVal;
   struct InstructionNode *next;
} InstructionNode;
typedef struct InstructionNode* InstructionNodePtr;

void __parseerror(int linenum, int lineoffset, int errcode)
{
   static char* errstr[] = {
      "NUM_EXPECTED",
      "SYM_EXPECTED",
      "ADDR_EXPECTED",
      "SYM_TOLONG",
      "TO_MANY_DEF_IN_MODULE",
      "TO_MANY_USE_IN_MODULE",
      "TO_MANY_SYMBOLS",
      "TO_MANY_INSTR"
   };
   printf("Parse Error line_%i offset %i: %s\n", linenum, lineoffset, errstr[errcode]);
}

int number_of_instructions(InstructionNodePtr head)
{
   InstructionNodePtr curr = head;
   int c = 0;
   while (curr != NULL)
   {
      c++;
      curr = curr->next;
   }
   return c;
}

void unsed_symbols (SymbolNodePtr head_symbol)
{
   SymbolNodePtr curr = head_symbol;
   while (curr != NULL)
   {
      if (curr->used == 0)
         printf("Warning: %s was defined in module %i but never used\n", curr->name, curr->block_num);

      curr = curr->next;
   }
}

void interpret_symbols(SymbolNodePtr head_symbol)
{
   SymbolNodePtr curr = head_symbol;
   while (curr != NULL)
   {
      printf("%s=%i", curr->name, curr->val);
      if (curr->retried == 1)
         printf(" Error: This variable is multiple times defined; first value used");
      // if (curr->too_big == 1)
         // printf(" Warning: Module %i: %s to big %i (max=%i) assume zero relative", curr->block_num, curr->name, curr->too_big_val, curr->too_big_size);
      printf("\n");
      curr = curr->next;
   }
}

void interpret_blocks(BlockNodePtr head_block, SymbolNodePtr head_symbol)
{
   BlockNodePtr curr_block = head_block;
   int count = 0;
   while (curr_block != NULL)
   {
      InstructionNodePtr curr_instruction = curr_block->instructions;
      while (curr_instruction != NULL)
      {
         char *error = (char *) malloc(512);
         int upper_addr = curr_instruction->upperVal;
         int new_mem_addr = curr_instruction->lowerVal;
         if (strcmp(curr_instruction->instruction, "E") == 0)
         {
            SymbolNodePtr curr_symbol = curr_block->symbols;

            int c = 0;
            while (curr_symbol != NULL
               && c < new_mem_addr)
            {
               curr_symbol = curr_symbol->next;
               c++;
            }

            char *symbol = NULL;
            if (curr_symbol != NULL)
            {
               symbol = curr_symbol->name;
               curr_symbol->used = 1;
            }
            else
            {
               c--;
            }

            if (symbol != NULL)
               curr_symbol = head_symbol;

            while(curr_symbol != NULL
               && symbol != NULL
               && strcmp(symbol, curr_symbol->name))
            {
               curr_symbol = curr_symbol->next;
            }

            int val = 0;
            if (curr_symbol != NULL)
            {
               val = curr_symbol->val;
               curr_symbol->used = 1;
            }
            else
            {
               if (c < new_mem_addr)
                  sprintf(error, "Error: External address exceeds length of uselist; treated as immediate");
               else
                  sprintf(error, "Error: %s is not defined; zero used", symbol);
            }

            if (c >= new_mem_addr)
               new_mem_addr -= new_mem_addr;

            new_mem_addr += val;
         }
         else if (strcmp(curr_instruction->instruction, "A") == 0)
         {
            if (new_mem_addr > 512)
            {
               sprintf(error, "Error: Absolute address exceeds machine size; zero used");
               new_mem_addr = 0;
            }
         }
         else if (strcmp(curr_instruction->instruction, "R") == 0)
         {
            if (new_mem_addr >= curr_block->size)
            {
               sprintf(error, "Error: Relative address exceeds module size; zero used");
               new_mem_addr = 0 + curr_block->mem_addr;
            }
            else
            {
               new_mem_addr += curr_block->mem_addr;
            }
         }

         printf("%03d: %i%03i %s\n", count, upper_addr, new_mem_addr, error);
         count++;

         curr_instruction = curr_instruction->next;
      }

      SymbolNodePtr curr_symbol = curr_block->symbols;
      while (curr_symbol != NULL)
      {
         if (curr_symbol->used == 0)
            printf("Warning: In Module %i %s appeared in the uselist but was not actually used\n", curr_block->num, curr_symbol->name);

         curr_symbol = curr_symbol->next;
      }

      curr_block = curr_block->next;
   }
}

InstructionNodePtr parse_instructions(char *input)
{
   char *word;
   char *word_tokenizer;

   InstructionNodePtr head = NULL, curr = NULL;

   int curr_word_type = INSTRUCTION_TYPE;
   word = strtok_r(input, " ", &word_tokenizer);
   int c = 0;
   while (word != NULL)
   {
      if (c != 0)
      {
         if (curr_word_type == INSTRUCTION_TYPE)
         {
            InstructionNodePtr new = (InstructionNodePtr) malloc(sizeof(struct InstructionNode));
            new->next = NULL;
            if (curr != NULL)
               curr->next = new;

            curr = new;

            if (head == NULL)
               head = curr;

            new->instruction = strdup(word);

            curr_word_type = INSTRUCTION_VALUE;
         }
         else if (curr_word_type == INSTRUCTION_VALUE)
         {
            curr->upperVal = atoi(word)/1000;
            curr->lowerVal = atoi(word)%1000;

            curr_word_type = INSTRUCTION_TYPE;
         }
      }

      c++;
      word = strtok_r(NULL, " ", &word_tokenizer);
   }

   return head;
}

SymbolNodePtr parse_symbols(char *input)
{
   char *word;
   char *word_tokenizer;

   SymbolNodePtr head = NULL, curr = NULL;

   word = strtok_r(input, " ", &word_tokenizer);
   int c = 0;
   while (word != NULL)
   {
      if (c != 0)
      {
         SymbolNodePtr new = (SymbolNodePtr) malloc(sizeof(struct SymbolNode));
         new->next = NULL;
         new->used = 0;
         if (curr != NULL)
            curr->next = new;

         curr = new;

         if (head == NULL)
            head = curr;

         new->name = strdup(word);
      }

      c++;
      word = strtok_r(NULL, " ", &word_tokenizer);
   }

   return head;
}

SymbolNodePtr parse_definitions(char *input, SymbolNodePtr head, int mem_addr, int block_num, int size)
{
   char *word;
   char *word_tokenizer;

   SymbolNodePtr curr = head;
   while(curr != NULL && curr->next != NULL)
   {
      curr = curr->next;
   }

   int curr_word_type = DEFINITION_NAME;
   word = strtok_r(input, " ", &word_tokenizer);
   int c = 0;
   while (word != NULL)
   {
      word = strdup(word);
      if (c != 0)
      {
         if (curr_word_type == DEFINITION_NAME)
         {
            SymbolNodePtr check_ptr = head;
            while (check_ptr != NULL
               && strcmp(word, check_ptr->name) != 0
            )
            {
               check_ptr = check_ptr->next;
            }

            if (check_ptr)
            {
               check_ptr->retried = 1;
               c++;
               word = strtok_r(input, " ", &word_tokenizer);
            }
            else
            {
               SymbolNodePtr new = (SymbolNodePtr) malloc(sizeof(struct SymbolNode));
               new->next = NULL;
               new->used = 0;
               new->block_num = block_num;
               new->retried = 0;
               new->too_big = 0;
               new->too_big_size = 0;
               if (curr != NULL)
                  curr->next = new;

               curr = new;

               if (head == NULL)
               {
                  head = curr;
               }

               curr->name = word;

               curr_word_type = DEFINITION_VALUE;
            }
         }
         else if (curr_word_type == DEFINITION_VALUE)
         {
            int new_val = atoi(word);
            if (new_val >= size)
            {
               printf("Warning: Module %i: %s to big %i (max=%i) assume zero relative\n", curr->block_num, curr->name, new_val, size-1);

               curr->too_big_val = new_val;
               new_val = 0;
               curr->too_big = 1;
               curr->too_big_size = size;
            }

            curr->val = new_val + mem_addr;

            curr_word_type = DEFINITION_NAME;
         }
      }

      c++;
      word = strtok_r(NULL, " ", &word_tokenizer);
   }

   return head;
}

char *combine(char *a, char *b)
{
   if (a == NULL)
   {
      return strdup(b);
   }

   int size = strlen(a) + strlen(b) + 1;
   char *new = (char *) malloc(size);

   strcpy(new, a);
   strcat(new, b);

   return new;
}

void parse_file(const char *input_file)
{
   char *contents = read_file(input_file);

   char *file;
   char *file_tokenizer;

   char *word;
   char *word_tokenizer;

   BlockNodePtr head_block = NULL;
   SymbolNodePtr head_symbol = NULL;

   BlockNodePtr curr_block = NULL;
   int curr_line_type = 999;
   int curr_sub_type = 999;
   char* new_line;
   char* new_def_line;
   char* new_sym_line;
   int curr_line = 1;
   int curr_pos = 1;
   int curr_count = 0;
   int max_count = 0;
   char *max_count_str = NULL;
   char *new_word;
   int block_num = 1;
   int curr_mem_addr = 0;

   file = strtok_r(contents, "\n", &file_tokenizer);
   while (file != NULL)
   {
      curr_pos = 1;
      word = strtok_r(file, " \t", &word_tokenizer);
      while (word != NULL)
      {
         new_word = strdup(word);
         if (curr_count >= max_count)
         {
            curr_count = 0;
            max_count = atoi(new_word);
            max_count_str = new_word;
            // printf("%s\n", new_line);

            if (curr_line_type == DEFINITION_LINE)
            {
               new_def_line = new_line;
            }
            else if (curr_line_type == SYMBOLS_LINE)
            {
               new_sym_line = new_line;
            }
            else if (curr_line_type == INSTRUCTION_LINE)
            {
               curr_block->instructions = parse_instructions(new_line);
               curr_block->size = number_of_instructions(curr_block->instructions);
               curr_block->symbols = parse_symbols(new_sym_line);
               head_symbol = parse_definitions(new_def_line, head_symbol, curr_mem_addr, curr_block->num, curr_block->size);
            }

            new_line = combine(word, " ");
            curr_line_type++;

            if (curr_line_type >= END_OF_LINE)
            {
               curr_line_type = DEFINITION_LINE;

               BlockNodePtr new_block = (BlockNodePtr) malloc(sizeof(struct BlockNode));
               new_block->next = NULL;
               new_block->num = block_num;
               block_num++;

               if (curr_block != NULL)
               {
                  curr_block->next = new_block;
                  curr_mem_addr += curr_block->size;
               }

               curr_block = new_block;
               curr_block->mem_addr = curr_mem_addr;

               if (head_block == NULL)
               {
                  head_block = curr_block;
               }
            }
         }
         else
         {
            if (curr_line_type == DEFINITION_LINE)
            {
               if ((curr_count == 0 && curr_sub_type != DEFINITION_VALUE)
                  || curr_sub_type == DEFINITION_VALUE+1
               )
                  curr_sub_type = DEFINITION_NAME;

               if (curr_sub_type == DEFINITION_NAME)
               {

               }
               else if (curr_sub_type == DEFINITION_VALUE)
               {

                  curr_count++;
               }

               curr_sub_type++;
            }
            else if (curr_line_type == SYMBOLS_LINE)
            {
               curr_count++;
            }
            else if (curr_line_type == INSTRUCTION_LINE)
            {
               if ((curr_count == 0 && curr_sub_type != INSTRUCTION_VALUE)
                  || curr_sub_type == INSTRUCTION_VALUE+1
               )
                  curr_sub_type = INSTRUCTION_TYPE;

               if (curr_sub_type == INSTRUCTION_TYPE)
               {

               }
               else if (curr_sub_type == INSTRUCTION_VALUE)
               {

                  curr_count++;
               }

               curr_sub_type++;
            }

            new_line = combine(new_line, new_word);
            if (curr_count != max_count)
               new_line = combine(new_line, " ");
         }

         curr_pos = curr_pos + strlen(word) + 1;
         word = strtok_r(NULL, " \t", &word_tokenizer);
      }

      curr_line++;
      file = strtok_r(NULL, "\n", &file_tokenizer);
   }

   curr_block->instructions = parse_instructions(new_line);
   curr_block->size = number_of_instructions(curr_block->instructions);
   curr_block->symbols = parse_symbols(new_sym_line);
   head_symbol = parse_definitions(new_def_line, head_symbol, curr_mem_addr, curr_block->num, curr_block->size);

   printf("Symbol Table\n");
   interpret_symbols(head_symbol);

   printf("\nMemory Map\n");
   interpret_blocks(head_block, head_symbol);

   printf("\n");
   unsed_symbols(head_symbol);
}
