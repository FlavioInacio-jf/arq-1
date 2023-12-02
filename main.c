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

void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32); // Load memory vector from a file
void decodeInstruction(uint32_t registers[SIZE_MEMORY], uint8_t *mem8, uint32_t *mem32, uint8_t *executa);
void decodeInstructions(uint32_t registers[SIZE_MEMORY], uint8_t *mem8, uint32_t *mem32);

void mov(uint32_t registers[SIZE_MEMORY], char instruction[30], uint8_t *z, uint32_t *xyl);

void bun(uint32_t registers[SIZE_MEMORY], char instruction[30], uint32_t *pc);
void interger(uint32_t registers[SIZE_MEMORY], char instruction[30], uint8_t *executa);

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
  decodeInstructions(registers, mem8, mem32);

  fclose(input);
  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8, uint32_t *mem32)
{

  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  char hexString[SIZE_MEMORY];

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

void decodeInstructions(uint32_t registers[SIZE_MEMORY], uint8_t *mem8, uint32_t *mem32)
{
  printf("\nMEM8:\n");
  for (uint8_t i = 0; i < 48; i = i + 4)
    printf("0x%08X: 0x%02X 0x%02X 0x%02X 0x%02X\n", i, mem8[i], mem8[i + 1], mem8[i + 2], mem8[i + 3]);

  printf("\nMEM32:\n");
  for (uint8_t i = 0; i < 12; i = i + 1)
    printf("0x%08X: 0x%08X (0x%02X 0x%02X 0x%02X 0x%02X)\n", i << 2, mem32[i], ((uint8_t *)(mem32))[(i << 2) + 3], ((uint8_t *)(mem32))[(i << 2) + 2], ((uint8_t *)(mem32))[(i << 2) + 1], ((uint8_t *)(mem32))[(i << 2) + 0]);

  printf("\nSaida esperada\n\n      |       \n      V       \n\n");
  printf("[START OF SIMULATION]\n");

  uint8_t executa = 1;
  while (executa)
    decodeInstruction(registers, mem8, mem32, &executa);

  printf("[END OF SIMULATION]\n");
}

void decodeInstruction(uint32_t registers[SIZE_MEMORY], uint8_t *mem8, uint32_t *mem32, uint8_t *executa)
{

  char instrucao[30] = {0};

  uint8_t z = 0, x = 0, i = 0;
  uint32_t pc = 0, xyl = 0;

  registers[28] = ((mem8[registers[29] + 0] << 24) | (mem8[registers[29] + 1] << 16) | (mem8[registers[29] + 2] << 8) | (mem8[registers[29] + 3] << 0)) | mem32[registers[29] >> 2];

  uint8_t opcode = (registers[28] & (0b111111 << 26)) >> 26;
  switch (opcode)
  {

  case 0b000000: // mov
    mov(registers, instrucao, &z, &xyl);
    break;
  case 0b011000: // l8
    l8();
    break;
  case 0b011010: // l32
    l32();
    break;
  case 0b110111: // bun
    bun(registers, instrucao, &pc);
    break;
  case 0b111111: // int
    interger(registers, instrucao, &executa);
    break;
  default: // Instrucao desconhecida
    // Exibindo mensagem de erro
    printf("Instrucao desconhecida!\n");
    // Parar a execucao
    (*executa) = 0;
  }
  // PC = PC + 4 (proxima instrucao)
  registers[29] = registers[29] + 4;
  // Exibindo a finalizacao da execucao
}

/******************************************************
 * Arithmetic and logical operations
 *******************************************************/

void mov(uint32_t registers[SIZE_MEMORY], char instruction[30], uint8_t *z, uint32_t *xyl)
{

  // Obtendo operandos
  *z = (registers[28] & (0b11111 << 21)) >> 21;
  xyl = registers[28] & 0x1FFFFF;
  // Execucao do comportamento
  registers[*z] = xyl;
  // Formatacao da instrucao
  sprintf(instruction, "mov r%u,%u", z, xyl);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=0x%08X\n", registers[29], instruction, z, xyl);
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bun(uint32_t registers[SIZE_MEMORY], char instruction[30], uint32_t *pc)
{
  // Armazenando o PC antigo
  pc = registers[29];
  // Execucao do comportamento
  registers[29] = registers[29] + ((registers[28] & 0x3FFFFFF) << 2);
  // Formatacao da instrucao
  sprintf(instruction, "bun %i", registers[28] & 0x3FFFFFF);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", pc, instruction, registers[29] + 4);
}

void interger(uint32_t registers[SIZE_MEMORY], char instruction[30], uint8_t *executa)
{
  // Parar a execucao
  (*executa) = 0;
  // Formatacao da instrucao
  sprintf(instruction, "int 0");
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tCR=0x00000000,PC=0x00000000\n", registers[29], instruction);
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(uint32_t registers[SIZE_MEMORY], char instruction[30], uint8_t *executa)
{
  // Otendo operandos
  z = (registers[28] & (0b11111 << 21)) >> 21;
  x = (registers[28] & (0b11111 << 16)) >> 16;
  i = registers[28] & 0xFFFF;
  // Execucao do comportamento com MEM8 e MEM32
  registers[z] = mem8[registers[x] + i] | (((uint8_t *)(mem32))[(registers[x] + i) >> 2]);
  // Formatacao da instrucao
  sprintf(instruction, "l8 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=MEM[0x%08X]=0x%02X\n", registers[29], instruction, z, registers[x] + i, registers[z]);
}

void l32()
{
  // Otendo operandos
  z = (registers[28] & (0b11111 << 21)) >> 21;
  x = (registers[28] & (0b11111 << 16)) >> 16;
  i = registers[28] & 0xFFFF;
  // Execucao do comportamento com MEM8 e MEM32
  registers[z] = ((mem8[((registers[x] + i) << 2) + 0] << 24) | (mem8[((registers[x] + i) << 2) + 1] << 16) | (mem8[((registers[x] + i) << 2) + 2] << 8) | (mem8[((registers[x] + i) << 2) + 3] << 0)) | mem32[registers[x] + i];
  // Formatacao da instrucao
  sprintf(instrucao, "l32 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  // Formatacao de saida em tela (deve mudar para o arquivo de saida)
  printf("0x%08X:\t%-25s\tR%u=MEM[0x%08X]=0x%08X\n", registers[29], instrucao, z, (registers[x] + i) << 2, registers[z]);
}
