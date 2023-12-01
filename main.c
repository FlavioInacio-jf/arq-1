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
void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32);

// Principal function
int main(int argc, char *argv[])
{
  // Input file
  FILE *input = fopen(argv[1], "r");
  if (input == NULL)
  {
    perror("Failed to load input file");
    exit(EXIT_FAILURE);
  }

  // Output file
  const FILE *output = fopen(argv[2], "w");

  // 32 registers initialized to zero
  uint32_t registers[SIZE_MEMORY] = {0};

  // 32 KiB memory initialized to zero
  uint8_t *mem8 = (uint8_t *)(calloc(SIZE_MEMORY, 1024));
  uint32_t *mem32 = (uint32_t *)(calloc(SIZE_MEMORY, 1024));

  loadMemory(input, mem8, mem32);

  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32)
{

  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  char hexString[32];

  while (fgets(hexString, sizeof(hexString), input) != NULL)
  {
    unsigned int hexCode = strtoul(hexString, NULL, 16);
    mem32[count] = hexCode;

    printf("%d: 0x%08X\n", count, mem32[count]);
    count += 1;
  }

  fclose(input);
}
