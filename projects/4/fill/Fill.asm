// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(RESTART)
@SCREEN
D=A
@address
M=D

@KBD
D=M
@BLACK
D;JGT
@WHITE
D;JEQ

@KBDCHEEK
0;JMP

(BLACK)
@flag
M=-1
@CHANGE
0;JMP

(WHITE)
@flag
M=0
@CHANGE
0;JMP

(CHANGE)
@flag
D=M
@address
A=M
M=D

@address
M=M+1
D=M
@KBD
D=A-D

@CHANGE
D;JGT

@RESTART
0;JMP
