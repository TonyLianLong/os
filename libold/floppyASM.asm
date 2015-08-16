global floppy_interrupt
extern floppy_exec
floppy_interrupt:
PUSH DS
PUSH ES
PUSH ESP
PUSH EBP
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
CALL floppy_exec
MOV AL,0x20
OUT 0x20,AL
OUT 0x0A0,AL
POPAD
POP EBP
POP ESP
POP ES
POP DS
iretd