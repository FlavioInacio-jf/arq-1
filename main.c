/******************************************************
 * Libraries
 *******************************************************/

#include <stdint.h> // Integer types with specific widths.
#include <stdlib.h> // General-purpose functions, including memory allocation and random number generation.
#include <stdio.h>  // Standard Input/Output library for input and output operations.
#include <string.h>

/******************************************************
 * Utility Constants
 *******************************************************/

#define NUM_REGISTERS 32

// Specific use register indexes
#define IR 28 // Instruction Register
#define PC 29 // Program Counter
#define SP 30 // Stack Pointer
#define SR 31 // Status Register

/******************************************************
 * Functin Signature
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32); // Load memory vector from a file
void decodeInstructions(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32, FILE *output);

void mov(uint32_t registers[NUM_REGISTERS], char instruction[30]);

void bun(uint32_t registers[NUM_REGISTERS], FILE *output);
void interger(uint32_t registers[NUM_REGISTERS], uint8_t *executa, FILE *output);

void l8(uint32_t registers[NUM_REGISTERS], char instruction[30], uint8_t *mem8, uint32_t *mem32);
void l32(uint32_t registers[NUM_REGISTERS], char instruction[30], uint8_t *mem8, uint32_t *mem32);

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
  FILE *output = fopen(argv[2], "w");
  if (output == NULL)
  {
    perror("Failed to load output file");
    exit(EXIT_FAILURE);
  }

  // 32 registers initialized to zero
  uint32_t registers[NUM_REGISTERS] = {0};

  // 32 KiB memory initialized to zero
  uint8_t *mem8 = (uint8_t *)(calloc(NUM_REGISTERS, 1024));
  uint32_t *mem32 = (uint32_t *)(calloc(NUM_REGISTERS, 1024));

  loadMemory(input, mem8, mem32);
  decodeInstructions(registers, mem8, mem32, output);

  fclose(input);
  fclose(output);
  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32)
{

  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  char hexString[NUM_REGISTERS];

  while (fgets(hexString, sizeof(hexString), input) != NULL)
  {
    uint32_t hexCode = strtoul(hexString, NULL, 16);
    mem32[count] = hexCode;

    mem8[count] = (hexCode & 0xFF000000) >> 24;
    mem8[count + 1] = (hexCode & 0x00FF0000) >> 16;
    mem8[count + 2] = (hexCode & 0x0000FF00) >> 8;
    mem8[count + 3] = (hexCode & 0x000000FF);

    count += 1;
  }
}

void decodeInstructions(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32, FILE *output)
{
  printf("\nMEM8:\n");
  for (uint8_t i = 0; i < 48; i = i + 4)
    printf("0x%08X: 0x%02X 0x%02X 0x%02X 0x%02X\n", i, mem8[i], mem8[i + 1], mem8[i + 2], mem8[i + 3]);

  printf("\nSaida esperada\n\n      |       \n      V       \n\n");

  // Output formatting to file
  fprintf(output, "[START OF SIMULATION]\n");

  printf("[START OF SIMULATION]\n");

  uint8_t executa = 1;
  while (executa)
  {
    char instrucao[30] = {0};
    uint32_t pc = 0, xyl = 0;

    registers[IR] = ((mem8[registers[PC] + 0] << 24) | (mem8[registers[PC] + 1] << 16) | (mem8[registers[PC] + 2] << 8) | (mem8[registers[PC] + 3] << 0)) | mem32[registers[PC] >> 2];

    uint8_t opcode = (registers[IR] >> 26) & 0x3F;
    switch (opcode)
    {

    case 0b000000: // mov
      mov(registers, instrucao);
      break;
    case 0b011000: // l8
      l8(registers, instrucao, mem8, mem32);
      break;
    case 0b011010: // l32
      l32(registers, instrucao, mem8, mem32);
      break;
    case 0b110111: // bun
      bun(registers, output);
      break;
    case 0b111111: // int
      interger(registers, &executa, output);
      break;
    default: // Instrucao desconhecida
      printf("Instrucao desconhecida!\n");
      // Parar a execucao
      executa = 0;
    }
    // PC = PC + 4 (proxima instrucao)
    registers[PC] = registers[PC] + 4;
    // Exibindo a finalizacao da execucao
  }

  // Output formatting to file
  fprintf(output, "[END OF SIMULATION]\n");

  printf("[END OF SIMULATION]\n");
}

/******************************************************
 * Arithmetic and logical operations
 *******************************************************/

void mov(uint32_t registers[NUM_REGISTERS], char instruction[30])
{
  uint8_t z = 0;
  uint32_t xyl = 0;

  // Obtendo operandos
  z = (registers[IR] & (0b11111 << 21)) >> 21;
  xyl = registers[IR] & 0x1FFFFF;

  // Execucao do comportamento
  registers[z] = xyl;
  // Formatacao da instrucao
  sprintf(instruction, "mov r%u,%u", z, xyl);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=0x%08X\n", registers[PC], instruction, z, xyl);
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bun(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  registers[PC] = registers[PC] + (label << 2);

  // Instruction formatting
  sprintf(instruction, "bun %i", label);

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void interger(uint32_t registers[NUM_REGISTERS], uint8_t *executa, FILE *output)
{
  char instruction[30] = {0};

  // Execution of behavior
  (*executa) = 0;
  memset(registers, 0, sizeof(uint32_t) * NUM_REGISTERS);

  // Instruction formatting
  sprintf(instruction, "int 0");

  // Screen output formatting
  printf("0x%08X:\t%-25s\tCR=0x00000000,PC=0x00000000\n", registers[PC], instruction);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tCR=0x00000000,PC=0x00000000\n", registers[PC], instruction);
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(uint32_t registers[NUM_REGISTERS], char instruction[30], uint8_t *mem8, uint32_t *mem32)
{
  uint8_t z = 0, x = 0, i = 0;

  // Otendo operandos
  z = (registers[IR] & (0b11111 << 21)) >> 21;
  x = (registers[IR] & (0b11111 << 16)) >> 16;
  i = registers[IR] & 0xFFFF;
  // Execucao do comportamento com MEM8 e MEM32
  registers[z] = mem8[registers[x] + i] | (((uint8_t *)(mem32))[(registers[x] + i) >> 2]);
  // Formatacao da instrucao
  sprintf(instruction, "l8 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=MEM[0x%08X]=0x%02X\n", registers[PC], instruction, z, registers[x] + i, registers[z]);
}

void l32(uint32_t registers[NUM_REGISTERS], char instruction[30], uint8_t *mem8, uint32_t *mem32)
{
  uint8_t z = 0, x = 0, i = 0;

  // Otendo operandos
  z = (registers[IR] & (0b11111 << 21)) >> 21;
  x = (registers[IR] & (0b11111 << 16)) >> 16;
  i = registers[IR] & 0xFFFF;
  // Execucao do comportamento com MEM8 e MEM32
  registers[z] = ((mem8[((registers[x] + i) << 2) + 0] << 24) | (mem8[((registers[x] + i) << 2) + 1] << 16) | (mem8[((registers[x] + i) << 2) + 2] << 8) | (mem8[((registers[x] + i) << 2) + 3] << 0)) | mem32[registers[x] + i];
  // Formatacao da instrucao
  sprintf(instruction, "l32 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=MEM[0x%08X]=0x%08X\n", registers[PC], instruction, z, (registers[x] + i) << 2, registers[z]);
}
