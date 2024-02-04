/******************************************************
 * Libraries
 *******************************************************/

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
#define CR 26  // Case interruption
#define IPC 27 // Interrupt address
#define IR 28  // Instruction Register
#define PC 29  // Program Counter
#define SP 30  // Stack Pointer
#define SR 31  // Status Register

// Flags in Status Register
#define ZN_FLAG 0b01000000
#define ZD_FLAG 0b00100000
#define SN_FLAG 0b00010000
#define OV_FLAG 0b00001000
#define IV_FLAG 0b00000100
#define IE_FLAG 0b00000010
#define CY_FLAG 0b00000001

// Interrupt Address
#define INIT_INTERRUPT_ADDR 0x00000000
#define DIVIDE_BY_ZERO_ADDR 0x00000008
#define INVALID_INSTRUCTION_ADDR 0x00000004
#define SOFTWARE_INTERRUPT_ADDR 0x0000000C
#define HARDWARE1_INTERRUPT_ADDR 0x00000010
#define HARDWARE2_INTERRUPT_ADDR 0x00000014
#define HARDWARE3_INTERRUPT_ADDR 0x00000018
#define HARDWARE4_INTERRUPT_ADDR 0x0000001C

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

void reti(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output);
void cbr(uint32_t registers[NUM_REGISTERS], FILE *output);
void sbr(uint32_t registers[NUM_REGISTERS], FILE *output);
void interrupt(uint32_t registers[NUM_REGISTERS], bool *run, FILE *output);

void unknownInstruction(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented);

int isZNSet(uint32_t registers[NUM_REGISTERS]);
int isZDSet(uint32_t registers[NUM_REGISTERS]);
int isSNSet(uint32_t registers[NUM_REGISTERS]);
int isOVSet(uint32_t registers[NUM_REGISTERS]);
int isIVSet(uint32_t registers[NUM_REGISTERS]);
int isIESet(uint32_t registers[NUM_REGISTERS]);
int isCYSet(uint32_t registers[NUM_REGISTERS]);

int32_t extendSign32(uint32_t value, uint8_t significantBit);
int64_t extendSign64(uint32_t value, uint8_t significantBit);
void printInstruction(uint32_t pc, FILE *output, char *instruction, char *additionalInfo);
char *formatRegisterName(uint8_t registerNumber, bool lower);
void printInterruptMessage(uint32_t address, FILE *output);

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

  bool run = true;
  bool pcAlreadyIncremented = false; // Flag to track whether the PC has been incremented

  while (run)
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
        uint8_t subOpcode = (registers[IR] >> 8) & 0x7;

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
        or (registers, output);
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

      case 0b100000: // reti
        reti(registers, mem8, output);
        break;
      case 0b100001: // cbr, sbr
        subOpcode = registers[IR] & 0x1;

        switch (subOpcode)
        {
        case 0b0: // cbr
          cbr(registers, output);
          break;
        case 0b1: // sbr
          sbr(registers, output);
          break;
        }
        break;
      case 0b111111: // int
        interrupt(registers, &run, output);
        break;

      default: // Unknown instruction
        unknownInstruction(registers, output, &pcAlreadyIncremented);
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
  // Fetch operands
  const uint8_t z = (registers[IR] & 0x03E00000) >> 21;
  const uint32_t xyl = registers[IR] & 0x1FFFFF;

  // Execution of behavior
  if (z != 0)
    registers[z] = xyl;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "mov %s,%u", formatRegisterName(z, true), xyl);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), xyl);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void movs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] & 0x03E00000) >> 21;
  const int32_t xyl = extendSign32(registers[IR] & 0x1FFFFF, 21);

  // Execution of behavior
  if (z != 0)
    registers[z] = xyl;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "movs %s,%i", formatRegisterName(z, true), xyl);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), xyl);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void add(uint32_t registers[NUM_REGISTERS], FILE *output)
{

  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX + valueY;

  if (z != 0)
    registers[z] = (uint32_t)result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) == (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "add %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s+%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void sub(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX - valueY;

  if (z != 0)
    registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "sub %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s-%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void mul(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX * valueY;
  if (z != 0)
    registers[z] = (uint32_t)result;

  if (l != 0)
    registers[l] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (registers[l] != 0)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "mul %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s:%s=%s*%s=0x%08X%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void sll(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)registers[z];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = z != 0 ? ((valueZ << 32) | valueY) << (l + 1) : valueY << (l + 1);

  if (x != 0)
    registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
    registers[z] = (result >> 32) & 0xFFFFFFFF;

  if (registers[z] != 0)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sll %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s<<%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, registers[z], registers[x], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void muls(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint64_t valueX = extendSign64(registers[x], 32);
  const uint64_t valueY = extendSign64(registers[y], 32);

  const int64_t result = valueX * valueY;

  if (z != 0)
    registers[z] = (uint32_t)result;

  if (l != 0)
    registers[l] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (registers[l] != 0)
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "muls %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s:%s=%s*%s=0x%08X%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void sla(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)registers[z];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = ((valueZ << 32) | valueY) << (l + 1);

  if (x != 0)
    registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
  {
    registers[z] = (result >> 32) & 0xFFFFFFFF;

    if (registers[z] != 0)
      registers[SR] |= OV_FLAG;
    else
      registers[SR] &= ~OV_FLAG;
  }

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sla %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s<<%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, registers[z], registers[x], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void divv(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint32_t valueX = registers[x];
  const uint32_t valueY = registers[y];

  if (valueY != 0)
  {
    if (z != 0)
      registers[z] = valueX / valueY;

    if (l != 0)
      registers[l] = valueX % valueY;

    if (registers[z] == 0)
      registers[SR] |= ZN_FLAG;
    else
      registers[SR] &= ~ZN_FLAG;

    if (registers[l] != 0)
      registers[SR] |= CY_FLAG;
    else
      registers[SR] &= ~CY_FLAG;
  }

  if (valueY == 0)
    registers[SR] |= ZD_FLAG;
  else
    registers[SR] &= ~ZD_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "div %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void srl(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)registers[z];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = z != 0 ? ((valueZ << 32) | valueY) >> (l + 1) : valueY >> (l + 1);

  if (x != 0)
    registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
  {
    registers[z] = (result >> 32) & 0xFFFFFFFF;

    if (registers[z] != 0)
      registers[SR] |= OV_FLAG;
    else
      registers[SR] &= ~OV_FLAG;
  }

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "srl %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s>>%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, registers[z], registers[x], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void divs(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const int32_t valueX = registers[x];
  const int32_t valueY = registers[y];

  if (valueY != 0)
  {
    if (l != 0)
      registers[l] = valueX % valueY;

    if (z != 0)
      registers[z] = valueX / valueY;

    if (registers[z] == 0)
      registers[SR] |= ZN_FLAG;
    else
      registers[SR] &= ~ZN_FLAG;

    if (registers[l] != 0)
      registers[SR] |= OV_FLAG;
    else
      registers[SR] &= ~OV_FLAG;
  }

  if (valueY == 0)
    registers[SR] |= ZD_FLAG;
  else
    registers[SR] &= ~ZD_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "divs %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void sra(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;
  const uint8_t l = registers[IR] & 0x1F;

  // Execution of behavior
  const int64_t valueZ = extendSign64(registers[z], 32);
  const int64_t valueY = extendSign64(registers[y], 32);

  const int64_t result = ((valueZ << 32) | valueY) >> (l + 1);

  if (x != 0)
    registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
    registers[z] = (result >> 32) & 0xFFFFFFFF;

  if (registers[z] != 0)
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sra %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s>>%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, registers[z], registers[x], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void cmp(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t valueY = (uint64_t)registers[y];

  const uint64_t result = valueX - valueY;

  if (z != 0)
    registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "cmp %s,%s", formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "SR=0x%08X", registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void and (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)registers[x];
  const uint32_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX & valueY;

  if (z != 0)
    registers[z] = result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "and %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s&%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void or (uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)registers[x];
  const uint32_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX | valueY;

  if (z != 0)
    registers[z] = result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "or %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s|%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void not(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)registers[x];

  const uint32_t result = ~valueX;

  if (z != 0)
    registers[z] = result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "not %s,%s", formatRegisterName(z, true), formatRegisterName(x, true));
  sprintf(additionalInfo, "%s=~%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void xor (uint32_t registers[NUM_REGISTERS], FILE *output) {
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint8_t y = (registers[IR] >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)registers[x];
  const uint32_t valueY = (uint32_t)registers[y];

  const uint32_t result = valueX ^ valueY;

  if (z != 0)
    registers[z] = result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (result & 0x80000000)
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "xor %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s^%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

    void addi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];

  const uint64_t result = valueX + (uint64_t)i;

  if (z != 0)
    registers[z] = (uint32_t)result;

  if (registers[z] == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) == (i & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[60] = {0};

  sprintf(instruction, "addi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s+0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void subi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];

  const uint64_t result = valueX - i;

  if (z != 0)
    registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG;
  else
    registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (i & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "subi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s-0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void muli(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const int64_t valueX = extendSign64(registers[x], 32);
  const int64_t result = valueX * i;

  if (z != 0)
    registers[z] = (uint32_t)result;

  if (result == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0xFFFFFFFF00000000) != 0)
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "muli %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s*0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void divi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const int32_t valueX = registers[x];

  if (i != 0)
  {
    if (z != 0)
      registers[z] = valueX / i;

    if (registers[z] == 0)
      registers[SR] |= ZN_FLAG;
    else
      registers[SR] &= ~ZN_FLAG;
  }

  if (i == 0)
    registers[SR] |= ZD_FLAG;
  else
    registers[SR] &= ~ZD_FLAG;

  registers[SR] &= ~OV_FLAG; // OV

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "divi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s/0x%08X=0x%08X,SR=0x%08X",
          formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void modi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const int32_t valueX = registers[x];

  if (z != 0)
    registers[z] = valueX % i;

  if (registers[z] == 0)
    registers[SR] |= ZN_FLAG;
  else
    registers[SR] &= ~ZN_FLAG;

  if (i == 0)
    registers[SR] |= ZD_FLAG;
  else
    registers[SR] &= ~ZD_FLAG;

  registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "modi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s%%0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, registers[z], registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void cmpi(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int64_t i = extendSign64(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)registers[x];
  const uint64_t result = valueX - (uint64_t)i;

  if (result == 0)
    registers[SR] |= ZN_FLAG; // error
  else
    registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    registers[SR] |= SN_FLAG; // error
  else
    registers[SR] &= ~SN_FLAG;

  if ((valueX & 0x80000000) != (i & 0x80000000) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    registers[SR] |= OV_FLAG;
  else
    registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    registers[SR] |= CY_FLAG;
  else
    registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "cmpi %s,%ld", formatRegisterName(x, true), i);
  sprintf(additionalInfo, "SR=0x%08X", registers[SR]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bae(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bae %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isCYSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bat(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bat %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(registers) || isCYSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbe(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bbe %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(registers) && !isCYSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bbt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isCYSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void beq(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "beq %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bge(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bge %i", i);
  sprintf(additionalInfo, "PC=0x%08X",
          isSNSet(registers) != isOVSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bgt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bgt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(registers) || (isSNSet(registers) != isOVSet(registers)) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void biv(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "biv %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isIVSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ble(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "ble %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(registers) && (isSNSet(registers) == isOVSet(registers)) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void blt(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "blt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isSNSet(registers) == isOVSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bne(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bne %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bni(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bni %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isIVSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bnz(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bnz %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZDSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bzd(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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

  sprintf(instruction, "bzd %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZDSet(registers) ? registers[PC] + 4 : registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bun(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  *(pcAlreadyIncremented) = true;
  const uint32_t oldPC = registers[PC];
  registers[PC] = registers[PC] + 4 + (i << 2);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bun %i", i);
  sprintf(additionalInfo, "PC=0x%08X", registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? registers[x] + i : i;

  if (z != 0)
    registers[z] = mem8[memoryAddress];

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "l8 %s,[%s%s%i]", formatRegisterName(z, true), formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "%s=MEM[0x%08X]=0x%02X", formatRegisterName(z, false), memoryAddress, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void l16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 1) : i << 1;

  if (z != 0)
    registers[z] = ((mem8[memoryAddress] << 24) |
                    (mem8[memoryAddress + 1] << 16));

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "l16 %s,[%s%s%i]", formatRegisterName(z, true), formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "%s=MEM[0x%08X]=0x%04X", formatRegisterName(z, false), memoryAddress, registers[z] >> 16);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void l32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 2) : i << 2;

  if (z != 0)
    registers[z] = ((mem8[memoryAddress] << 24) |
                    (mem8[memoryAddress + 1] << 16) |
                    (mem8[memoryAddress + 2] << 8) |
                    (mem8[memoryAddress + 3] << 0));

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "l32 %s,[%s%s%i]", formatRegisterName(z, true), formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i);
  sprintf(additionalInfo, "%s=MEM[0x%08X]=0x%08X", formatRegisterName(z, false), memoryAddress, registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s8(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? registers[x] + i : i;

  if (memoryAddress <= 1024)
    mem8[memoryAddress] = registers[z];

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "s8 [%s%s%i],%s", formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i, formatRegisterName(z, true));
  sprintf(additionalInfo, "MEM[0x%08X]=%s=0x%02X", memoryAddress, formatRegisterName(z, false), registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s16(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 1) : i << 1;
  mem8[memoryAddress] = (registers[z] >> 24) & 0xFF;
  mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "s16 [%s%s%i],%s", formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i, formatRegisterName(z, true));
  sprintf(additionalInfo, "MEM[0x%08X]=%s=0x%04X", memoryAddress, formatRegisterName(z, false), registers[z] >> 16);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s32(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 2) : i << 2;
  mem8[memoryAddress + 0] = (registers[z] >> 24) & 0xFF;
  mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;
  mem8[memoryAddress + 2] = (registers[z] >> 8) & 0xFF;
  mem8[memoryAddress + 3] = (registers[z]) & 0xFF;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "s32 [%s%s%i],%s", formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i, formatRegisterName(z, true));
  sprintf(additionalInfo, "MEM[0x%08X]=%s=0x%08X", memoryAddress, formatRegisterName(z, false), registers[z]);

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
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

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
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

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
  sprintf(additionalInfo, "PC=0x%08X,MEM[0x%08X]=0x%08X", registers[PC], registers[SP] + 4, oldPC + 4);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ret(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output, bool *pcAlreadyIncremented)
{
  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented twice

  const uint32_t oldPC = registers[PC];
  registers[SP] += 4;
  registers[PC] = ((mem8[registers[SP]] << 24) |
                   (mem8[registers[SP] + 1] << 16) |
                   (mem8[registers[SP] + 2] << 8) |
                   (mem8[registers[SP] + 3] << 0));

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[42] = {0};

  sprintf(instruction, "ret");
  sprintf(additionalInfo, "PC=MEM[0x%08X]=0x%08X", registers[SP], registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void push(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint32_t v = (registers[IR] >> 6) & 0x1F;
  const uint32_t w = registers[IR] & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;
  const uint32_t z = (registers[IR] >> 21) & 0x1F;

  const uint32_t operands[] = {v, w, x, y, z};

  // Execution of behavior
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand == 0)
      break;

    mem8[registers[SP] + 0] = (registers[operand] >> 24) & 0xFF;
    mem8[registers[SP] + 1] = (registers[operand] >> 16) & 0xFF;
    mem8[registers[SP] + 2] = (registers[operand] >> 8) & 0xFF;
    mem8[registers[SP] + 3] = (registers[operand]) & 0xFF;

    registers[SP] -= 4;
  }

  // Instruction formatting
  char instruction[50] = {0};
  char additionalInfo[300] = {0};

  char registerValues[100] = {0};
  char registerLabels[100] = {0};

  int tempInstruction = sprintf(instruction, "push %s", (operands[0] == 0) ? ("-") : (""));
  int tempValues = sprintf(registerValues, "%s", "");
  int tempLabels = sprintf(registerLabels, "%s", "");

  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand == 0)
      break;

    // Instruction
    tempInstruction += sprintf(instruction + tempInstruction, "%s%s",
                               (i > 0) ? (",") : (""), formatRegisterName(operand, true));

    // Register Values
    tempValues += sprintf(registerValues + tempValues, "%s0x%08X",
                          (i > 0) ? (",") : (""), registers[operand]);

    // Register Labels
    tempLabels += sprintf(registerLabels + tempLabels, "%s%s",
                          (i > 0) ? (",") : (""), formatRegisterName(operand, false));
  }

  sprintf(additionalInfo, "MEM[0x%08X]{%s}={%s}", registers[SP], registerValues, registerLabels);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void pop(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Fetch operands
  const uint32_t v = (registers[IR] >> 6) & 0x1F;
  const uint32_t w = registers[IR] & 0x1F;
  const uint32_t x = (registers[IR] >> 16) & 0x1F;
  const uint32_t y = (registers[IR] >> 11) & 0x1F;
  const uint32_t z = (registers[IR] >> 21) & 0x1F;

  const uint32_t operands[] = {v, w, x, y, z};

  // Execution of behavior
  const uint32_t oldSP = registers[SP];
  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand == 0)
      break;

    registers[SP] += 4;
    registers[operand] = ((mem8[registers[SP] + 0] << 24) |
                          (mem8[registers[SP] + 1] << 16) |
                          (mem8[registers[SP] + 2] << 8) |
                          (mem8[registers[SP] + 3] << 0));
  }

  // Instruction formatting
  char instruction[50] = {0};
  char additionalInfo[300] = {0};

  char registerValues[100] = {0};
  char registerLabels[100] = {0};

  int tempInstruction = sprintf(instruction, "pop %s", (operands[0] == 0) ? ("-") : (""));
  int tempValues = sprintf(registerValues, "%s", "");
  int tempLabels = sprintf(registerLabels, "%s", "");

  for (uint8_t i = 0; i < 5; i++)
  {
    const uint32_t operand = operands[i];
    if (operand == 0)
      break;

    // Instruction
    tempInstruction += sprintf(instruction + tempInstruction, "%s%s",
                               (i > 0) ? (",") : (""), formatRegisterName(operand, true));

    // Register Values
    tempValues += sprintf(registerValues + tempValues, "%s0x%08X",
                          (i > 0) ? (",") : (""), registers[operand]);

    // Register Labels
    tempLabels += sprintf(registerLabels + tempLabels, "%s%s",
                          (i > 0) ? (",") : (""), formatRegisterName(operand, false));
  }

  sprintf(additionalInfo, "{%s}=MEM[0x%08X]{%s}", registerLabels, oldSP, registerValues);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

/******************************************************
 * Iterruption
 *******************************************************/

void reti(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, FILE *output)
{
  // Execution of behavior
  registers[SP] += 4;
  registers[IPC] = mem8[registers[SP]];

  registers[SP] += 4;
  registers[CR] = mem8[registers[SP]];

  registers[SP] += 4;
  registers[PC] = mem8[registers[SP]];

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[300] = {0};

  sprintf(instruction, "reti");
  sprintf(additionalInfo, "IPC=MEM[0x%08X]=0x%08X,CR=MEM[0x%08X]=0x%08X,PC=MEM[0x%08X]=0x%08X", registers[IPC], registers[SP] - 8, registers[CR], registers[SP] - 4, registers[PC], registers[SP]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void cbr(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  registers[z] &= ~(0x00000001 << x);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[300] = {0};

  sprintf(instruction, "cbr %s[%i]",
          formatRegisterName(z, true), x);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), registers[z]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void sbr(uint32_t registers[NUM_REGISTERS], FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  registers[z] |= (0x00000001 << x);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[300] = {0};

  sprintf(instruction, "sbr %s[%i]",
          formatRegisterName(z, true), x);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), registers[z]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void interrupt(uint32_t registers[NUM_REGISTERS], bool *run, FILE *output)
{
  // Fetch operands
  const uint32_t i = registers[IR] & 0x3FFFFF;

  // Execution of behavior
  const uint32_t oldPC = registers[PC];
  if (i == 0)
  {
    (*run) = 0;
    memset(registers, 0, sizeof(uint32_t) * NUM_REGISTERS);
  }
  else
  {
    registers[CR] = i;
    registers[IPC] = oldPC;
    registers[PC] = SOFTWARE_INTERRUPT_ADDR;
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[300] = {0};

  sprintf(instruction, "int %i", i);
  sprintf(additionalInfo, "CR=0x%08X,PC=0x%08X", registers[CR], registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

/******************************************************
 * Routines interrupt handling
 *******************************************************/

void unknownInstruction(uint32_t registers[NUM_REGISTERS], FILE *output, bool *pcAlreadyIncremented)
{

  // Execution of behavior
  *(pcAlreadyIncremented) = true;
  const uint32_t oldPC = registers[PC];

  registers[SR] |= IV_FLAG;
  registers[CR] = (registers[IR] >> 26) & 0x3F;
  registers[IPC] = registers[PC];
  registers[PC] = INVALID_INSTRUCTION_ADDR;

  // Instruction formatting
  char instruction[100] = {0};
  sprintf(instruction, "[INVALID INSTRUCTION @ 0x%08X]\n", oldPC);

  // Output formatting to file
  printf("%s", instruction);
  fprintf(output, "%s", instruction);

  // Screen output formatting
  printInterruptMessage((uint32_t)SOFTWARE_INTERRUPT_ADDR, output);
}

/******************************************************
 * Fetch from the status register(SR)
 *******************************************************/

int isCYSet(uint32_t registers[NUM_REGISTERS])
{
  return (registers[SR] & CY_FLAG) != 0;
}

int isIESet(uint32_t registers[NUM_REGISTERS])
{
  return (registers[SR] & IE_FLAG) != 0;
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

int32_t extendSign32(uint32_t value, uint8_t significantBit)
{
  const uint32_t bitSignalDefined = value & (1 << (significantBit - 1));
  return (bitSignalDefined ? (value | (0xFFFFFFFF << (significantBit))) : value);
}

int64_t extendSign64(uint32_t value, uint8_t significantBit)
{
  const uint32_t bitSignalDefined = value & (1 << (significantBit - 1));
  return (bitSignalDefined ? (value | (0xFFFFFFFFFFFFFFFF << (significantBit))) : value);
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

void printInterruptMessage(uint32_t address, FILE *output)
{

  switch (address)
  {
  case SOFTWARE_INTERRUPT_ADDR:
    printf("[SOFTWARE INTERRUPTION]\n");
    fprintf(output, "[SOFTWARE INTERRUPTION]\n%s", "");
    break;
  case HARDWARE1_INTERRUPT_ADDR:
    printf("[HARDWARE INTERRUPTION 1]\n");
    fprintf(output, "[HARDWARE INTERRUPTION 1]\n%s", "");
    break;
  case HARDWARE2_INTERRUPT_ADDR:
    printf("[HARDWARE INTERRUPTION 2]\n");
    fprintf(output, "[HARDWARE INTERRUPTION 2]\n%s", "");
    break;
  case HARDWARE3_INTERRUPT_ADDR:
    printf("[HARDWARE INTERRUPTION 3]\n");
    fprintf(output, "[HARDWARE INTERRUPTION 3]\n%s", "");
    break;
  case HARDWARE4_INTERRUPT_ADDR:
    printf("[HARDWARE INTERRUPTION 4]\n");
    fprintf(output, "[HARDWARE INTERRUPTION 4]\n%s", "");
    break;
  }
}
