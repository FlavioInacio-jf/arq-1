/******************************************************
 * Libraries
 *******************************************************/

#include <math.h>
#include <stdint.h> // Integer types with specific widths.
#include <stdlib.h> // General-purpose functions, including memory allocation and random number generation.
#include <stdio.h>  // Standard Input/Output library for input and output operations.
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/******************************************************
 * Utility Constants
 *******************************************************/

#define NUM_REGISTERS 32

// Specific use register indexes
#define IR 28 // Instruction Register
#define PC 29 // Program Counter
#define SP 30 // Stack Pointer
#define SR 31 // Status Register

// Flags in Status Register
#define ZN_FLAG 0b01000000
#define ZD_FLAG 0b00100000
#define SN_FLAG 0b00010000
#define OV_FLAG 0b00001000
#define IV_FLAG 0b00000100
#define CY_FLAG 0b00000001

/******************************************************
 * Functin Signature
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8); // Load memory vector from a file
void decodeInstructions(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);

void mov(uint32_t registers[NUM_REGISTERS], FILE *output);
void movs(uint32_t registers[NUM_REGISTERS], FILE *output);
void add(uint32_t registers[NUM_REGISTERS], FILE *output);
void sub(uint32_t registers[NUM_REGISTERS], FILE *output);
void mul(uint32_t registers[NUM_REGISTERS], FILE *output);
void sll(uint32_t registers[NUM_REGISTERS], FILE *output);
void muls(uint32_t registers[NUM_REGISTERS], FILE *output);
void sla(uint32_t registers[NUM_REGISTERS], FILE *output);
void divv(uint32_t registers[NUM_REGISTERS], FILE *output);
void srl(uint32_t registers[NUM_REGISTERS], FILE *output);
void divs(uint32_t registers[NUM_REGISTERS], FILE *output);
void sra(uint32_t registers[NUM_REGISTERS], FILE *output);
void cmp(uint32_t registers[NUM_REGISTERS], FILE *output);
void and (uint32_t registers[NUM_REGISTERS], FILE *output);
void or (uint32_t registers[NUM_REGISTERS], FILE *output);
void not(uint32_t registers[NUM_REGISTERS], FILE *output);
void xor (uint32_t registers[NUM_REGISTERS], FILE *output);
void addi(uint32_t registers[NUM_REGISTERS], FILE *output);
void subi(uint32_t registers[NUM_REGISTERS], FILE *output);
void muli(uint32_t registers[NUM_REGISTERS], FILE *output);
void divi(uint32_t registers[NUM_REGISTERS], FILE *output);
void modi(uint32_t registers[NUM_REGISTERS], FILE *output);
void cmpi(uint32_t registers[NUM_REGISTERS], FILE *output);

void bae(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bat(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bbe(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bbt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void beq(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bge(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bgt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void biv(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void ble(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void blt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bne(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bni(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bnz(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bzd(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void bun(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);
void interrupt(uint32_t registers[NUM_REGISTERS], bool *executa, FILE *output);

void l8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void l16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void l32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void s8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void s16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void s32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);

void callf(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented);
void calls(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented);
void ret(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented);
void push(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void pop(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);

int isZNSet(uint32_t registers[NUM_REGISTERS]);
int isZDSet(uint32_t registers[NUM_REGISTERS]);
int isSNSet(uint32_t registers[NUM_REGISTERS]);
int isOVSet(uint32_t registers[NUM_REGISTERS]);
int isIVSet(uint32_t registers[NUM_REGISTERS]);
int isCYSet(uint32_t registers[NUM_REGISTERS]);

int32_t extendSign(uint32_t value, uint8_t significantBit);
void printInstruction(uint32_t pc, FILE *output, char *instruction, char *additionalInfo);
char *formatRegisterName(uint8_t registerNumber, bool lower);
int power(int base, int exponent);

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

  loadMemory(input, mem8);
  decodeInstructions(registers, mem8, output);

  fclose(input);
  fclose(output);
  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void loadMemory(FILE *input, uint8_t *mem8)
{

  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  char hexString[32];

  while (fgets(hexString, sizeof(char) * 32, input) != NULL)
  {
    const uint32_t hexCode = strtoul(hexString, NULL, 16);

    mem8[count] = (hexCode & 0xFF000000) >> 24;
    mem8[count + 1] = (hexCode & 0x00FF0000) >> 16;
    mem8[count + 2] = (hexCode & 0x0000FF00) >> 8;
    mem8[count + 3] = (hexCode & 0x000000FF);

    count += 4;
  }
}

void decodeInstructions(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Output formatting to file
  fprintf(output, "[START OF SIMULATION]\n");

  printf("[START OF SIMULATION]\n");

  bool executa = true;
  bool pcAlreadyIncremented = false; // Flag to track whether the PC has been incremented

  while (executa)
  {
    registers[IR] = ((mem8[registers[PC] + 0] << 24) |
                     (mem8[registers[PC] + 1] << 16) |
                     (mem8[registers[PC] + 2] << 8) |
                     (mem8[registers[PC] + 3] << 0));

    const uint8_t opcode = (registers[IR] >> 26) & 0x3F;

    if (registers[IR] != 0) // If it is not idle instruction
    {
      switch (opcode)
      {
      case 0b000000: // mov
        mov(registers, output);
        break;
      case 0b000001: // movs
        movs(registers, output);
        break;
      case 0b000010: // add
        add(registers, output);
        break;
      case 0b000011: // sub
        sub(registers, output);
        break;
      case 0b000100: // mul, sll, muls, sla, div, srl, divs, sra
        const uint8_t subOpcode = (registers[IR] >> 8) & 0x7;

        switch (subOpcode)
        {
        case 0b000: // mul
          mul(registers, output);
          break;
        case 0b001: // sll
          sll(registers, output);
          break;
        case 0b010: // muls
          muls(registers, output);
          break;
        case 0b011: // sla
          sla(registers, output);
          break;
        case 0b100: // div
          divv(registers, output);
          break;
        case 0b101: // srl
          srl(registers, output);
          break;
        case 0b110: // divs
          divs(registers, output);
          break;
        case 0b111: // sra
          sra(registers, output);
          break;
        }

        break;
      case 0b000101: // cmp
        cmp(registers, output);
        break;
      case 0b000110: // and
        and(registers, output);
        break;
      case 0b000111: // or
        and(registers, output);
        break;
      case 0b001000: // not
        not(registers, output);
        break;
      case 0b001001: // xor
        xor(registers, output);
        break;
      case 0b010010: // addi
        addi(registers, output);
        break;
      case 0b010011: // subi
        subi(registers, output);
        break;
      case 0b010100: // muli
        muli(registers, output);
        break;
      case 0b010101: // divi
        divi(registers, output);
        break;
      case 0b010110: // modi
        modi(registers, output);
        break;
      case 0b010111: // cmpi
        cmpi(registers, output);
        break;

      case 0b011000: // l8
        l8(registers, mem8, output);
        break;
      case 0b011001: // l16
        l16(registers, mem8, output);
        break;
      case 0b011010: // l32
        l32(registers, mem8, output);
        break;
      case 0b011011: // s8
        s8(registers, mem8, output);
        break;
      case 0b011100: // s16
        s16(registers, mem8, output);
        break;
      case 0b011101: // s32
        s32(registers, mem8, output);
        break;

      case 0b101010: // bae
        bae(registers, output, &pcAlreadyIncremented);
        break;
      case 0b101011: // bat
        bat(registers, output, &pcAlreadyIncremented);
        break;
      case 0b101100: // bbe
        bbe(registers, output, &pcAlreadyIncremented);
        break;
      case 0b101101: // bbt
        bbt(registers, output, &pcAlreadyIncremented);
        break;
      case 0b101110: // beq
        beq(registers, output, &pcAlreadyIncremented);
        break;
      case 0b101111: // bge
        bge(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110000: // bgt
        bgt(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110001: // biv
        biv(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110010: // ble
        ble(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110011: // blt
        blt(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110100: // bne
        bne(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110101: // bni
        bni(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110110: // bnz
        bnz(registers, output, &pcAlreadyIncremented);
        break;
      case 0b110111: // bun
        bun(registers, output, &pcAlreadyIncremented);
        break;
      case 0b111000: // bzd
        bzd(registers, output, &pcAlreadyIncremented);
        break;
      case 0b111111: // int
        interrupt(registers, &executa, output);
        break;

      case 0b011110: // call type F
        callf(registers, mem8, output, &pcAlreadyIncremented);
        break;
      case 0b111001: // call type S
        calls(registers, mem8, output, &pcAlreadyIncremented);
        break;
      case 0b011111: // ret
        ret(registers, mem8, output, &pcAlreadyIncremented);
        break;
      case 0b001010: // push
        push(registers, mem8, output);
        break;
      case 0b001011: // pop
        pop(registers, mem8, output);
        break;

      default: // Instrucao desconhecida
        printf("[INVALID INSTRUCTION @ 0x%08X]\n", registers[PC]);
        executa = 0; // Parar a execucao
      }
    }
    if (!pcAlreadyIncremented)
      registers[PC] += 4; // PC = PC + 4 (next instruction)

    pcAlreadyIncremented = false;
  }

  // Output formatting to file
  fprintf(output, "[END OF SIMULATION]\n");

  printf("[END OF SIMULATION]\n");
}

/******************************************************
 * Arithmetic and logical operations
 *******************************************************/

void mov(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] & 0x03E00000) >> 21;
  const uint32_t xyl = registers[IR] & 0x1FFFFF;

  // Execution of behavior
  registers[z] = xyl;

  // Instruction formatting
  sprintf(instruction, "mov %s,%u", formatRegisterName(z, true), xyl);

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), xyl);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), xyl);
}

void movs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] & 0x03E00000) >> 21;
  const int32_t xyl = (registers[IR] & 0x1FFFFF) |
                      ((registers[IR] & 0x100000) ? 0xFFF00000 : 0x00000000);

  // Execution of behavior
  registers[z] = xyl;

  // Instruction formatting
  sprintf(instruction, "movs %s,%i", formatRegisterName(z, true), xyl);

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), xyl);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), xyl);
}

void add(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "add %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX + valueY;
  registers[z] = (uint32_t)result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if ((result & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  if ((valueX > 0 && valueY > 0 && result <= 0) || (valueX < 0 && valueY < 0 && result >= 0))
  {
    registers[SR] |= OV_FLAG;
  }

  if (result > 0xFFFFFFFF)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s+%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s+%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void sub(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "sub %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX - valueY;
  registers[z] = (uint32_t)result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if ((result & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  if ((valueX > 0 && valueY > 0 && result <= 0) || (valueX < 0 && valueY < 0 && result >= 0))
  {
    registers[SR] |= OV_FLAG;
  }

  if (result > 0xFFFFFFFF)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s-%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s+%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void mul(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Instruction formatting
  sprintf(instruction, "mul %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX * valueY;
  registers[z] = (uint32_t)result;
  registers[l] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (registers[l] != 0)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s:%s=%s*%s=0x%016lX,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(l, false), formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), result, registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s+%s=0x%016lX,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), result, registers[SR]);
}

void sll(uint32_t registers[NUM_REGISTERS], FILE *output)
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

void sla(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const int32_t l = registers[IR] & 0x1F;

  // Instruction formatting
  sprintf(instruction, "sla %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);

  // Execution of behavior
  const uint64_t valueZ = registers[z];
  const uint64_t valueY = registers[y];

  const uint64_t result = ((valueZ << 32) | valueY) * power(2, l + 1);
  registers[x] = result & 0xFFFFFFFF;
  registers[z] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (registers[z] != 0)
  {
    registers[SR] |= OV_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s:%s=%s:%s<<%u=0x%016lX,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, result, registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s:%s=%s:%s<<%u=0x%016lX,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, result, registers[SR]);

  // Falta corrigir
}

void divv(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint32_t l = registers[IR] & 0x1F;

  // Instruction formatting
  sprintf(instruction, "div %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);

  // Execution of behavior
  const uint32_t valueX = registers[x];
  const uint32_t valueY = registers[y];

  registers[l] = valueX % valueY;
  registers[z] = valueX / valueY;

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (valueY == 0)
  {
    registers[SR] |= ZD_FLAG;
  }

  if (registers[l] != 0)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void srl(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void divs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const int32_t l = (registers[IR] & 0x1F) |
                    ((registers[IR] & 0x000010) ? 0xFFFFFFE0 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "divs %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);

  // Execution of behavior
  const uint32_t valueX = registers[x];
  const uint32_t valueY = registers[y];

  registers[l] = valueX % valueY;
  registers[z] = valueX / valueY;

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (valueY == 0)
  {
    registers[SR] |= ZD_FLAG;
  }

  if (registers[l] != 0)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void sra(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void cmp(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 10) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "cmp %s,%s", formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX - valueY;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if ((result & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  if (((valueX & 0x8FFFFFFF) == (valueY & 0x8FFFFFFF)) ||
      (valueX < 0 && valueY < 0 && result >= 0))
  {
    registers[SR] |= OV_FLAG;
  }

  if (result > 0xFFFFFFFF)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tSR=0x%08X\n", registers[PC], instruction, registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tSR=0x%08X\n", registers[PC], instruction, registers[SR]);

  // Falta corrigir
}

void and (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "and %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint32_t)registers[x];
  const uint64_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX & valueY;
  registers[z] = result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (result & 0x80000000) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  printf("0x%08X:\t%-25s\t%s=%s&%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s&%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void or (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "or %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint32_t)registers[x];
  const uint64_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX | valueY;
  registers[z] = result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (result & 0x80000000) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  printf("0x%08X:\t%-25s\t%s=%s|%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s|%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

void not(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "not %s,%s", formatRegisterName(z, true), formatRegisterName(x, true));

  // Execution of behavior
  const uint64_t valueX = (uint32_t)registers[x];

  const uint32_t result = ~valueX;
  registers[z] = result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (result & 0x80000000) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  printf("0x%08X:\t%-25s\t%s=~%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=~%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), registers[z], registers[SR]);
}

void xor (uint32_t registers[NUM_REGISTERS], FILE *output) {
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;

  // Instruction formatting
  sprintf(instruction, "xor %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));

  // Execution of behavior
  const uint64_t valueX = (uint32_t)registers[x];
  const uint64_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX ^ valueY;
  registers[z] = result;

  if (result == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (result & 0x80000000) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  printf("0x%08X:\t%-25s\t%s=%s^%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s^%s=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);
}

    void addi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = (registers[IR] & 0xFFFF) |
                    ((registers[IR] & 0x00004000) ? 0xFC000000 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "addi %s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), i);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];

  const uint64_t result = valueX + (uint64_t)i;
  registers[z] = (uint32_t)result;

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if ((result & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  if ((valueX > 0 && i > 0 && result <= 0) || (valueX < 0 && i < 0 && result >= 0))
  {
    registers[SR] |= OV_FLAG;
  }

  if (result > 0xFFFFFFFF)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s+0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s+0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);
}

void subi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = (registers[IR] & 0xFFFF) |
                    ((registers[IR] & 0x00004000) ? 0xFC000000 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "subi %s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), i);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];

  const uint64_t result = valueX - (uint64_t)i;
  registers[z] = (uint32_t)result;

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if ((result & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG;
  }

  if ((valueX >= 0 && i > 0 && result > valueX) || (valueX < 0 && i < 0 && result < valueX))
  {
    registers[SR] |= OV_FLAG;
  }

  if (result > 0xFFFFFFFF)
  {
    registers[SR] |= CY_FLAG;
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s-0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s-0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Falta corrigir
}

void muli(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  uint32_t x, y = 0;

  // Falta fazer
}

void divi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t i = (registers[IR] & 0xFFFF) |
                     ((registers[IR] & 0x00004000) ? 0xFC000000 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "divi %s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), i);

  // Execution of behavior
  const uint32_t valueX = registers[x];

  if (i != 0)
  {
    registers[z] = valueX / i;
  }

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (i == 0)
  {
    registers[SR] |= ZD_FLAG;
  }

  registers[SR] |= OV_FLAG;

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s/0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s/0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);
}

void modi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t i = (registers[IR] & 0xFFFF) |
                     ((registers[IR] & 0x00004000) ? 0xFC000000 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "modi %s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), i);

  // Execution of behavior
  const uint32_t valueX = registers[x];

  registers[z] = valueX % i;

  if (registers[z] == 0)
  {
    registers[SR] |= ZN_FLAG;
  }

  if (i == 0)
  {
    registers[SR] |= ZD_FLAG;
  }

  registers[SR] |= OV_FLAG;

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=%s%%0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=%s%%0x%08X=0x%08X,SR=0x%08X\n", registers[PC], instruction, formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);
}

void cmpi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  char instruction[30] = {0};

  // Fetch operands
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t valueX = (int32_t)registers[x];

  const int32_t i = (registers[IR] & 0xFFFF) |
                    ((registers[IR] & 0x02000000) ? 0xFC000000 : 0x00000000);

  // Instruction formatting
  sprintf(instruction, "cmpi r%u,%i", x, i);

  // Execution of behavior
  const int32_t dff = valueX - i;

  if (dff == 0)
  {
    registers[SR] = ZN_FLAG; // ZN
  }

  if ((dff & 0x80000000)) // Check MSB
  {
    registers[SR] |= SN_FLAG; // SN
  }

  if ((valueX & 0x8FFFFFFF) != (i & 0x8FFFFFFF) &&
      ((dff & 0x8FFFFFF) != (valueX & 0x8FFFFFF)))
  {
    registers[SR] |= OV_FLAG; // OV
  }

  if ((dff & 0x80000000) == 1) // Check MSB
  {
    registers[SR] |= CY_FLAG; // CY
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tSR=0x%08X\n", registers[PC], instruction, registers[SR]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tSR=0x%08X\n", registers[PC], instruction, registers[SR]);
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bae(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isCYSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bae %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bat(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers) && !isCYSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bat %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbe(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers) || isCYSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bbe %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isCYSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bbt %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void beq(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "beq %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bge(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isSNSet(registers) == isOVSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bge %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bgt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers) && (isSNSet(registers) == isOVSet(registers)))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bgt %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void biv(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isIVSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "biv %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ble(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZNSet(registers) || (isSNSet(registers) != isOVSet(registers)))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "ble %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void blt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isSNSet(registers) != isOVSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "blt %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bne(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZNSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bne %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bni(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isIVSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bni %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bnz(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (!isZDSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bnz %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bzd(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (isZDSet(registers))
  {
    *(pcAlreadyIncremented) = true;
    registers[PC] = registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bzd %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bun(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  *(pcAlreadyIncremented) = true;
  const uint32_t oldPC = registers[PC];
  registers[PC] = registers[PC] + 4 + (i << 2);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bun %u", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void interrupt(uint32_t registers[NUM_REGISTERS], bool *executa, FILE *output)
{
  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  (*executa) = 0;
  memset(registers, 0, sizeof(uint32_t) * NUM_REGISTERS);

  // Output
  printInstruction(oldPC, output, "int 0", "CR=0x00000000,PC=0x00000000");
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  registers[z] = mem8[registers[x] + i];

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "l8 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "R%u=MEM[0x%08X]=0x%08X", z, registers[x] + i, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void l16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = ((registers[x] + i) << 1);
  registers[z] = ((mem8[memoryAddress] << 24) |
                  (mem8[memoryAddress + 1] << 16));

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "l16 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "R%u=MEM[0x%08X]=0x%08X", z, memoryAddress, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void l32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = ((registers[x] + i) << 2);
  registers[z] = ((mem8[memoryAddress] << 24) |
                  (mem8[memoryAddress + 1] << 16) |
                  (mem8[memoryAddress + 2] << 8) |
                  (mem8[memoryAddress + 3] << 0));

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "l32 r%u,[r%u%s%i]", z, x, (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "R%u=MEM[0x%08X]=0x%08X", z, memoryAddress, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  mem8[registers[x] + i] = registers[z];

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "s8 [r%u%s%i],r%u", x, (i >= 0) ? ("+") : (""), i, z);
  sprintf(additionalInfo, "MEM[0x%08X]=R%u=0x%08X", registers[x] + i, z, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = ((registers[x] + i) << 1);
  mem8[memoryAddress] = (registers[z] >> 24) & 0xFF;
  mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "s16 [r%u%s%i],r%u", x, (i >= 0) ? ("+") : (""), i, z);
  sprintf(additionalInfo, "MEM[0x%08X]=R%u=0x%08X", memoryAddress, z, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = ((registers[x] + i) << 2);
  mem8[memoryAddress + 0] = (registers[z] >> 24) & 0xFF;
  mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;
  mem8[memoryAddress + 2] = (registers[z] >> 8) & 0xFF;
  mem8[memoryAddress + 3] = (registers[z]) & 0xFF;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "s32 [r%u%s%i],r%u", x, (i >= 0) ? ("+") : (""), i, z);
  sprintf(additionalInfo, "MEM[0x%08X]=R%u=0x%08X", memoryAddress, z, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

/******************************************************
 * Subroutine call operation
 *******************************************************/

void callf(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented
  const uint32_t oldPC = registers[PC];

  mem8[registers[SP]] = ((registers[PC] + 4) >> 24) & 0xFF;
  mem8[registers[SP] + 1] = ((registers[PC] + 4) >> 16) & 0xFF;
  mem8[registers[SP] + 2] = ((registers[PC] + 4) >> 8) & 0xFF;
  mem8[registers[SP] + 3] = ((registers[PC] + 4) >> 0) & 0xFF;

  registers[PC] = (registers[x] + i) << 2;
  registers[SP] -= 4;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[42] = {0};

  sprintf(instruction, "call [%s%s%i]",
          formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "PC=0x%08X,MEM[0x%08X]=0x%08X", registers[PC], registers[SP] + 4, oldPC + 4);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void calls(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented twice

  const uint32_t oldPC = registers[PC];

  mem8[registers[SP]] = ((registers[PC] + 4) >> 24) & 0xFF;
  mem8[registers[SP] + 1] = ((registers[PC] + 4) >> 16) & 0xFF;
  mem8[registers[SP] + 2] = ((registers[PC] + 4) >> 8) & 0xFF;
  mem8[registers[SP] + 3] = ((registers[PC] + 4) >> 0) & 0xFF;

  registers[PC] = registers[PC] + 4 + (i << 2);
  registers[SP] = registers[SP] - 4;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[42] = {0};

  sprintf(instruction, "call %i", i);
  sprintf(additionalInfo, "PC=0x%08X,MEM[0x%08X]=0x%08X", oldPC, registers[SP] + 4, oldPC + 4);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ret(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented)
{
  char instruction[30] = {0};

  // Instruction formatting
  sprintf(instruction, "ret");

  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented twice

  const uint32_t oldPC = registers[PC];
  registers[SP] += 4;
  registers[PC] = ((mem8[registers[SP]] << 24) |
                   (mem8[registers[SP] + 1] << 16) |
                   (mem8[registers[SP] + 2] << 8) |
                   (mem8[registers[SP] + 3] << 0));

  // Screen output formatting
  printf("0x%08X:\t%-25s\tPC=MEM[0x%08X]=0x%08X\n", oldPC, instruction, registers[SP], registers[PC]);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tPC=MEM[0x%08X]=0x%08X\n", oldPC, instruction, registers[SP], registers[PC]);
}

void push(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  char instruction[50] = {0};
  char valuesRegistry[100] = {0};

  // Fetch operands
  const uint32_t v = (registers[IR] >> 6) & 0x1F;
  const uint32_t w = registers[IR] & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;
  const uint32_t z = (registers[IR] >> 21) & 0x1F;

  const uint32_t operands[] = {v, w, x, y, z};

  // Instruction formatting
  int charsWritten = sprintf(instruction, "push ");
  int charsRegister = sprintf(valuesRegistry, "{");

  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      // Instruction
      charsWritten += sprintf(instruction + charsWritten, "%s%s", (i > 0) ? (",") : (""), formatRegisterName(operand, true));

      // Values ​​in the registry
      charsRegister += sprintf(valuesRegistry + charsRegister, "0x%08X%s", registers[operand], (i > 0) ? (",") : (""));
    }

    // Add braces at the end for values ​​and initial braces for labels
    if (i == 4)
    {
      charsRegister += sprintf(valuesRegistry + charsRegister, "}={");
    }
  }

  // Add register labels
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      // Values ​​in the registry
      charsRegister += sprintf(valuesRegistry + charsRegister, "%s%s", formatRegisterName(i + 1, false), (i > 0) ? (",") : (""));
    }

    if (i == 4)
    {
      charsRegister += sprintf(valuesRegistry + charsRegister, "}");
    }
  }

  // Execution of behavior
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      mem8[registers[SP] + 0] = (registers[operand] >> 24) & 0xFF;
      mem8[registers[SP] + 1] = (registers[operand] >> 16) & 0xFF;
      mem8[registers[SP] + 2] = (registers[operand] >> 8) & 0xFF;
      mem8[registers[SP] + 3] = (registers[operand]) & 0xFF;

      registers[SP] -= 4;
    }
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\tMEM[0x%08X]%s\n", registers[PC], instruction, registers[SP], valuesRegistry);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\tMEM[0x%08X]%s\n", registers[PC], instruction, registers[SP], valuesRegistry);
}

void pop(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  char instruction[50] = {0};
  char labelsRegistry[100] = {0};
  char valuesRegistry[100] = {0};

  // Fetch operands
  const uint32_t v = (registers[IR] >> 6) & 0x1F;
  const uint32_t w = registers[IR] & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;
  const uint32_t z = (registers[IR] >> 21) & 0x1F;

  const uint32_t operands[] = {v, w, x, y, z};

  // Instruction formatting
  int charsWritten = sprintf(instruction, "pop ");
  int charsLabel = sprintf(labelsRegistry, "{");
  int charsValue = sprintf(valuesRegistry, "{");

  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      // Instruction
      charsWritten += sprintf(instruction + charsWritten, "%s%s", (i > 0) ? (",") : (""), formatRegisterName(operand, true));

      // Labels ​​in the registry
      charsLabel += sprintf(labelsRegistry + charsLabel, "%s%s", formatRegisterName(operand, false), (i > 0) ? (",") : (""));
    }

    if (i == 4)
    {
      charsLabel += sprintf(labelsRegistry + charsLabel, "}");
    }
  }

  // Execution of behavior
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      registers[SP] += 4;

      registers[operand] = ((mem8[registers[SP] + 0] << 24) |
                            (mem8[registers[SP] + 1] << 16) |
                            (mem8[registers[SP] + 2] << 8) |
                            (mem8[registers[SP] + 3] << 0));
    }
  }

  // Add register values
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand != 0)
    {
      // Values ​​in the registry
      charsValue += sprintf(valuesRegistry + charsValue, "0x%08X%s", registers[operand], (i > 0) ? (",") : (""));
    }

    if (i == 4)
    {
      charsValue += sprintf(valuesRegistry + charsValue, "}");
    }
  }

  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s=MEM[0x%08X]%s\n", registers[PC], instruction, labelsRegistry, registers[SP] - 4, valuesRegistry);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s=MEM[0x%08X]%s\n", registers[PC], instruction, labelsRegistry, registers[SP] - 4, valuesRegistry);
}

/******************************************************
 * Fetch from the status register(SR)
 *******************************************************/

int isCYSet(uint32_t registers[NUM_REGISTERS])
{
  return (registers[SR] & CY_FLAG) != 0;
}

int isIVSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] & IV_FLAG) >> 2) != 0;
}

int isOVSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] & OV_FLAG) >> 3) != 0;
}

int isSNSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] & SN_FLAG) >> 4) != 0;
}

int isZDSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] & ZD_FLAG) >> 5) != 0;
}

int isZNSet(uint32_t registers[NUM_REGISTERS])
{
  return ((registers[SR] & ZN_FLAG) >> 6) != 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

int32_t extendSign(uint32_t value, uint8_t significantBit)
{
  const uint32_t bitSignalDefined = value & (1 << (significantBit - 1));
  return (bitSignalDefined ? (value | (0xFFFFFFFF << (significantBit))) : value);
}

void printInstruction(uint32_t pc, FILE *output, char *instruction, char *additionalInfo)
{
  // Screen output formatting
  printf("0x%08X:\t%-25s\t%s\n", pc, instruction, additionalInfo);

  // Output formatting to file
  fprintf(output, "0x%08X:\t%-25s\t%s\n", pc, instruction, additionalInfo);
}

char *formatRegisterName(uint8_t registerNumber, bool lower)
{
  char *result;

  if (registerNumber >= (NUM_REGISTERS - 4))
  {
    char instruction[3] = {0};

    switch (registerNumber)
    {
    case IR:
      sprintf(instruction, "IR");
      break;
    case PC:
      sprintf(instruction, "PC");
      break;
    case SP:
      sprintf(instruction, "SP");
      break;
    case SR:
      sprintf(instruction, "SR");
      break;
    }

    result = malloc(strlen(instruction) + 1);
    strcpy(result, instruction);
  }
  else
  {
    size_t len = snprintf(NULL, 0, "R%u", registerNumber) + 1;
    result = malloc(len);
    snprintf(result, len, "R%u", registerNumber);
  }

  if (lower)
  {
    for (size_t i = 0; result[i]; i++)
    {
      result[i] = tolower((unsigned char)result[i]);
    }
  }

  return result;
}

int power(int base, int exponent)
{
  if (exponent == 0)
  {
    return 1;
  }
  return base * power(base, exponent - 1);
}
