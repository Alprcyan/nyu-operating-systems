#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

enum {COUNT_STATE, PARSE_STATE};
enum {DEFINITION_LINE, SYMBOLS_LINE, INSTRUCTION_LINE};
enum {DEFINITION_NAME, DEFINITION_VALUE};
enum {INSTRUCTION_TYPE, INSTRUCTION_VALUE};

typedef struct BlockNode
{
   int size;
   int mem_addr;
   struct SymbolNode *symbols;
   struct InstructionNode *instructions;
   struct BlockNode *next;
} BlockNode;
typedef struct BlockNode* BlockNodePtr;

typedef struct SymbolNode
{
   char *name;
   int val;
   struct SymbolNode *next;
} SymbolNode;
typedef struct SymbolNode* SymbolNodePtr;

typedef struct InstructionNode
{
   char *instruction;
   int val;
   struct InstructionNode *next;
} InstructionNode;
typedef struct InstructionNode* InstructionNodePtr;

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

void interpret_symbols(SymbolNodePtr head_symbol)
{
   SymbolNodePtr curr = head_symbol;
   while (curr != NULL)
   {
      printf("%s = %i\n", curr->name, curr->val);
      curr = curr->next;
   }
}

void interpret_blocks(BlockNodePtr head_block, SymbolNodePtr head_symbol)
{
   BlockNodePtr curr_block = head_block;
   while (curr_block != NULL)
   {
      InstructionNodePtr curr_instruction = curr_block->instructions;
      while (curr_instruction != NULL)
      {
         int new_mem_addr = curr_instruction->val;
         if (strcmp(curr_instruction->instruction, "R") == 0)
         {
            new_mem_addr += curr_block->mem_addr;
         }
         else if (strcmp(curr_instruction->instruction, "E") == 0)
         {
            SymbolNodePtr curr_symbol = curr_block->symbols;

            int c = 0;
            while (c < new_mem_addr)
            {
               if (curr_symbol->next == NULL)
                  curr_symbol = curr_block->symbols;
               else
                  curr_symbol = curr_symbol->next;

               c++;
            }
            char *symbol = curr_symbol->name;

            curr_symbol = head_symbol;
            while(strcmp(symbol, curr_symbol->name))
            {
               curr_symbol = curr_symbol->next;
            }

            new_mem_addr += curr_symbol->val;
         }
         printf("%i\n", new_mem_addr);

         curr_instruction = curr_instruction->next;
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
            curr->val = atoi(word);

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

SymbolNodePtr parse_definitions(char *input, SymbolNodePtr head, int mem_addr)
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
      if (c != 0)
      {
         if (curr_word_type == DEFINITION_NAME)
         {
            SymbolNodePtr new = (SymbolNodePtr) malloc(sizeof(struct SymbolNode));
            new->next = NULL;
            if (curr != NULL)
               curr->next = new;

            curr = new;

            if (head == NULL)
            {
               head = curr;
            }

            curr->name = strdup(word);

            curr_word_type = DEFINITION_VALUE;
         }
         else if (curr_word_type == DEFINITION_VALUE)
         {
            curr->val = atoi(word) + mem_addr;

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

void parse_file(const char *input_file, const char *output_file)
{
   char *contents = read_file(input_file);

   char *word;
   char *word_tokenizer;

   BlockNodePtr head_block = NULL;
   SymbolNodePtr head_symbol = NULL;

   int current_line_type = INSTRUCTION_LINE;
   int word_state = COUNT_STATE;
   BlockNodePtr curr_block = NULL;
   word = strtok_r(contents, " \n\t", &word_tokenizer);
   int curr_mem_addr = 0;
   int def_count = 0;
   char *def_count_string = NULL;
   while (word != NULL)
   {
      if (word_state == COUNT_STATE)
      {
         def_count = atoi(word);
         def_count_string = strdup(word);

         word_state = PARSE_STATE;
         if (current_line_type == DEFINITION_LINE)
         {
            current_line_type = SYMBOLS_LINE;
         }
         else if (current_line_type == SYMBOLS_LINE)
         {
            current_line_type = INSTRUCTION_LINE;
         }
         else if (current_line_type == INSTRUCTION_LINE)
         {
            current_line_type = DEFINITION_LINE;
         }

         word = strtok_r(NULL, " \n\t", &word_tokenizer);
      }
      else if (word_state == PARSE_STATE)
      {
         if (current_line_type == DEFINITION_LINE)
         {
            BlockNodePtr new_block = (BlockNodePtr) malloc(sizeof(struct BlockNode));
            new_block->next = NULL;

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

            int i = 0;
            int c = DEFINITION_NAME;
            char *line = combine(def_count_string, " ");
            while (word != NULL && i < def_count)
            {
               if (c == DEFINITION_NAME)
               {

                  c = DEFINITION_VALUE;
               }
               else if (c == DEFINITION_VALUE)
               {

                  c = DEFINITION_NAME;
                  i++;
               }

               line = combine(line, word);
               if (i != def_count)
                  line = combine(line, " ");

               word = strtok_r(NULL, " \n\t", &word_tokenizer);
            }

            printf("%s\n", line);
            head_symbol = parse_definitions(line, head_symbol, curr_mem_addr);
         }
         else if (current_line_type == SYMBOLS_LINE)
         {
            int i = 0;
            char *line = combine(def_count_string, " ");
            while (word != NULL && i < def_count)
            {
               i++;

               line = combine(line, word);
               if (i != def_count)
                  line = combine(line, " ");

               word = strtok_r(NULL, " \n\t", &word_tokenizer);
            }

            printf("%s\n", line);
            curr_block->symbols = parse_symbols(line);
         }
         else if (current_line_type == INSTRUCTION_LINE)
         {
            int i = 0;
            int c = INSTRUCTION_TYPE;
            char *line = combine(def_count_string, " ");
            while (word != NULL && i < def_count)
            {
               if (c == INSTRUCTION_TYPE)
               {

                  c = INSTRUCTION_VALUE;
               }
               else if (c == INSTRUCTION_VALUE)
               {

                  c = INSTRUCTION_TYPE;
                  i++;
               }

               line = combine(line, word);
               if (i != def_count)
                  line = combine(line, " ");

               word = strtok_r(NULL, " \n\t", &word_tokenizer);
            }

            printf("%s\n", line);
            curr_block->instructions = parse_instructions(line);
            curr_block->size = number_of_instructions(curr_block->instructions);
         }

         word_state = COUNT_STATE;
      }
   }

   printf("\nSYMBOL TABLE\n");
   interpret_symbols(head_symbol);

   printf("\nMEMORY MAP\n");
   interpret_blocks(head_block, head_symbol);
}
