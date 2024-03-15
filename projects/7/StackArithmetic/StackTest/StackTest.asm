// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
AM=M-1
D=M
A=A-1
D=M-D
@EQ_TRUE0
D;JEQ
@SP
A=M-1
M=0
@EQ_END0
0;JMP
(EQ_TRUE0)
@SP
A=M-1
M=-1
(EQ_END0)
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
AM=M-1
D=M
A=A-1
D=M-D
@EQ_TRUE1
D;JEQ
@SP
A=M-1
M=0
@EQ_END1
0;JMP
(EQ_TRUE1)
@SP
A=M-1
M=-1
(EQ_END1)
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
AM=M-1
D=M
A=A-1
D=M-D
@EQ_TRUE2
D;JEQ
@SP
A=M-1
M=0
@EQ_END2
0;JMP
(EQ_TRUE2)
@SP
A=M-1
M=-1
(EQ_END2)
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@LT_TRUE0
D;JLT
@SP
A=M-1
M=0
@LT_END0
0;JMP
(LT_TRUE0)
@SP
A=M-1
M=-1
(LT_END0)
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@LT_TRUE1
D;JLT
@SP
A=M-1
M=0
@LT_END1
0;JMP
(LT_TRUE1)
@SP
A=M-1
M=-1
(LT_END1)
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@LT_TRUE2
D;JLT
@SP
A=M-1
M=0
@LT_END2
0;JMP
(LT_TRUE2)
@SP
A=M-1
M=-1
(LT_END2)
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@GT_TRUE0
D;JGT
@SP
A=M-1
M=0
@GT_END0
0;JMP
(GT_TRUE0)
@SP
A=M-1
M=-1
(GT_END0)
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@GT_TRUE1
D;JGT
@SP
A=M-1
M=0
@GT_END1
0;JMP
(GT_TRUE1)
@SP
A=M-1
M=-1
(GT_END1)
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
AM=M-1
D=M
A=A-1
D=M-D
@GT_TRUE2
D;JGT
@SP
A=M-1
M=0
@GT_END2
0;JMP
(GT_TRUE2)
@SP
A=M-1
M=-1
(GT_END2)
// push constant 57
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 31
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 53
@53
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
AM=M-1
D=M
A=A-1
M=M+D
// push constant 112
@112
D=A
@SP
A=M
M=D
@SP
M=M+1
// sub
@SP
AM=M-1
D=M
A=A-1
M=M-D
// neg
@SP
A=M-1
M=-M
// and
@SP
AM=M-1
D=M
A=A-1
M=M&D
// push constant 82
@82
D=A
@SP
A=M
M=D
@SP
M=M+1
// or
@SP
AM=M-1
D=M
A=A-1
M=M|D
// not
@SP
A=M-1
M=!M
