global _init_interrupt
global empty_interrupt
global timer_interrupt
global keyboard_interrupt
global mouse_interrupt
global enable_interrupt
global soft_interrupt
global trap
global trap2
extern timer_exec
extern keyboard_exec
extern mouse_exec
extern soft_interrupt_exec
extern trap_exec
extern trap2_exec
extern change_task
extern task_is_ready
IDT_base_address EQU 0x200000
_init_interrupt:
PUSH AX
mov	al,0b00000000;Enable interrupts of the master 8259A
out 0x21,al;Master 8259A,OCW1
mov	al,0b00000000;Enable interrupts of the slave 8259A
out	0xA1,al;Slave 8259A,OCW1
mov al,0x11
out 0x20,al;ICW1
mov al,0x30
out 0x21,al;ICW2
mov al,1<<2
out 0x21,al;ICW3
mov al,1
out 0x21,al;ICW4
mov al,0x11
out 0xA0,al
mov al,0x38
out 0xA1,al
mov al,1<<2
out 0xA1,al
mov al,1
out 0xA1,al
POP AX
LIDT [IDT_Pointer]
ret
trap:
PUSH DS
PUSH ES
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
PUSH ESP
CALL trap_exec
POP ESP
POPAD
POP ES
POP DS
iret
trap2:
PUSH DS
PUSH ES
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
PUSH ESP
CALL trap2_exec
POP ESP
POPAD
POP ES
POP DS
iret
empty_interrupt:
PUSH AX
MOV AL,0x20
out 0x20,AL
out 0x0A0,AL
POP AX
iret
timer_interrupt:
PUSH DS
PUSH ES
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
CALL timer_exec
MOV AL,0x20
OUT 0x20,AL
OUT 0x0A0,AL
MOV AL,[task_is_ready]
CMP AL,0
JE timer_next
CALL change_task
timer_next:
POPAD
POP ES
POP DS
iretd
keyboard_interrupt:
PUSH DS
PUSH ES
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
CALL keyboard_exec
MOV AL,0x20
OUT 0x20,AL
OUT 0x0A0,AL
POPAD
POP ES
POP DS
iretd
mouse_interrupt:
PUSH DS
PUSH ES
PUSHAD
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
CALL mouse_exec
MOV AL,0x20
OUT 0x20,AL
OUT 0x0A0,AL
POPAD
POP ES
POP DS
iretd
soft_interrupt:
PUSH DS
PUSH ES
PUSH EBX
PUSH ECX
PUSH EDX
PUSH EBP
PUSH ESI
PUSH EDI
PUSH EAX
MOV AX,0x10
MOV DS,AX
MOV ES,AX
MOV SS,AX
POP EAX
CALL soft_interrupt_exec
POP EDI
POP ESI
POP EBP
POP EDX
POP ECX
POP EBX
POP ES
POP DS
PUSH EAX
MOV AL,0x20
OUT 0x20,AL
OUT 0x0A0,AL
POP EAX
iretd
IDT_Pointer:
dw 0x800-1
dd IDT_base_address
