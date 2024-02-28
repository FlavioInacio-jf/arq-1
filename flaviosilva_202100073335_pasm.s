// Segmento de código
.text
	init:
		bun main
		.align 5
  readTerminal:
    // R10 = numbers (ARRAY POINTER)
    mov r10, numbers

    // R11 = 0 (COUNTER)
    mov r11, 0

    // START OF WHILE
    cmpi r11, 400
    beq 5

    // R3 = 1 BYTE FROM TERMINAL
    l8 r3, [r1]
    s8 [r10], r3

    addi r10, r10, 1
    addi r11, r11, 1

    // REPEAT THE INTERATION
    bun -7

    mov sr, 0
    ret
  convertNumberToAscii:
    // DIVIDE THE NUMBER BY 10
    mov r8, 10

    // DIGITS COUNTER
    mov r9, 0

    // START OF WHILE
    // DIVIDE THE NUMBER BY 10, R6 CONTAINS THE WHOLE PART, R5 THE REMAINDER
    div r5, r6, r3, r8

    // CONVERTS THE DIGIT TO ASCII BY ADDING THE ASCII VALUE OF '0'
    addi r7, r5, 48
    push r7

    addi r9, r9, 1
    addi r3, r6, 0

    // CHECKS IF THE INTEGER PART IS ZERO (END OF CONVERSION)
    cmpi r6, 0
    beq 1

    bun -8

    // ADD NUMBER IN BUFFER
    mov r4, buffer
    // COPY DIGITS COUNTER
    addi r7, r9, 0

    cmpi r9, 0
    beq 5

    pop r3
    s8 [r4], r3

    addi r4, r4, 1
    addi r9, r9, -1
    bun -7

    mov sr, 0
    ret
  writeTerminal:
    // R10 = numbers (ARRAY POINTER)
    mov r10, numbers
    // 32-BIT ALIGNMENT
    divi r10, r10, 4

    // START OF WHILE
    mov r11, 0
    cmpi r11, 100
    beq 19

    // READ NUMBER FROM ARRAY
    l32 r3, [r10]

    // CONFIGURE R4 TO RECEIVE THE CONVERTED NUMBER
    call convertNumberToAscii
    mov r4, buffer

    cmpi r7, 0
    beq 5
    l8 r3, [r4]

    // WRITE TO TERMINAL
    s8 [r2], r3

    addi r4, r4, 1
    addi r7, r7, -1
    bun -7

    addi r10, r10, 1
    addi r11, r11, 1

    // ADD SPACE
    mov sr, 0
    cmpi r11, 100
    beq 3
    mov r4, space
    l8 r4, [r4]
    s8 [r2], r4

    // REPEAT THE INTERATION
    bun -21

    mov sr, 0
    ret
  printHeader:
    l8 r11, [r10]
    cmpi r11, 0
    beq 3
    s8 [r2], r11
    addi r10, r10, 1
    bun -6

    mov sr, 0
    ret
  swap:
    // R3 = XP = ARRAY ELEMENT POINTER
    // R4 = XP = ELEMENT

    // R10 = YP = ARRAY ELEMENT POINTER
    // R5 = YP = ELEMENT


    l32 r5, [r10]
    l32 r4, [r3]

    // YP = XP
    s32 [r10], r4

    // XP = YP
    s32 [r3], r5

    ret
  findMinIndex:
    mov sr, 0

    // R7 = MIN INDEX = start
    addi r7, r10, 0
    // R8 = I = START + 1
    addi r8, r7, 1

    cmp r8, r20
    beq 8
    // arr[i]
    l32 r15, [r8]
    // arr[min_idx]
    l32 r16, [r7]

    // arr[i] < arr[min_idx]
    mov sr, 0
    cmp r15, r16
    bgt 1
    addi r7, r8, 1

    addi r8, r8, 1

    // REPEAT THE INTERATION
    bun -10

    mov sr, 0
    ret
  selectionSort:
    // R10 = HEAD ARRAY
    mov r10, numbers
    // 32-BIT ALIGNMENT
    divi r10, r10, 4

    // R20 = TAIL ARRAY (100 * 4 BYTES)
    addi r20, r10, 400

    // WHILE
    cmp r10, r20
    beq 5

    // RETURN R7 = MIN INDEX
    call findMinIndex

    // SET R3 = MIN INDEX
    addi r3, r7, 0
    call swap

    addi r10, r10, 1

    // REPEAT THE INTERATION
    bun -7

    mov sr, 0
    ret
	main:
    // SP = 32KiB
		mov sp, 0x7FFC

    // R1 = TERMINAL IN ADDRESS
    l32 r1, [terminalIn]

    // R2 = TERMINAL OUT ADDRESS
    l32 r2, [terminalOut]

    // PRINT HEADER INPUT
    mov r10, headerInput
    call printHeader
    call readTerminal
    call writeTerminal

    call selectionSort

    // PRINT HEADER SORTED NUMBERS
    mov r10, headerOutput
    call printHeader
    call writeTerminal

		int 0
.data
	numbers:
    .fill 100, 4, 0
	terminalIn:
    .4byte 0x8888888A
  terminalOut:
    .4byte 0x8888888B
  buffer:
    .fill 12, 1, 0
  headerInput:
    .asciz "Input numbers:\n"
  headerOutput:
    .asciz "\nSorted numbers:\n"
  space:
    .asciz " "
