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
    beq 6

    // R3 = 1 BYTE FROM TERMINAL
    l8 r3, [r1]
    s8 [r10], r3

    addi r10, r10, 1
    addi r11, r11, 1

    call writeTerminal

    // REPEAT THE INTERATION
    bun -8

    mov sr, 0
    ret
  convertNumberToAscii:
    call cleanBuffer
    mov r4, buffer
    // DIVIDE THE NUMBER BY 10
    mov r8, 10

    //GETTING THE DIGIT 0
    mov r9, zero
    l8 r12, [r9]

    // START OF WHILE
    // DIVIDE THE NUMBER BY 10, R6 CONTAINS THE WHOLE PART, R5 THE REMAINDER
    div r5, r6, r3, r8
    // CHECKS IF THE INTEGER PART IS ZERO (END OF CONVERSION)
    cmpi r6, 0
    beq 5

    // CONVERTS THE DIGIT TO ASCII BY ADDING THE ASCII VALUE OF '0'
    add r7, r5, r12
    // STORE THE ASCII DIGIT IN THE BUFFER
    s8 [r4], r7

    addi r4, r4, 1
    addi r3, r6, 0
    bun -8

    mov sr, 0
    ret
  writeTerminal:
    call convertNumberToAscii

    // CONFIGURE R4 TO RECEIVE THE CONVERTED NUMBER
    mov r4, buffer
    l8 r3, [r4]
    // Comparando com '\0'
    cmpi r3, 0
    beq 3

    // WRITE TO TERMINAL
    s8 [r2], r3
    addi r4, r4, 1
    bun -6

    // ADD SPACE EVERY 4 BYTES
    mov sr, 0
    modi r12, r11, 4
    cmpi r12, 0
    bne 3
    mov r4, space
    l8 r4, [r4]
    s8 [r2], r4

    mov sr, 0
    ret
  cleanBuffer:
    mov r4, buffer
    mov r5, 0
    s8 [r4], r5
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
  orderNumbers:
    // HEADER ARRAY
    mov r10, numbers

    // I ARRAY
    mov r11, 0

    // WHILE
    cmpi r10, 100
    beq 2
    // I LESS
    l32 r12, [numbers]

    // WHILE
    mov r14, 0
    addi r14, r10, 1
    cmp r14, r10
    beq 2

    cmp r14, r12
    bgt 2
    mov r12, r11

    addi r14, r14, 1

    addi r11, r11, 1

    // REPEAT THE INTERATION
    bun -14

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

    // PRINT HEADER SORTED NUMBERS
    mov r10, headerOutput
    call printHeader

		int 0
.data
	numbers:
    .fill 100, 4, -1
	terminalIn:
    .4byte 0x8888888A
  terminalOut:
    .4byte 0x8888888B
  zero:
    .asciz "0"
  buffer:
    .zero 1
  headerInput:
    .asciz "Input numbers:\n"
  headerOutput:
    .asciz "\nSorted numbers:\n"
  space:
    .asciz " "
