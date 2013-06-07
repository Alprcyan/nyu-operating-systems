#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readfile.h"

enum {DEFINITION_LINE, SYMBOLS_LINE, INSTRUCTION_LINE};
enum {DEFINITION_NAME, DEFINITION_VALUE};
enum {INSTRUCTION_TYPE, INSTRUCTION_VALUE};

typedef struct BlockNode
{
   int size;
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

InstructionNodePtr parse_instructions(char *input)
{
   char *word;
   char *word_tokenizer;

   InstructionNodePtr head, curr;

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

   SymbolNodePtr head, curr;

   word = strtok_r(input, " ", &word_tokenizer);
   int c = 0;
   while (word != NULL)
   {
      if (c != 0)
      {
         SymbolNodePtr new = (SymbolNodePtr) malloc(sizeof(struct SymbolNode));
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

SymbolNodePtr parse_definitions(char *input, SymbolNodePtr head)
{
   char *word;
   char *word_tokenizer;

   SymbolNodePtr curr = head;
   while(curr != NULL)
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
            if (curr != NULL)
               curr->next = new;

            curr = new;

            if (head == NULL)
               head = curr;

            new->name = strdup(word);

            curr_word_type = DEFINITION_VALUE;
         }
         else if (curr_word_type == DEFINITION_VALUE)
         {
            curr->val = atoi(word);

            curr_word_type = DEFINITION_NAME;
         }
      }

      c++;
      word = strtok_r(NULL, " ", &word_tokenizer);
   }

   return head;
}

void parse_file(const char *input_file, const char *output_file)
{
   char *contents = read_file(input_file);

   char *line;
   char *line_tokenizer;

   BlockNodePtr head_block = NULL;
   SymbolNodePtr head_symbol = NULL;

   int current_line_type = DEFINITION_LINE;
   BlockNodePtr curr_block = NULL;
   line = strtok_r(contents, "\n", &line_tokenizer);
   while (line != NULL)
   {
      printf("Parsing Line: %s\n", line);

      if (current_line_type == DEFINITION_LINE)
      {
         printf("\tParsing definition.\n");

         BlockNodePtr new_block = (BlockNodePtr) malloc(sizeof(struct BlockNode));

         if (curr_block != NULL)
         {
            curr_block->next = new_block;
         }

         curr_block = new_block;

         if (head_block == NULL)
         {
            head_block = curr_block;
         }

         head_symbol = parse_definitions(line, head_symbol);

         current_line_type = SYMBOLS_LINE;
      }
      else if (current_line_type == SYMBOLS_LINE)
      {
         printf("\tParsing symbols.\n");

         curr_block->symbols = parse_symbols(line);

         current_line_type = INSTRUCTION_LINE;
      }
      else if (current_line_type == INSTRUCTION_LINE)
      {
         printf("\tParsing instructions.\n");

         curr_block->instructions = parse_instructions(line);
         curr_block->size = number_of_instructions(curr_block->instructions);

         current_line_type = DEFINITION_LINE;
      }

      line = strtok_r(NULL, "\n", &line_tokenizer);
   }
}
