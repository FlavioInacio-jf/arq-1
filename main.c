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

void mov(uint32_t registers[NUM_REGISTERS], FILE *output);

void bae(uint32_t registers[NUM_REGISTERS], FILE *output);
void bat(uint32_t registers[NUM_REGISTERS], FILE *output);
void bbe(uint32_t registers[NUM_REGISTERS], FILE *output);
void bbt(uint32_t registers[NUM_REGISTERS], FILE *output);
void beq(uint32_t registers[NUM_REGISTERS], FILE *output);
void bge(uint32_t registers[NUM_REGISTERS], FILE *output);
void bgt(uint32_t registers[NUM_REGISTERS], FILE *output);
void biv(uint32_t registers[NUM_REGISTERS], FILE *output);
void ble(uint32_t registers[NUM_REGISTERS], FILE *output);
void blt(uint32_t registers[NUM_REGISTERS], FILE *output);
void bne(uint32_t registers[NUM_REGISTERS], FILE *output);
void bni(uint32_t registers[NUM_REGISTERS], FILE *output);
void bnz(uint32_t registers[NUM_REGISTERS], FILE *output);
void bzd(uint32_t registers[NUM_REGISTERS], FILE *output);
void bun(uint32_t registers[NUM_REGISTERS], FILE *output);
void interrupt(uint32_t registers[NUM_REGISTERS], uint8_t *executa, FILE *output);

void l8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);
void l16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);
void l32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);
void s8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);
void s16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);
void s32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32);

int isZNSet(uint32_t registers[NUM_REGISTERS]);
int isZDSet(uint32_t registers[NUM_REGISTERS]);
int isSNSet(uint32_t registers[NUM_REGISTERS]);
int isOVSet(uint32_t registers[NUM_REGISTERS]);
int isIVSet(uint32_t registers[NUM_REGISTERS]);
int isCYSet(uint32_t registers[NUM_REGISTERS]);

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
      mov(registers, output);
      break;

    case 0b011000: // l8
      l8(registers, mem8, mem32);
      break;
    case 0b011001: // l16
      l16(registers, mem8, mem32);
      break;
    case 0b011010: // l32
      l32(registers, mem8, mem32);
      break;
    case 0b011011: // s8
      s8(registers, mem8, mem32);
      break;
    case 0b011100: // s16
      s16(registers, mem8, mem32);
      break;
    case 0b011101: // s32
      s32(registers, mem8, mem32);
      break;

    case 0b101010: // bae
      bae(registers, output);
      break;
    case 0b101011: // bat
      bat(registers, output);
      break;
    case 0b101100: // bbe
      bbe(registers, output);
      break;
    case 0b101101: // bbt
      bbt(registers, output);
      break;
    case 0b101110: // beq
      beq(registers, output);
      break;
    case 0b101111: // bge
      bge(registers, output);
      break;
    case 0b110000: // bgt
      bgt(registers, output);
      break;
    case 0b110001: // biv
      biv(registers, output);
      break;
    case 0b110010: // ble
      ble(registers, output);
      break;
    case 0b110011: // blt
      blt(registers, output);
      break;
    case 0b110100: // bne
      bne(registers, output);
      break;
    case 0b110101: // bni
      bni(registers, output);
      break;
    case 0b110110: // bnz
      bnz(registers, output);
      break;
    case 0b110111: // bun
      bun(registers, output);
      break;
    case 0b111000: // bzd
      bzd(registers, output);
      break;
    case 0b111111: // int
      interrupt(registers, &executa, output);
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

void add(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

void addi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

void mov(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t xyl = 0;

  // Fetch operands
  z = (registers[IR] & 0x03E00000) >> 21;
  xyl = registers[IR] & 0x1FFFFF;

  // Execution of behavior
  registers[z] = xyl;

  // Instruction formatting
  sprintf(instruction, "mov r%u,%u", z, xyl);

  // Screen output formatting
  printf("0x%08X:\t%-25s\tR%u=0x%08X\n", registers[PC], instruction, z, xyl);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tR%u=0x%08X\n", registers[PC], instruction, z, xyl);
}

void movs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t xyl = 0;

  // Fetch operands
  z = (registers[IR] & 0x03E00000) >> 21;
  xyl = registers[IR] & 0x1FFFFF;

  // Falta fazer
}

void and (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

void or (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

void not(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

void xor (uint32_t registers[NUM_REGISTERS], FILE *output) {
  char instruction[30] = {0};

  uint8_t z = 0;
  uint32_t x, y = 0;

  // Falta fazer
}

    void cmp(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void cmpi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void sla(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void sll(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void sra(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void srl(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void div(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void divs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void divi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void mul(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void muls(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void muli(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void sub(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void subi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bae(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bae %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isCYSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bat(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bat %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers) && !isCYSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bbe(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bbe %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers) || isCYSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bbt(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bbt %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isCYSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void beq(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "beq %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bge(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bge %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isSNSet(registers) == isOVSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bgt(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bgt %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers) && (isSNSet(registers) == isOVSet(registers)))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void biv(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "biv %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isIVSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void ble(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "ble %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers) || (isSNSet(registers) != isOVSet(registers)))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void blt(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "blt %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isSNSet(registers) != isOVSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bne(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bne %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bni(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bni %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isIVSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bnz(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bnz %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZDSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bzd(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bzd %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZDSet(registers))
  {
    registers[PC] = registers[PC] + (label << 2);
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void bun(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t label = registers[IR] & 0x03FFFFFF;

  // Instruction formatting
  sprintf(instruction, "bun %i", label);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  registers[PC] = registers[PC] + (label << 2);

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=0x%08X\n", oldPC, instruction, registers[PC] + 4);
}

void interrupt(uint32_t registers[NUM_REGISTERS], uint8_t *executa, FILE *output)
{
  char instruction[30] = {0};

  // Instruction formatting
  sprintf(instruction, "int 0");

  // Execution of behavior
  (*executa) = 0;
  memset(registers, 0, sizeof(uint32_t) * NUM_REGISTERS);

  // Screen output formatting
  printf("0x%08X:\t%-25s\tCR=0x00000000,PC=0x00000000\n", registers[PC], instruction);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tCR=0x00000000,PC=0x00000000\n", registers[PC], instruction);
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

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

void l16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

  uint8_t z = 0, x = 0, i = 0;

  // Falta fazer
}

void l32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

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

void s8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

  uint8_t z = 0, x = 0, i = 0;

  // Falta fazer
}

void s16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

  uint8_t z = 0, x = 0, i = 0;

  // Falta fazer
}

void s32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *mem32)
{
  char instruction[30] = {0};

  uint8_t z = 0, x = 0, i = 0;

  // Falta fazer
}

/******************************************************
 * Fetch from the status register(SR)
 *******************************************************/

int isCYSet(uint32_t registers[NUM_REGISTERS])
{
  return (registers[SR] && 0x00000001) != 0;
}

int isIVSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] && 0x00000004) >> 2) != 0;
}

int isOVSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] && 0x00000008) >> 3) != 0;
}

int isSNSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] && 0x00000010) >> 4) != 0;
}

int isZDSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] && 0x00000020) >> 5) != 0;
}

int isZNSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] && 0x00000040) >> 6) != 0;
}
