ORG 0x7C00
;entry_address�ǲ�������ģ�����ڵĵ�ַ
; ������
; usage: Descriptor Base, Limit, Attr
;        Base:  dd
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3
 dw %2 & 0FFFFh    ; �ν��� 1    (2 �ֽ�)
 dw %1 & 0FFFFh    ; �λ�ַ 1    (2 �ֽ�)
 db (%1 >> 16) & 0FFh   ; �λ�ַ 2    (1 �ֽ�)
 dw ((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) ; ���� 1 + �ν��� 2 + ���� 2  (2 �ֽ�)
 db (%1 >> 24) & 0FFh   ; �λ�ַ 3    (1 �ֽ�)
%endmacro ; �� 8 �ֽ�
VIDEO_MODE EQU 0x115
VIDEO_MODE_SPECIAL EQU 0x4000;���Ե�ַ
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
;MOV [0x5000],DL;��0x5000��DL�������������豸����
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
;��ȡ�ڴ��С
MOV AX,0
MOV ES,AX
MOV DI,0x5000
MOV EBX,0
get_size:
MOV EDX,0x534D4150
MOV EAX,0xE820
MOV ECX,24
INT 0x15
JC not_supported;��ȡ�ڴ��Сʧ����
CMP EBX,0
JE fin
ADD DI,24;����ÿ����Ŀ�ĳ�����20����24��ÿ����Ŀ�Ŀռ䶼��24�������4�ֽ�δ��ʼ��
CMP DI,0x6000-24-3;��ֹд�����ݳ���0x5FFD
JA not_supported;JA���޷��Ŵ���
JMP get_size
fin:
MOV [0x5FFD],WORD DI
MOV [0x5FFF],BYTE CL;ÿһ����Ŀ��С
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
;CX �ַ�������
;ES:BP �ַ�����ַ
PUSHA
MOV AX,0x1301
;AH 0x13 д��
;AL 0x01 
MOV BX,0x0007
;BH ��Ƶҳ
;BL ����
MOV DH,[cursor_y]
MOV DL,0
INT 0x10
ADD BYTE [cursor_y],1
;call return
;�����Ҫ�������������ʾ���µ�һ�У�����ȡ��ע�ͣ������ע�����ǹ������һ�����ʱ��ʾ���µ�һ��
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
DA_32  EQU 4000h ; 32 λ��
DA_C  EQU 98h ; ���ڵ�ִֻ�д��������ֵ
DA_DRW  EQU 92h ; ���ڵĿɶ�д���ݶ�����ֵ
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