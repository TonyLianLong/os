ORG 0x7C00
;entry_address是参数传入的，是入口的地址
; 描述符
; usage: Descriptor Base, Limit, Attr
;        Base:  dd
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3
 dw %2 & 0FFFFh    ; 段界限 1    (2 字节)
 dw %1 & 0FFFFh    ; 段基址 1    (2 字节)
 db (%1 >> 16) & 0FFh   ; 段基址 2    (1 字节)
 dw ((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) ; 属性 1 + 段界限 2 + 属性 2  (2 字节)
 db (%1 >> 24) & 0FFh   ; 段基址 3    (1 字节)
%endmacro ; 共 8 字节
VIDEO_MODE EQU 0x115
VIDEO_MODE_SPECIAL EQU 0x4000;线性地址
VBE_buffer EQU 0x700
VBE_RAM_addr EQU 0x900
[bits 16]
JMP short loader
nop
;Jump to loader
db 0x44, 0x4F, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x02, 0x10, 0x05, 0x00, 0x02, 0x00, 0x02, 
db 0x40, 0x0B, 0xF8, 0xF0, 0x00, 0x3D, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
db 0x00, 0x00, 0x00, 0x29, 0x34, 0x61, 0x53, 0x09, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 
db 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20
loader:
MOV AX,0
MOV SS,AX
MOV ES,AX
MOV SP,0x7C00
MOV AX,0x0600
MOV CX,0x0000
;MOV [0x5000],DL;在0x5000存DL，这是启动的设备号码
MOV DX,0x184F;DH 24 DL 79
MOV BH,0x08
INT 0x10
MOV BP,booting_text
MOV CH,0
MOV CL,booting_text_length
call print_text
MOV AL,[0x410]
AND AL,0x30
CMP AL,0x20
JNE not_supported
MOV AX,0
MOV DL,0
INT 0x13
JC read_disk_failed
MOV AX,0x7E0
MOV ES,AX
MOV AH,0x2
MOV AL,3
MOV BX,0x0
MOV CH,16
MOV CL,6
MOV DH,0
MOV DL,0
INT 0x13
JC read_disk_failed
MOV AX,0x900
MOV ES,AX
MOV AH,0x2
MOV AL,32
MOV BX,0
MOV CH,14
MOV CL,14
MOV DH,0
MOV DL,0
INT 0x13
JC read_disk_failed
;call return
MOV AX,0xD000
MOV ES,AX
MOV AH,0x2
MOV AL,32
MOV BX,0
MOV CH,15
MOV CL,10
MOV DH,0
MOV DL,0
INT 0x13
JC read_disk_failed
;获取内存大小
MOV AX,0
MOV ES,AX
MOV DI,0x5000
MOV EBX,0
get_size:
MOV EDX,0x534D4150
MOV EAX,0xE820
MOV ECX,24
INT 0x15
JC not_supported;获取内存大小失败了
CMP EBX,0
JE fin
ADD DI,24;不管每个项目的长度是20还是24，每个项目的空间都是24，后面的4字节未初始化
CMP DI,0x6000-24-3;防止写入数据超过0x5FFD
JA not_supported;JA是无符号大于
JMP get_size
fin:
MOV [0x5FFD],WORD DI
MOV [0x5FFF],BYTE CL;每一个项目大小
CLI
LGDT [GDTPointer]
IN AL,0x92
OR AL,0b10
OUT 0x92,AL
MOV EAX,CR0
OR EAX,1
MOV CR0,EAX
JMP DWORD SelectorC:LABEL_SEG_CODE32
not_supported:
MOV BP,not_supported_text
MOV CL,not_supported_text_length
call print_text
JMP loop
read_disk_failed:
MOV BP,read_disk_failed_text
MOV CL,read_disk_failed_text_length
call print_text
JMP loop
loop:
JMP loop
print_text:
;CX 字符串长度
;ES:BP 字符串地址
PUSHA
MOV AX,0x1301
;AH 0x13 写入
;AL 0x01 
MOV BX,0x0007
;BH 视频页
;BL 属性
MOV DH,[cursor_y]
MOV DL,0
INT 0x10
ADD BYTE [cursor_y],1
;call return
;如果想要光标在输出完后显示在新的一行，可以取消注释，如果有注释则是光标在下一次输出时显示在新的一行
POPA
ret
return:
PUSHA
MOV AH,0x0E
MOV AL,0x0D
MOV BH,0
MOV BL,0
INT 0x10
MOV AL,0x0A
INT 0x10
ADD BYTE [cursor_y],1
POPA
ret
print_text_no_return:
PUSH AX
PUSH BX
MOV AH,0x0E
MOV AL,byte [BX]
CMP byte AL,0
je print_finish_no_return
MOV BH,0
MOV BL,0
INT 0x10
POP BX
POP AX
ADD BX,1
JMP print_text_no_return
print_finish_no_return:
POP BX
POP AX
ret
[BITS 32]  
LABEL_SEG_CODE32:
MOV AX,SelectorData
MOV DS,AX
MOV SS,AX
MOV ES,AX
;MOV ESP,0x200000
;MOV EAX,[0x9000+0x8+0x4]
;ADD EAX,-5-0x9000
;MOV [0x9000+0x1],EAX
NOP
JMP DWORD SelectorC:entry_address
loop32:
HLT
JMP loop32
DA_32  EQU 4000h ; 32 位段
DA_C  EQU 98h ; 存在的只执行代码段属性值
DA_DRW  EQU 92h ; 存在的可读写数据段属性值
G_bit EQU 0x8000
SelectorData EQU LABEL_SEGMENTDATA-GDT_LABEL
SelectorC EQU LABEL_SEGMENTC-GDT_LABEL
GDT_LABEL:Descriptor 0,0,0
LABEL_SEGMENTC:Descriptor 0,0xFFFFF,DA_C+DA_32+G_bit
LABEL_SEGMENTDATA:Descriptor 0,0xFFFFF,DA_DRW+DA_32+G_bit
GdtLen equ $-GDT_LABEL
GDTPointer:dw GdtLen - 1
dd GDT_LABEL
not_supported_text:
db "Not supported."
not_supported_text_length EQU $-not_supported_text
read_disk_failed_text:
db "Read disk failed."
read_disk_failed_text_length EQU $-read_disk_failed_text
booting_text:
db "Booting..."
booting_text_length EQU $-booting_text
cursor_y:db 0
times 510-($-$$) db 0
dw 0xAA55