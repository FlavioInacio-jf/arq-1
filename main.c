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
 * Types
 *******************************************************/

struct TCPU
{
  uint32_t registers[NUM_REGISTERS];
};
typedef struct TCPU CPU;

struct FPU
{
};

struct Watchdog
{
};

struct DMA
{
};

struct TSystem
{
  CPU cpu;
  struct FPU fpu;
  struct Watchdog watchdog;
  struct DMA dma;
  uint8_t *memory;
};

typedef struct TSystem System;

/******************************************************
 * Functin Signature
 *******************************************************/
void initializeSystem(System *system, FILE *input, FILE *output);
void loadMemoryFromFile(System *system, FILE *input); // Load memory vector from a file
void decodeInstructions(System *system, FILE *output);

void mov(CPU *cpu, FILE *output);
void movs(CPU *cpu, FILE *output);
void add(CPU *cpu, FILE *output);
void sub(CPU *cpu, FILE *output);
void mul(CPU *cpu, FILE *output);
void sll(CPU *cpu, FILE *output);
void muls(CPU *cpu, FILE *output);
void sla(CPU *cpu, FILE *output);
void divv(CPU *cpu, FILE *output);
void srl(CPU *cpu, FILE *output);
void divs(CPU *cpu, FILE *output);
void sra(CPU *cpu, FILE *output);
void cmp(CPU *cpu, FILE *output);
void and (CPU * cpu, FILE *output);
void or (CPU * cpu, FILE *output);
void not(CPU * cpu, FILE *output);
void xor (CPU * cpu, FILE *output);
void addi(CPU *cpu, FILE *output);
void subi(CPU *cpu, FILE *output);
void muli(CPU *cpu, FILE *output);
void divi(CPU *cpu, FILE *output, uint32_t *interruptionCode);
void modi(CPU *cpu, FILE *output, uint32_t *interruptionCode);
void cmpi(CPU *cpu, FILE *output);

void bae(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bat(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bbe(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bbt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void beq(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bge(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bgt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void biv(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void ble(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void blt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bne(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bni(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bnz(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bzd(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);
void bun(CPU *cpu, FILE *output, bool *pcAlreadyIncremented);

void l8(System *system, FILE *output);
void l16(System *system, FILE *output);
void l32(System *system, FILE *output);
void s8(System *system, FILE *output);
void s16(System *system, FILE *output);
void s32(System *system, FILE *output);

void callf(System *system, FILE *output, bool *pcAlreadyIncremented);
void calls(System *system, FILE *output, bool *pcAlreadyIncremented);
void ret(System *system, FILE *output, bool *pcAlreadyIncremented);
void push(System *system, FILE *output);
void pop(System *system, FILE *output);

void reti(System *system, FILE *output, bool *pcAlreadyIncremented);
void cbr(CPU *cpu, FILE *output);
void sbr(CPU *cpu, FILE *output);
void interrupt(System *system, bool *run, FILE *output, uint32_t *interruptionCode);

void handprepareForISR(System *system);
void savePCPlus4ToStack(System *system);
void saveCRToStack(System *system);
void saveIPCToStack(System *system);

void unknownInstruction(CPU *cpu, FILE *output, bool *pcAlreadyIncremented, uint32_t *interruptionCode);

int isZNSet(CPU *cpu);
int isZDSet(CPU *cpu);
int isSNSet(CPU *cpu);
int isOVSet(CPU *cpu);
int isIVSet(CPU *cpu);
int isIESet(CPU *cpu);
int isCYSet(CPU *cpu);

int32_t extendSign32(uint32_t value, uint8_t significantBit);
int64_t extendSign64(uint32_t value, uint8_t significantBit);
void printInstruction(uint32_t pc, FILE *output, char *instruction, char *additionalInfo);
char *formatRegisterName(uint8_t registerNumber, bool lower);
void handleInterrupt(System *system, uint32_t *interruptionCode, FILE *output);
uint32_t readMemory32(System *system, uint32_t memoryAddress);

int main(int argc, char *argv[])
{
  // Input file
  FILE *input = fopen(argv[1], "r");
  if (input == NULL)
    exit(EXIT_FAILURE);

  // Output file
  FILE *output = fopen(argv[2], "w");
  if (output == NULL)
    exit(EXIT_FAILURE);

  System system;
  initializeSystem(&system, input, output);

  return 0;
}

/******************************************************
 * Utility Functions
 *******************************************************/

void initializeSystem(System *system, FILE *input, FILE *output)
{
  // 32 registers initialized to zero
  memset(system->cpu.registers, 0, sizeof(system->cpu.registers));

  // 32 KiB memory initialized to zero
  system->memory = (uint8_t *)(calloc(32 * 1024, sizeof(uint8_t)));

  loadMemoryFromFile(system, input);
  decodeInstructions(system, output);

  fclose(input);
  fclose(output);
  free(system->memory);
}

void loadMemoryFromFile(System *system, FILE *input)
{
  fseek(input, 0, SEEK_SET);

  unsigned int count = 0;
  char hexString[32];

  while (fgets(hexString, sizeof(char) * 32, input) != NULL)
  {
    const uint32_t hexCode = strtoul(hexString, NULL, 16);

    system->memory[count] = (hexCode & 0xFF000000) >> 24;
    system->memory[count + 1] = (hexCode & 0x00FF0000) >> 16;
    system->memory[count + 2] = (hexCode & 0x0000FF00) >> 8;
    system->memory[count + 3] = (hexCode & 0x000000FF);

    count += 4;
  }
}

void decodeInstructions(System *system, FILE *output)
{
  // Output formatting to file
  fprintf(output, "[START OF SIMULATION]\n");

  printf("[START OF SIMULATION]\n");

  bool run = true;
  bool pcAlreadyIncremented = false; // Flag to track whether the PC has been incremented
  uint32_t interruptionCode = 999;

  while (run)
  {
    const uint32_t pc = system->cpu.registers[PC];
    const uint32_t ir = ((system->memory[pc + 0] << 24) |
                         (system->memory[pc + 1] << 16) |
                         (system->memory[pc + 2] << 8) |
                         (system->memory[pc + 3] << 0));
    system->cpu.registers[IR] = ir;

    const uint8_t opcode = (ir >> 26) & 0x3F;
    if (ir != 0) // If it is not idle instruction
    {
      switch (opcode)
      {
      case 0b000000: // mov
        mov(&system->cpu, output);
        break;
      case 0b000001: // movs
        movs(&system->cpu, output);
        break;
      case 0b000010: // add
        add(&system->cpu, output);
        break;
      case 0b000011: // sub
        sub(&system->cpu, output);
        break;
      case 0b000100: // mul, sll, muls, sla, div, srl, divs, sra
        uint8_t subOpcode = (ir >> 8) & 0x7;

        switch (subOpcode)
        {
        case 0b000: // mul
          mul(&system->cpu, output);
          break;
        case 0b001: // sll
          sll(&system->cpu, output);
          break;
        case 0b010: // muls
          muls(&system->cpu, output);
          break;
        case 0b011: // sla
          sla(&system->cpu, output);
          break;
        case 0b100: // div
          divv(&system->cpu, output);
          break;
        case 0b101: // srl
          srl(&system->cpu, output);
          break;
        case 0b110: // divs
          divs(&system->cpu, output);
          break;
        case 0b111: // sra
          sra(&system->cpu, output);
          break;
        }

        break;
      case 0b000101: // cmp
        cmp(&system->cpu, output);
        break;
      case 0b000110: // and
        and(&system->cpu, output);
        break;
      case 0b000111: // or
        or (&system->cpu, output);
        break;
      case 0b001000: // not
        not(&system->cpu, output);
        break;
      case 0b001001: // xor
        xor(&system->cpu, output);
        break;
      case 0b010010: // addi
        addi(&system->cpu, output);
        break;
      case 0b010011: // subi
        subi(&system->cpu, output);
        break;
      case 0b010100: // muli
        muli(&system->cpu, output);
        break;
      case 0b010101: // divi
        divi(&system->cpu, output, &interruptionCode);
        break;
      case 0b010110: // modi
        modi(&system->cpu, output, &interruptionCode);
        break;
      case 0b010111: // cmpi
        cmpi(&system->cpu, output);
        break;

      case 0b011000: // l8
        l8(system, output);
        break;
      case 0b011001: // l16
        l16(system, output);
        break;
      case 0b011010: // l32
        l32(system, output);
        break;
      case 0b011011: // s8
        s8(system, output);
        break;
      case 0b011100: // s16
        s16(system, output);
        break;
      case 0b011101: // s32
        s32(system, output);
        break;

      case 0b101010: // bae
        bae(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b101011: // bat
        bat(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b101100: // bbe
        bbe(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b101101: // bbt
        bbt(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b101110: // beq
        beq(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b101111: // bge
        bge(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110000: // bgt
        bgt(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110001: // biv
        biv(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110010: // ble
        ble(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110011: // blt
        blt(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110100: // bne
        bne(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110101: // bni
        bni(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110110: // bnz
        bnz(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b110111: // bun
        bun(&system->cpu, output, &pcAlreadyIncremented);
        break;
      case 0b111000: // bzd
        bzd(&system->cpu, output, &pcAlreadyIncremented);
        break;

      case 0b011110: // call type F
        callf(system, output, &pcAlreadyIncremented);
        break;
      case 0b111001: // call type S
        calls(system, output, &pcAlreadyIncremented);
        break;
      case 0b011111: // ret
        ret(system, output, &pcAlreadyIncremented);
        break;
      case 0b001010: // push
        push(system, output);
        break;
      case 0b001011: // pop
        pop(system, output);
        break;

      case 0b100000: // reti
        reti(system, output, &pcAlreadyIncremented);
        break;
      case 0b100001: // cbr, sbr
        subOpcode = ir & 0x1;

        switch (subOpcode)
        {
        case 0b0: // cbr
          cbr(&system->cpu, output);
          break;
        case 0b1: // sbr
          sbr(&system->cpu, output);
          break;
        }
        break;
      case 0b111111: // int
        interrupt(system, &run, output, &interruptionCode);
        break;

      default: // Unknown instruction
        handprepareForISR(system);
        unknownInstruction(&system->cpu, output, &pcAlreadyIncremented, &interruptionCode);
      }
    }

    if (interruptionCode != 999)
      handleInterrupt(system, &interruptionCode, output);

    if (!pcAlreadyIncremented && interruptionCode == 999)
      system->cpu.registers[PC] += 4; // PC = PC + 4 (next instruction)

    pcAlreadyIncremented = false;
    interruptionCode = 999;
  }

  // Output formatting to file
  fprintf(output, "[END OF SIMULATION]\n");
  printf("[END OF SIMULATION]\n");
}

/******************************************************
 * Arithmetic and logical operations
 *******************************************************/

void mov(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = ir >> 21;
  const uint32_t xyl = ir & 0x1FFFFF;

  // Execution of behavior
  if (z != 0)
    cpu->registers[z] = xyl;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "mov %s,%u", formatRegisterName(z, true), xyl);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), cpu->registers[z]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void movs(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir & 0x03E00000) >> 21;
  const int32_t xyl = extendSign32(ir & 0x1FFFFF, 21);

  // Execution of behavior
  if (z != 0)
    cpu->registers[z] = xyl;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "movs %s,%i", formatRegisterName(z, true), xyl);
  sprintf(additionalInfo, "%s=0x%08X", formatRegisterName(z, false), cpu->registers[z]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void add(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = valueX + valueY;

  if (z != 0)
    cpu->registers[z] = (uint32_t)result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) == (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "add %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s+%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void sub(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = valueX - valueY;

  if (z != 0)
    cpu->registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "sub %s,%s,%s",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s-%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void mul(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = valueX * valueY;
  if (z != 0)
    cpu->registers[z] = (uint32_t)result;

  if (l != 0)
    cpu->registers[l] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (cpu->registers[l] != 0)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "mul %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s:%s=%s*%s=0x%08X%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[l], cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void sll(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)cpu->registers[z];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = z != 0 ? ((valueZ << 32) | valueY) << (l + 1) : valueY << (l + 1);

  if (x != 0)
    cpu->registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
    cpu->registers[z] = (result >> 32) & 0xFFFFFFFF;

  if (cpu->registers[z] != 0)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sll %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s<<%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, cpu->registers[z], cpu->registers[x], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void muls(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint64_t valueX = extendSign64(cpu->registers[x], 32);
  const uint64_t valueY = extendSign64(cpu->registers[y], 32);

  const int64_t result = valueX * valueY;

  if (z != 0)
    cpu->registers[z] = (uint32_t)result;

  if (l != 0)
    cpu->registers[l] = (result >> 32) & 0xFFFFFFFF;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (cpu->registers[l] != 0)
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "muls %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s:%s=%s*%s=0x%08X%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[l], cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void sla(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)cpu->registers[z];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = ((valueZ << 32) | valueY) << (l + 1);

  if (x != 0)
    cpu->registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
  {
    cpu->registers[z] = (result >> 32) & 0xFFFFFFFF;

    if (cpu->registers[z] != 0)
      cpu->registers[SR] |= OV_FLAG;
    else
      cpu->registers[SR] &= ~OV_FLAG;
  }

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sla %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s<<%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, cpu->registers[z], cpu->registers[x], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void divv(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint32_t valueX = cpu->registers[x];
  const uint32_t valueY = cpu->registers[y];

  if (valueY != 0)
  {
    if (z != 0)
      cpu->registers[z] = valueX / valueY;

    if (l != 0)
      cpu->registers[l] = valueX % valueY;

    if (cpu->registers[z] == 0)
      cpu->registers[SR] |= ZN_FLAG;
    else
      cpu->registers[SR] &= ~ZN_FLAG;

    if (cpu->registers[l] != 0)
      cpu->registers[SR] |= CY_FLAG;
    else
      cpu->registers[SR] &= ~CY_FLAG;
  }

  if (valueY == 0)
    cpu->registers[SR] |= ZD_FLAG;
  else
    cpu->registers[SR] &= ~ZD_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "div %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void srl(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const uint64_t valueZ = (uint64_t)cpu->registers[z];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = z != 0 ? ((valueZ << 32) | valueY) >> (l + 1) : valueY >> (l + 1);

  if (x != 0)
    cpu->registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
  {
    cpu->registers[z] = (result >> 32) & 0xFFFFFFFF;

    if (cpu->registers[z] != 0)
      cpu->registers[SR] |= OV_FLAG;
    else
      cpu->registers[SR] &= ~OV_FLAG;
  }

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "srl %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s>>%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, cpu->registers[z], cpu->registers[x], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void divs(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const int32_t valueX = cpu->registers[x];
  const int32_t valueY = cpu->registers[y];

  if (valueY != 0)
  {
    if (l != 0)
      cpu->registers[l] = valueX % valueY;

    if (z != 0)
      cpu->registers[z] = valueX / valueY;

    if (cpu->registers[z] == 0)
      cpu->registers[SR] |= ZN_FLAG;
    else
      cpu->registers[SR] &= ~ZN_FLAG;

    if (cpu->registers[l] != 0)
      cpu->registers[SR] |= OV_FLAG;
    else
      cpu->registers[SR] &= ~OV_FLAG;
  }

  if (valueY == 0)
    cpu->registers[SR] |= ZD_FLAG;
  else
    cpu->registers[SR] &= ~ZD_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "divs %s,%s,%s,%s",
          formatRegisterName(l, true), formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s%%%s=0x%08X,%s=%s/%s=0x%08X,SR=0x%08X", formatRegisterName(l, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[l], formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void sra(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;
  const uint8_t l = ir & 0x1F;

  // Execution of behavior
  const int64_t valueZ = extendSign64(cpu->registers[z], 32);
  const int64_t valueY = extendSign64(cpu->registers[y], 32);

  const int64_t result = ((valueZ << 32) | valueY) >> (l + 1);

  if (x != 0)
    cpu->registers[x] = result & 0xFFFFFFFF;

  if (z != 0)
    cpu->registers[z] = (result >> 32) & 0xFFFFFFFF;

  if (cpu->registers[z] != 0)
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "sra %s,%s,%s,%u",
          formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true), l);
  sprintf(additionalInfo, "%s:%s=%s:%s>>%u=0x%08X%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(z, false), formatRegisterName(y, false), l + 1, cpu->registers[z], cpu->registers[x], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void cmp(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];
  const uint64_t valueY = (uint64_t)cpu->registers[y];

  const uint64_t result = valueX - valueY;

  if (z != 0)
    cpu->registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (valueY & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "cmp %s,%s", formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "SR=0x%08X", cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void and (CPU * cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)cpu->registers[x];
  const uint32_t valueY = (uint32_t)cpu->registers[y];

  const uint32_t result = valueX & valueY;

  if (z != 0)
    cpu->registers[z] = result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "and %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s&%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void or (CPU * cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)cpu->registers[x];
  const uint32_t valueY = (uint32_t)cpu->registers[y];

  const uint32_t result = valueX | valueY;

  if (z != 0)
    cpu->registers[z] = result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "or %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s|%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void not(CPU * cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)cpu->registers[x];

  const uint32_t result = ~valueX;

  if (z != 0)
    cpu->registers[z] = result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (result & 0x80000000)
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "not %s,%s", formatRegisterName(z, true), formatRegisterName(x, true));
  sprintf(additionalInfo, "%s=~%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void xor (CPU * cpu, FILE *output) {
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const uint8_t y = (ir >> 11) & 0x1F;

  // Execution of behavior
  const uint32_t valueX = (uint32_t)cpu->registers[x];
  const uint32_t valueY = (uint32_t)cpu->registers[y];

  const uint32_t result = valueX ^ valueY;

  if (z != 0)
    cpu->registers[z] = result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (result & 0x80000000)
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "xor %s,%s,%s", formatRegisterName(z, true), formatRegisterName(x, true), formatRegisterName(y, true));
  sprintf(additionalInfo, "%s=%s^%s=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), formatRegisterName(y, false), cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

    void addi(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const int32_t i = extendSign32(ir & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];

  const uint64_t result = valueX + (uint64_t)i;

  if (z != 0)
    cpu->registers[z] = (uint32_t)result;

  if (cpu->registers[z] == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) == (i & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[60] = {0};

  sprintf(instruction, "addi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s+0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void subi(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const int32_t i = extendSign32(ir & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];

  const uint64_t result = valueX - i;

  if (z != 0)
    cpu->registers[z] = (result & 0xFFFFFFFF);

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG;
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if (
      ((valueX & 0x80000000) != (i & 0x80000000)) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "subi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s-0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void muli(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const int32_t i = extendSign32(ir & 0xFFFF, 16);

  // Execution of behavior
  const int64_t valueX = extendSign64(cpu->registers[x], 32);
  const int64_t result = valueX * i;

  if (z != 0)
    cpu->registers[z] = (uint32_t)result;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0xFFFFFFFF00000000) != 0)
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "muli %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s*0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void divi(CPU *cpu, FILE *output, uint32_t *interruptionCode)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const int32_t i = extendSign32(ir & 0xFFFF, 16);

  // Execution of behavior
  const int32_t valueX = cpu->registers[x];

  if (i != 0)
  {
    if (z != 0)
      cpu->registers[z] = valueX / i;

    if (cpu->registers[z] == 0)
      cpu->registers[SR] |= ZN_FLAG;
    else
      cpu->registers[SR] &= ~ZN_FLAG;
  }

  if (i == 0)
  {
    cpu->registers[SR] |= ZD_FLAG;
    (*interruptionCode) = DIVIDE_BY_ZERO_ADDR;
  }
  else
    cpu->registers[SR] &= ~ZD_FLAG;

  cpu->registers[SR] &= ~OV_FLAG; // OV

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "divi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s/0x%08X=0x%08X,SR=0x%08X",
          formatRegisterName(z, false), formatRegisterName(x, false), i, cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void modi(CPU *cpu, FILE *output, uint32_t *interruptionCode)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t z = (ir >> 21) & 0x1F;
  const uint8_t x = (ir >> 16) & 0x1F;
  const int32_t i = extendSign32(ir & 0xFFFF, 16);

  // Execution of behavior
  const int32_t valueX = cpu->registers[x];

  if (z != 0)
    cpu->registers[z] = valueX % i;

  if (cpu->registers[z] == 0)
    cpu->registers[SR] |= ZN_FLAG;
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if (i == 0)
  {
    cpu->registers[SR] |= ZD_FLAG;
    (*interruptionCode) = DIVIDE_BY_ZERO_ADDR;
  }
  else
    cpu->registers[SR] &= ~ZD_FLAG;

  cpu->registers[SR] &= ~OV_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "modi %s,%s,%i",
          formatRegisterName(z, true), formatRegisterName(x, true), i);
  sprintf(additionalInfo, "%s=%s%%0x%08X=0x%08X,SR=0x%08X", formatRegisterName(z, false), formatRegisterName(x, false), i, cpu->registers[z], cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

void cmpi(CPU *cpu, FILE *output)
{
  // Fetch operands
  const uint32_t ir = cpu->registers[IR];

  const uint8_t x = (ir >> 16) & 0x1F;
  const int64_t i = extendSign64(ir & 0xFFFF, 16);

  // Execution of behavior
  const uint64_t valueX = (uint64_t)cpu->registers[x];
  const uint64_t result = valueX - (uint64_t)i;

  if (result == 0)
    cpu->registers[SR] |= ZN_FLAG; // error
  else
    cpu->registers[SR] &= ~ZN_FLAG;

  if ((result & 0x80000000))
    cpu->registers[SR] |= SN_FLAG; // error
  else
    cpu->registers[SR] &= ~SN_FLAG;

  if ((valueX & 0x80000000) != (i & 0x80000000) &&
      ((result & 0x80000000) != (valueX & 0x80000000)))
    cpu->registers[SR] |= OV_FLAG;
  else
    cpu->registers[SR] &= ~OV_FLAG;

  if (result > 0xFFFFFFFF)
    cpu->registers[SR] |= CY_FLAG;
  else
    cpu->registers[SR] &= ~CY_FLAG;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[50] = {0};

  sprintf(instruction, "cmpi %s,%ld", formatRegisterName(x, true), i);
  sprintf(additionalInfo, "SR=0x%08X", cpu->registers[SR]);

  // Output
  printInstruction(cpu->registers[PC], output, instruction, additionalInfo);
}

/******************************************************
 * Flow Control Operations
 *******************************************************/

void bae(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isCYSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bae %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isCYSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bat(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isZNSet(cpu) && !isCYSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bat %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(cpu) || isCYSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbe(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isZNSet(cpu) || isCYSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bbe %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(cpu) && !isCYSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bbt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isCYSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bbt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isCYSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void beq(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isZNSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "beq %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bge(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isSNSet(cpu) == isOVSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bge %i", i);
  sprintf(additionalInfo, "PC=0x%08X",
          isSNSet(cpu) != isOVSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bgt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isZNSet(cpu) && (isSNSet(cpu) == isOVSet(cpu)))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bgt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(cpu) || (isSNSet(cpu) != isOVSet(cpu)) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void biv(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isIVSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "biv %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isIVSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ble(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isZNSet(cpu) || (isSNSet(cpu) != isOVSet(cpu)))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "ble %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZNSet(cpu) && (isSNSet(cpu) == isOVSet(cpu)) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void blt(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isSNSet(cpu) != isOVSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "blt %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isSNSet(cpu) == isOVSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bne(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isZNSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bne %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZNSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bni(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isIVSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bni %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isIVSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bnz(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (!isZDSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bnz %i", i);
  sprintf(additionalInfo, "PC=0x%08X", isZDSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bzd(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  const uint32_t oldPC = cpu->registers[PC];
  if (isZDSet(cpu))
  {
    *(pcAlreadyIncremented) = true;
    cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bzd %i", i);
  sprintf(additionalInfo, "PC=0x%08X", !isZDSet(cpu) ? cpu->registers[PC] + 4 : cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void bun(CPU *cpu, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(cpu->registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  *(pcAlreadyIncremented) = true;
  const uint32_t oldPC = cpu->registers[PC];
  cpu->registers[PC] = cpu->registers[PC] + 4 + (i << 2);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[30] = {0};

  sprintf(instruction, "bun %i", i);
  sprintf(additionalInfo, "PC=0x%08X", cpu->registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

/******************************************************
 * Memory read/write operations
 *******************************************************/

void l8(System *system, FILE *output)
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

void l16(System *system, uint8_t *mem8, FILE *output)
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

void l32(System *system, uint8_t *mem8, FILE *output)
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

void s8(System *system, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? registers[x] + i : i;

  if (memoryAddress < (NUM_REGISTERS * 1024))
    mem8[memoryAddress] = registers[z];

  // Instruction formatting
  char instruction[50] = {0};
  char additionalInfo[200] = {0};

  sprintf(instruction, "s8 [%s%s%i],%s", formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i, formatRegisterName(z, true));
  sprintf(additionalInfo, "MEM[0x%08X]=%s=0x%02X", memoryAddress, formatRegisterName(z, false), registers[z]);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s16(System *system, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 1) : i << 1;
  if (memoryAddress < (NUM_REGISTERS * 1024))
  {
    mem8[memoryAddress] = (registers[z] >> 24) & 0xFF;
    mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;
  }

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "s16 [%s%s%i],%s", formatRegisterName(x, true), (i >= 0) ? ("+") : (""), i, formatRegisterName(z, true));
  sprintf(additionalInfo, "MEM[0x%08X]=%s=0x%04X", memoryAddress, formatRegisterName(z, false), registers[z] >> 16);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void s32(System *system, FILE *output)
{
  // Fetch operands
  const uint8_t z = (registers[IR] >> 21) & 0x1F;
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const uint16_t i = registers[IR] & 0xFFFF;

  // Execution of behavior
  const uint32_t memoryAddress = (x != 0) ? ((registers[x] + i) << 2) : i << 2;
  if (memoryAddress < (NUM_REGISTERS * 1024))
  {
    mem8[memoryAddress + 0] = (registers[z] >> 24) & 0xFF;
    mem8[memoryAddress + 1] = (registers[z] >> 16) & 0xFF;
    mem8[memoryAddress + 2] = (registers[z] >> 8) & 0xFF;
    mem8[memoryAddress + 3] = (registers[z]) & 0xFF;
  }

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

void callf(System *system, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const uint8_t x = (registers[IR] >> 16) & 0x1F;
  const int32_t i = extendSign32(registers[IR] & 0xFFFF, 16);

  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented
  const uint32_t oldPC = registers[PC];
  const uint32_t oldSP = registers[SP];

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
  sprintf(additionalInfo, "PC=0x%08X,MEM[0x%08X]=0x%08X", registers[PC], oldSP, oldPC + 4);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void calls(System *system, FILE *output, bool *pcAlreadyIncremented)
{
  // Fetch operands
  const int32_t i = extendSign32(registers[IR] & 0x03FFFFFF, 26);

  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented twice

  const uint32_t oldPC = registers[PC];
  const uint32_t oldSP = registers[SP];

  mem8[registers[SP]] = ((registers[PC] + 4) >> 24) & 0xFF;
  mem8[registers[SP] + 1] = ((registers[PC] + 4) >> 16) & 0xFF;
  mem8[registers[SP] + 2] = ((registers[PC] + 4) >> 8) & 0xFF;
  mem8[registers[SP] + 3] = ((registers[PC] + 4) >> 0) & 0xFF;

  registers[PC] = registers[PC] + 4 + (i << 2);
  registers[SP] -= 4;

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[100] = {0};

  sprintf(instruction, "call %i", i);
  sprintf(additionalInfo, "PC=0x%08X,MEM[0x%08X]=0x%08X", registers[PC], oldSP, oldPC + 4);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void ret(System *system, FILE *output, bool *pcAlreadyIncremented)
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

void push(System *system, FILE *output)
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

  sprintf(additionalInfo, "MEM[0x%08X]{%s}={%s}", oldSP, registerValues, registerLabels);

  // Output
  printInstruction(registers[PC], output, instruction, additionalInfo);
}

void pop(System *system, FILE *output)
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

void reti(System *system, FILE *output, bool *pcAlreadyIncremented)
{
  // Execution of behavior
  *(pcAlreadyIncremented) = true; // Prevent it from being incremented twice
  const uint32_t oldPC = registers[PC];

  registers[SP] += 4;
  registers[IPC] = readMemory32(mem8, registers[SP]);

  registers[SP] += 4;
  registers[CR] = readMemory32(mem8, registers[SP]);

  registers[SP] += 4;
  registers[PC] = readMemory32(mem8, registers[SP]);

  // Instruction formatting
  char instruction[30] = {0};
  char additionalInfo[300] = {0};

  sprintf(instruction, "reti");
  sprintf(additionalInfo, "IPC=MEM[0x%08X]=0x%08X,CR=MEM[0x%08X]=0x%08X,PC=MEM[0x%08X]=0x%08X", registers[SP] - 8, registers[IPC], registers[SP] - 4, registers[CR], registers[SP], registers[PC]);

  // Output
  printInstruction(oldPC, output, instruction, additionalInfo);
}

void cbr(CPU *cpu, FILE *output)
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

void sbr(CPU *cpu, FILE *output)
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

void interrupt(System *system, bool *run, FILE *output, uint32_t *interruptionCode)
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
    handprepareForISR(registers, mem8);

    registers[CR] = i;
    registers[IPC] = registers[PC];
    registers[PC] = SOFTWARE_INTERRUPT_ADDR;
    (*interruptionCode) = SOFTWARE_INTERRUPT_ADDR;
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

void savePCPlus4ToStack(System *system)
{
  mem8[registers[SP] + 0] = ((registers[PC] + 4) >> 24) & 0xFF;
  mem8[registers[SP] + 1] = ((registers[PC] + 4) >> 16) & 0xFF;
  mem8[registers[SP] + 2] = ((registers[PC] + 4) >> 8) & 0xFF;
  mem8[registers[SP] + 3] = (registers[PC] + 4) & 0xFF;
  registers[SP] -= 4;
}

void saveCRToStack(System *system)
{
  mem8[registers[SP] + 0] = (registers[CR] >> 24) & 0xFF;
  mem8[registers[SP] + 1] = (registers[CR] >> 16) & 0xFF;
  mem8[registers[SP] + 2] = (registers[CR] >> 8) & 0xFF;
  mem8[registers[SP] + 3] = (registers[CR]) & 0xFF;
  registers[SP] -= 4;
}

void saveIPCToStack(System *system)
{
  mem8[registers[SP] + 0] = (registers[IPC] >> 24) & 0xFF;
  mem8[registers[SP] + 1] = (registers[IPC] >> 16) & 0xFF;
  mem8[registers[SP] + 2] = (registers[IPC] >> 8) & 0xFF;
  mem8[registers[SP] + 3] = (registers[IPC]) & 0xFF;
  registers[SP] -= 4;
}

void handprepareForISR(System *system)
{
  savePCPlus4ToStack(system);
  saveCRToStack(system);
  saveIPCToStack(system);
}

void unknownInstruction(CPU *cpu, FILE *output, bool *pcAlreadyIncremented, uint32_t *interruptionCode)
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

  (*interruptionCode) = INVALID_INSTRUCTION_ADDR;
}

/******************************************************
 * Fetch from the status register(SR)
 *******************************************************/

int isCYSet(CPU *cpu)
{
  return (cpu->registers[SR] & CY_FLAG) != 0;
}

int isIESet(CPU *cpu)
{
  return (cpu->registers[SR] & IE_FLAG) != 0;
}

int isIVSet(CPU *cpu)
{
  return ((cpu->registers[SR] & IV_FLAG) >> 2) != 0;
}

int isOVSet(CPU *cpu)
{
  return ((cpu->registers[SR] & OV_FLAG) >> 3) != 0;
}

int isSNSet(CPU *cpu)
{
  return ((cpu->registers[SR] & SN_FLAG) >> 4) != 0;
}

int isZDSet(CPU *cpu)
{
  return ((cpu->registers[SR] & ZD_FLAG) >> 5) != 0;
}

int isZNSet(CPU *cpu)
{
  return ((cpu->registers[SR] & ZN_FLAG) >> 6) != 0;
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

  if ((registerNumber >= 26))
  {
    char instruction[4] = {0};

    switch (registerNumber)
    {
    case IR:
      sprintf(instruction, "IR");
      break;
    case CR:
      sprintf(instruction, "CR");
      break;
    case IPC:
      sprintf(instruction, "IPC");
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

void handleInterrupt(uint32_t registers[NUM_REGISTERS], uint8_t *mem8, uint32_t *interruptionCode, FILE *output)
{
  char interruptMessage[100] = {0};

  switch (*interruptionCode)
  {
  case INVALID_INSTRUCTION_ADDR:
    sprintf(interruptMessage, "[SOFTWARE INTERRUPTION]");
    break;
  case SOFTWARE_INTERRUPT_ADDR:
    sprintf(interruptMessage, "[SOFTWARE INTERRUPTION]");
    break;
  case DIVIDE_BY_ZERO_ADDR:
    handprepareForISR(registers, mem8);

    registers[SR] |= ZD_FLAG;
    if (isIESet(registers))
    {
      registers[SR] |= ZD_FLAG;
      registers[CR] = 0;
      registers[IPC] = registers[PC];
      registers[PC] = DIVIDE_BY_ZERO_ADDR;
    }

    sprintf(interruptMessage, "[SOFTWARE INTERRUPTION]");
    break;
  case HARDWARE1_INTERRUPT_ADDR:
    sprintf(interruptMessage, "[HARDWARE INTERRUPTION 1]");
    break;
  case HARDWARE2_INTERRUPT_ADDR:
    sprintf(interruptMessage, "[HARDWARE INTERRUPTION 2]");
    break;
  case HARDWARE3_INTERRUPT_ADDR:
    sprintf(interruptMessage, "[HARDWARE INTERRUPTION 3]");
    break;
  case HARDWARE4_INTERRUPT_ADDR:
    sprintf(interruptMessage, "[HARDWARE INTERRUPTION 4]");
    break;
  default:
    break;
  }

  printf("%s\n", interruptMessage);
  fprintf(output, "%s\n%s", interruptMessage, "");
}

uint32_t readMemory32(uint8_t *mem8, uint32_t memoryAddress)
{
  return ((mem8[memoryAddress] << 24) |
          (mem8[memoryAddress + 1] << 16) |
          (mem8[memoryAddress + 2] << 8) |
          (mem8[memoryAddress + 3] << 0));
}
