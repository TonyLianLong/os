[BITS 32]
global entry
extern main
entry:
pushfd;保存EFLAGS
MOV EAX,[ESP]
XOR DWORD [ESP], 0x200000
popfd;修改EFLAGS的位
pushfd
POP EBX
CMP EAX,EBX
MOV CL,'1'
JE cpuid_not_support;不支持CPUID
MOV EAX, 0x80000000;获取最大功能ID
CPUID
MOV CL,'2'
CMP EAX,0x80000001
JB cpuid_not_support;不支持扩展查询功能
MOV EAX,0x80000001
CPUID
MOV CL,'3'
AND EDX,1<<29;测试长模式位 wiki cpuid
JE cpuid_not_support;不支持长模式
;汇编语言和C语言指针的运算是不一样的
;所有的页表（页目录表 3级指针 4级指针等）的空位（没有映射的选择项）不一定为0 可能是任意的内容 没有初始化
MOV EDI,0x1000
MOV CR3,EDI
MOV DWORD [EDI],0x00002003      ; Set the uint32_t at the destination index to 0x2003.
MOV DWORD [EDI+4],0x00000000
MOV EDI,0x2000
MOV DWORD [EDI],0x00003003      ; Set the uint32_t at the destination index to 0x3003.
MOV DWORD [EDI+4],0x00000000
MOV EDI,0x3000
MOV DWORD [EDI],0x00004003      ; Set the uint32_t at the destination index to 0x4003.
MOV DWORD [EDI+4],0x00000000
ADD EDI, 0x1000              ; Add 0x1000 to the destination index.
MOV EBX, 0x00000003          ; Set the B-register to 0x00000003.
MOV ECX, 512                 ; Set the C-register to 512.
SetPT:
MOV DWORD [EDI],EBX        ; Set the uint32_t at the destination index to the B-register.
MOV DWORD [EDI+4],0x00000000
ADD EDI,8
ADD EBX,0x1000              ; Add 0x1000 to the B-register.
LOOP SetPT           ; Set the next entry.
MOV EAX,CR4                 ; Set the A-register to control register 4.
OR EAX,1<<5               ; Set the PAE-bit, which is the 6th bit (bit 5).
MOV CR4,EAX                 ; Set control register 4 to the A-register.
MOV ECX, 0xC0000080
RDMSR
OR EAX,1<<8
WRMSR
MOV EAX,CR0                ; Set the A-register to control register 0.
OR EAX,1<<31              ; Set the PG-bit, which is the 32nd bit (bit 31).
MOV CR0, EAX                 ; Set control register 0 to the A-register.
LGDT [GDTPointer]
jmp CodeSeg:LongMain
GDT64:
dw 0
dw 0
db 0
db 0
db 0
db 0
CodeSeg: equ $ - GDT64; The code descriptor.
dw 0                         ; Limit (low).
dw 0                         ; Base (low).
db 0                         ; Base (middle)
db 10011010b                 ; Access (exec/read).
db 00100000b                 ; Granularity.
db 0                         ; Base (high).
DataSeg: equ $ - GDT64; The data descriptor.
dw 0                         ; Limit (low).
dw 0                         ; Base (low).
db 0                         ; Base (middle)
db 10010010b                 ; Access (read/write).
db 00000000b                 ; Granularity.
db 0                         ; Base (high).
GDTPointer:                    ; The GDT-pointer.
dw $ - GDT64 - 1             ; Limit.
dq GDT64                     ; Base.
[BITS 64]
LongMain:
MOV ECX, 0xC0000080
test_long_active:
RDMSR
AND EAX,1<<10
CMP EAX,0
JZ test_long_active
;需要切换到长模式LME才会是1（GDT要是长模式） EFER.LMA = 1, CR0.PE=1, CS.L=1
MOV AX,DataSeg
MOV DS,AX
MOV ES, AX
MOV FS, AX
MOV GS, AX
MOV SS, AX
MOV EDI, 0xB8000
MOV RAX, 0x0720072007200720
MOV ECX, 480
REP STOSQ
;把光标移到左上角
MOV DX,[0x463];0x463是BIOS DATA中的显示端口储存的地址
MOV AL,0x0F
OUT DX,AL
PUSH DX
MOV DX,0x3D5
MOV AL,0x00
OUT DX,AL;光标的低位
POP DX
MOV AL,0x0E
OUT DX,AL
MOV DX,0x3D5
MOV AL,0x00
OUT DX,AL;光标的高位
CALL main;这是入口
CLI
HLT
cpuid_not_support:;CL显示错误号
MOV [DWORD 0xB8000],BYTE 'E'
MOV [DWORD 0xB8001],BYTE 0x07
MOV [DWORD 0xB8002],BYTE ':'
MOV [DWORD 0xB8003],BYTE 0x07
MOV [DWORD 0xB8004],CL
MOV [DWORD 0xB8005],BYTE 0x07
HLT;无中断所以可以不用JMP