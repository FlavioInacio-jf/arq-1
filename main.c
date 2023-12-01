/******************************************************
 * Libraries
 *******************************************************/

#include <stdint.h> // Integer types with specific widths.
#include <stdlib.h> // General-purpose functions, including memory allocation and random number generation.
#include <stdio.h>  // Standard Input/Output library for input and output operations.

/******************************************************
 * Utility Constants
 *******************************************************/

#define SIZE_MEMORY 32

/******************************************************
 * Functin Signature
 *******************************************************/

// Load memory vector from a file
void loadMemory(char *filename, uint8_t *mem8, uint8_t *mem32);

// Principal function
int main(int argc, char *argv[])
{

  const FILE *output = fopen(argv[2], "w");

  // 32 registers initialized to zero
  uint32_t registers[SIZE_MEMORY] = {0};

  // 32 KiB memory initialized to zero
  uint8_t *mem8 = (uint8_t *)(calloc(SIZE_MEMORY, 1024));
  uint32_t *mem32 = (uint32_t *)(calloc(SIZE_MEMORY, 1024));

  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void loadMemory(char *filename, uint8_t *mem8, uint8_t *mem32)
{
  FILE *input = fopen(filename, "r");
  if (input == NULL)
  {
    perror("Failed to load input file");
    exit(EXIT_FAILURE);
  }

  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  while (!feof(input))
  {
    if (fscanf(input, "0x%08X", &mem32[count]) == 1)
    {
      for (int i = count; i < count * 2; i++)
      {
        mem8[count] = mem32[count] >> (24 - (abs(i - count)) * 8);
      }
    }
    else
    {
      fprintf(stderr, "The instruction is not in hexadecimal");
    }

    count += 1;
  }

  fclose(input);
}
