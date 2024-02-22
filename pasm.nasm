// Segmento de código
.text
	init:
		bun main
		.align 5
    readTerminal:
    	// R10 = [unorderedNumbers] (ARRAY HEAD)
    	mov r10, unorderedNumbers
        // R11 = 0 (ASSIST WITH ADDRESS CALCULATION)
        mov r11, 0
    	// R12 = 0 (COUNTER)
    	mov r12, 0

      // START OF WHILE
      cmpi r12, 100
      beq 6
    	// R3 = 1 BYTE FROM TERMINAL
    	l8 r3, [r1]

      muli r11, r12, 4
      add r10, r10, r11
      addi r12, r12, 1

      // ADD TO THE NEXT VECTOR INDEX
      s32 [r10], r3

      // REPEAT THE INTERATION
      bun -8
      mov sr, 0
      ret
	main:
    // R10 = [unorderedNumbers] (ARRAY HEAD)
    mov r10, unorderedNumbers
    // R11 = 0 (COUNTER)
    mov r11, 0

    // START OF WHILE
    cmpi r11, 100
    beq 5
    // R3 = 1 BYTE FROM TERMINAL
    l8 r3, [r1]
    s32 [r10], r3
    muli r10, r10, 4
    addi r11, r11, 1
    // REPEAT THE INTERATION
    bun -7
    ret
.data
	unorderedNumbers:
    	.fill 100, 4, -1
	terminalIn:
    	.4byte 0x8888888A
  terminalOut:
    	.4byte 0x8888888B
