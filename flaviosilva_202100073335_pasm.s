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

    call writeTerminal

    addi r10, r10, 1
    addi r11, r11, 1

    // REPEAT THE INTERATION
    bun -8

    mov sr, 0
    ret
  convertIntToAscii:
    // COMPARING WITH '\0'
    cmpi r3, 0
    beq 39

    // SET '0' IN ASCII
    cmpi r3, 0
    bne 2
    mov r4, 48
    // BREAK
    bun 35

    // SET '1' IN ASCII
    cmpi r3, 1
    bne 2
    mov r4, 49
    // BREAK
    bun 31

    // SET '2' IN ASCII
    cmpi r3, 2
    bne 2
    mov r4, 50
    // BREAK
    bun 27

    // SET '3' IN ASCII
    cmpi r3, 3
    bne 2
    mov r4, 51
    // BREAK
    bun 23

    // SET '4' IN ASCII
    cmpi r3, 4
    bne 2
    mov r4, 52
    // BREAK
    bun 19

    // SET '5' IN ASCII
    cmpi r3, 5
    bne 2
    mov r4, 53
    // BREAK
    bun 15

    // SET '6' IN ASCII
    cmpi r3, 6
    bne 2
    mov r4, 54
    // BREAK
    bun 11

    // SET '7' IN ASCII
    cmpi r3, 7
    bne 2
    mov r4, 55
    // BREAK
    bun 7

    // SET '8' IN ASCII
    cmpi r3, 8
    bne 2
    mov r4, 56
    // BREAK
    bun 3

    // SET '9' IN ASCII
    cmpi r3, 9
    bne 1
    mov r4, 57

    mov sr, 0
    ret
  writeTerminal:
    // CONFIGURE R4 TO RECEIVE THE CONVERTED NUMBER
    call convertIntToAscii

    // WRITE TO TERMINAL
    s8 [r2], r4

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
  headerInput:
    .asciz "Input numbers:\n"
  headerOutput:
    .asciz "Sorted numbers:\n"
  space:
    .asciz " "
