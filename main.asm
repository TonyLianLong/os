	.file	"main.c"
	.intel_syntax noprefix
	.text
	.globl	panic
	.type	panic, @function
panic:
.LFB0:
	.cfi_startproc
	mov	BYTE PTR ds:753664, 69
	mov	BYTE PTR ds:753665, 7
	mov	al, BYTE PTR [rdi]
	mov	BYTE PTR ds:753667, 7
	mov	BYTE PTR ds:753666, al
#APP
# 28 "main.c" 1
	MOV DX,[0x463];MOV AL,0x0F
OUT DX,AL
PUSH DX
MOV DX,0x3D5
MOV AL,0x02
OUT DX,AL
POP DX
MOV AL,0x0E
OUT DX,AL
MOV DX,0x3D5
MOV AL,0x00
OUT DX,AL
# 0 "" 2
# 44 "main.c" 1
	CLI
HLT
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE0:
	.size	panic, .-panic
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"0:Can't get RAM size."
	.text
	.globl	get_RAM_size
	.type	get_RAM_size, @function
get_RAM_size:
.LFB1:
	.cfi_startproc
	movzx	r8d, WORD PTR ds:24573
	mov	r9b, BYTE PTR ds:24575
	xor	ecx, ecx
	mov	rsi, QWORD PTR RAMbase[rip]
	mov	rax, QWORD PTR MaxRAMAddr[rip]
	mov	edx, 20480
.L3:
	cmp	rdx, r8
	jae	.L15
	cmp	DWORD PTR [rdx+16], 1
	jne	.L4
	cmp	r9b, 24
	jne	.L5
	test	BYTE PTR [rdx+20], 1
	jne	.L6
.L5:
	mov	rdi, QWORD PTR [rdx+8]
	cmp	rdi, rcx
	jbe	.L4
	mov	rsi, QWORD PTR [rdx]
	mov	rcx, rdi
.L4:
	lea	rdi, [rsi+rcx]
	cmp	rdi, rax
	cmova	rax, rdi
.L6:
	add	rdx, 24
	jmp	.L3
.L15:
	test	rcx, rcx
	mov	QWORD PTR RAMlength[rip], rcx
	mov	QWORD PTR RAMbase[rip], rsi
	mov	QWORD PTR MaxRAMAddr[rip], rax
	jne	.L10
	mov	edi, OFFSET FLAT:.LC0
	call	panic
	or	eax, -1
.L10:
	ret
	.cfi_endproc
.LFE1:
	.size	get_RAM_size, .-get_RAM_size
	.section	.rodata.str1.1
.LC1:
	.string	"2:Too many characters are there on the screen."
	.text
	.globl	print_char
	.type	print_char, @function
print_char:
.LFB2:
	.cfi_startproc
	movzx	eax, WORD PTR cursor_offset[rip]
	mov	BYTE PTR [rax+753664+rax], dil
	movzx	eax, WORD PTR cursor_offset[rip]
	mov	BYTE PTR [rax+753665+rax], 7
	mov	ax, WORD PTR cursor_offset[rip]
	inc	eax
	cmp	ax, 2000
	je	.L17
	mov	WORD PTR cursor_offset[rip], ax
	ret
.L17:
	mov	edi, OFFSET FLAT:.LC1
	mov	WORD PTR cursor_offset[rip], 0
	jmp	panic
	.cfi_endproc
.LFE2:
	.size	print_char, .-print_char
	.globl	update_cursor
	.type	update_cursor, @function
update_cursor:
.LFB3:
	.cfi_startproc
	mov	ax, WORD PTR cursor_offset[rip]
#APP
# 88 "main.c" 1
	PUSH AX
		MOV DX,[0x463]
		MOV AL,0x0F
		OUT DX,AL
		POP AX
		PUSH DX
		MOV DX,0x3D5
		OUT DX,AL
		POP DX
		MOV AL,0x0E
		OUT DX,AL
		MOV DX,0x3D5
		MOV AL,AH
		OUT DX,AL
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE3:
	.size	update_cursor, .-update_cursor
	.globl	print_hex
	.type	print_hex, @function
print_hex:
.LFB4:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rdi
	push	rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	mov	ebx, 60
.L25:
	mov	rax, rbp
	mov	cl, bl
	shr	rax, cl
	and	eax, 15
	cmp	eax, 9
	lea	edi, [rax+48]
	jle	.L27
	add	eax, 87
	movzx	edi, al
.L27:
	sub	ebx, 4
	call	print_char
	cmp	ebx, -4
	jne	.L25
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	xor	eax, eax
	jmp	update_cursor
	.cfi_endproc
.LFE4:
	.size	print_hex, .-print_hex
	.globl	init_paging
	.type	init_paging, @function
init_paging:
.LFB5:
	.cfi_startproc
	cmp	QWORD PTR MaxRAMAddr[rip], 1073741823
	ja	.L34
	mov	QWORD PTR ds:1048576, 2142211
	mov	edx, 262144
	mov	esi, 2142208
.L30:
	lea	edi, [rdx-262144]
	xor	eax, eax
.L33:
	lea	ecx, [rdi+rax]
	lea	r8d, [rdx+rax]
	inc	rax
	sal	ecx, 21
	sal	r8d, 3
	or	cl, -93
	cmp	rax, 512
	mov	ecx, ecx
	mov	QWORD PTR [r8d], rcx
	jne	.L33
	lea	eax, [3+rdx*8]
	add	rsi, 8
	add	edx, 512
	mov	QWORD PTR [rsi-8], rax
	cmp	rsi, 2142288
	jne	.L30
#APP
# 143 "main.c" 1
	NOP
# 0 "" 2
#NO_APP
	mov	eax, 1048576
#APP
# 144 "main.c" 1
	MOV CR3,rax
# 0 "" 2
# 145 "main.c" 1
	HLT
# 0 "" 2
#NO_APP
	xor	eax, eax
	ret
.L34:
	or	eax, -1
	ret
	.cfi_endproc
.LFE5:
	.size	init_paging, .-init_paging
	.globl	init_interrupt
	.type	init_interrupt, @function
init_interrupt:
.LFB6:
	.cfi_startproc
#APP
# 149 "main.c" 1
	mov al,0b00000000#启动8259的所有中断
out 0x21,al#Master 8259A,OCW1
out	0xA1,al#Slave 8259A,OCW1
mov al,0x11
out 0x20,al#开始
mov al,0x20#偏移
out 0x21,al#ICW2
mov al,1<<2
out 0x21,al#ICW3
mov al,1
out 0x21,al#ICW4
mov al,0x11
out 0xA0,al
mov al,0x28
out 0xA1,al
mov al,1<<2
out 0xA1,al
mov al,1
out 0xA1,al

# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE6:
	.size	init_interrupt, .-init_interrupt
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB7:
	.cfi_startproc
	xor	eax, eax
	mov	WORD PTR cursor_offset[rip], 0
	call	get_RAM_size
	xor	eax, eax
	call	init_paging
	xor	eax, eax
	call	init_interrupt
	mov	edi, 65
	call	print_char
	mov	rdi, QWORD PTR RAMlength[rip]
	shr	rdi, 22
	add	edi, 48
	movsx	edi, dil
	call	print_char
	xor	eax, eax
	call	update_cursor
	xor	eax, eax
	ret
	.cfi_endproc
.LFE7:
	.size	main, .-main
	.comm	cursor_offset,2,2
	.comm	MaxRAMAddr,8,8
	.comm	RAMlength,8,8
	.comm	RAMbase,8,8
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
