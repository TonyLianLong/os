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
# 25 "main.c" 1
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
# 41 "main.c" 1
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
# 85 "main.c" 1
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
	movabs	rbp, -3989547408
	push	rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	mov	ebx, 60
.L25:
	mov	rdi, rbp
	mov	cl, bl
	shr	rdi, cl
	and	edi, 15
	cmp	edi, 9
	jg	.L22
	add	edi, 48
	jmp	.L27
.L22:
	add	edi, 87
	movzx	edi, dil
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
	mov	QWORD PTR PML4T[rip], 65536
	mov	QWORD PTR PDPT[rip], 69632
	xor	eax, eax
	mov	QWORD PTR PDT[rip], 73728
	mov	QWORD PTR PT[rip], 77824
	mov	QWORD PTR ds:65536, 69635
	mov	QWORD PTR ds:69632, 73731
	mov	QWORD PTR ds:73728, 77827
.L30:
	mov	edx, eax
	sal	edx, 12
	or	edx, 3
	movsx	rdx, edx
	mov	QWORD PTR [77824+rax*8], rdx
	inc	rax
	cmp	rax, 512
	jne	.L30
	mov	eax, 65536
#APP
# 139 "main.c" 1
	MOV CR3,RAX
# 0 "" 2
#NO_APP
	xor	eax, eax
	ret
	.cfi_endproc
.LFE5:
	.size	init_paging, .-init_paging
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	xor	eax, eax
	mov	WORD PTR cursor_offset[rip], 0
	call	get_RAM_size
	xor	eax, eax
	call	init_paging
	mov	edi, 65
	call	print_char
	mov	rdi, QWORD PTR RAMlength[rip]
	shr	rdi, 22
	add	edi, 48
	movsx	edi, dil
	call	print_char
	xor	eax, eax
	call	update_cursor
	mov	edi, 1
	call	print_hex
	xor	eax, eax
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.comm	cursor_offset,2,2
	.comm	MaxRAMAddr,8,8
	.comm	RAMlength,8,8
	.comm	RAMbase,8,8
	.comm	PT,8,8
	.comm	PDT,8,8
	.comm	PDPT,8,8
	.comm	PML4T,8,8
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
