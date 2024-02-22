// Segmento de código
.text
	init:
		bun main
		.align 5
  readTerminal:
    // R10 = [unorderedNumbers] (ARRAY POINTER)
    mov r10, unorderedNumbers

    // R12 = 0 (COUNTER)
    mov r12, 0

    // START OF WHILE
    cmpi r12, 100
    beq 6

    // R3 = 1 BYTE FROM TERMINAL
    l8 r3, [r1]

    addi r10, r10, 1
    addi r12, r12, 1

    // CONFIGURE R4 TO RECEIVE THE CONVERTED NUMBER
    call convertStringToNumber
    s32 [r10], r4

    // REPEAT THE INTERATION
    bun -8

    mov sr, 0
    ret
  convertStringToNumber:
    // COMPARING WITH '\0'
    cmpi r3, 0
    beq 39

    // COMPARING WITH '0'
    cmpi r3, 48
    bne 2
    mov r4, 0
    // BREAK
    bun 35

    // COMPARING WITH '1'
    cmpi r3, 49
    bne 2
    mov r4, 1
    // BREAK
    bun 31

    // COMPARING WITH '2'
    cmpi r3, 50
    bne 2
    mov r4, 2
    // BREAK
    bun 27

    // COMPARING WITH '3'
    cmpi r3, 51
    bne 2
    mov r4, 3
    // BREAK
    bun 23

    // COMPARING WITH '4'
    cmpi r3, 52
    bne 2
    mov r4, 4
    // BREAK
    bun 19

    // COMPARING WITH '5'
    cmpi r3, 53
    bne 2
    mov r4, 5
    // BREAK
    bun 15

    // COMPARING WITH '6'
    cmpi r3, 54
    bne 2
    mov r4, 6
    // BREAK
    bun 11

    // COMPARING WITH '7'
    cmpi r3, 55
    bne 2
    mov r4, 7
    // BREAK
    bun 7

    // COMPARING WITH '8'
    cmpi r3, 56
    bne 2
    mov r4, 8
    // BREAK
    bun 3

    // COMPARING WITH '9'
    cmpi r3, 57
    bne 5
    mov r4, 9

    mov sr, 0
    ret
  printf:
    // R10 = [unorderedNumbers] (ARRAY POINTER)
    mov r10, unorderedNumbers

    // R11 = 0 (COUNTER)
    mov r11, 0

    // START OF WHILE
    cmpi r11, 100
    beq 8

    // READING BYTE FROM ARRAY
    l8 r3, [r10]

    // WRITE TO TERMINAL
    s8 [r2], r3

    // ADD SPACE CARACTER
    mov r3, space
    l8 r3, [r3]
    s8 [r2], r3

    addi r10, r10, 1
    addi r11, r11, 1

    // REPEAT THE INTERATION
    bun -10

    mov sr, 0
    ret
  orderNumbers:


    mov sr, 0
    ret
	main:
    // SP = 32KiB
		mov sp, 0x7FFC

    // R1 = TERMINAL IN ADDRESS
    l32 r1, [terminalIn]

    // R2 = TERMINAL OUT ADDRESS
    l32 r2, [terminalOut]

    call readTerminal
    call printf
    call orderNumbers

		int 0
.data
	unorderedNumbers:
    .fill 100, 4, -1
  orderedNumbers:
    .fill 100, 4, -1
	terminalIn:
    .4byte 0x8888888A
  terminalOut:
    .4byte 0x8888888B
  space:
    .asciz " "
