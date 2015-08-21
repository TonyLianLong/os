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
# 43 "main.c" 1
	MOV DX,[0x463]
MOV AL,0x0F
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
# 59 "main.c" 1
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
	.string	"2:Too many characters are there on the screen."
	.text
	.globl	print_char
	.type	print_char, @function
print_char:
.LFB3:
	.cfi_startproc
	cmp	dil, 10
	jne	.L3
	mov	ax, WORD PTR cursor_offset[rip]
	mov	esi, 80
	xor	edx, edx
	lea	ecx, [rax+80]
	div	si
	sub	ecx, edx
	mov	WORD PTR cursor_offset[rip], cx
	ret
.L3:
	cmp	dil, 13
	je	.L2
	movzx	eax, WORD PTR cursor_offset[rip]
	mov	BYTE PTR [rax+753664+rax], dil
	movzx	eax, WORD PTR cursor_offset[rip]
	mov	BYTE PTR [rax+753665+rax], 7
	mov	ax, WORD PTR cursor_offset[rip]
	inc	eax
	cmp	ax, 2000
	je	.L6
	mov	WORD PTR cursor_offset[rip], ax
	ret
.L6:
	mov	edi, OFFSET FLAT:.LC0
	mov	WORD PTR cursor_offset[rip], 0
	jmp	panic
.L2:
	ret
	.cfi_endproc
.LFE3:
	.size	print_char, .-print_char
	.globl	update_cursor
	.type	update_cursor, @function
update_cursor:
.LFB4:
	.cfi_startproc
	mov	ax, WORD PTR cursor_offset[rip]
#APP
# 91 "main.c" 1
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
.LFE4:
	.size	update_cursor, .-update_cursor
	.globl	print_str
	.type	print_str, @function
print_str:
.LFB5:
	.cfi_startproc
	push	rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	mov	rbx, rdi
.L11:
	movsx	edi, BYTE PTR [rbx]
	test	dil, dil
	je	.L14
	inc	rbx
	call	print_char
	jmp	.L11
.L14:
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	xor	eax, eax
	jmp	update_cursor
	.cfi_endproc
.LFE5:
	.size	print_str, .-print_str
	.globl	print_hex
	.type	print_hex, @function
print_hex:
.LFB6:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rdi
	push	rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	mov	ebx, 60
.L19:
	mov	rax, rbp
	mov	cl, bl
	shr	rax, cl
	and	eax, 15
	cmp	eax, 9
	lea	edi, [rax+48]
	jle	.L21
	add	eax, 87
	movzx	edi, al
.L21:
	sub	ebx, 4
	call	print_char
	cmp	ebx, -4
	jne	.L19
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	xor	eax, eax
	jmp	update_cursor
	.cfi_endproc
.LFE6:
	.size	print_hex, .-print_hex
	.globl	init_mem_table
	.type	init_mem_table, @function
init_mem_table:
.LFB7:
	.cfi_startproc
	mov	rax, QWORD PTR MemoryTableAddr[rip]
	mov	rdx, QWORD PTR RAMbase[rip]
	mov	QWORD PTR mem_table[rip], rax
	mov	QWORD PTR [rax], rdx
	mov	rdx, QWORD PTR RAMlength[rip]
	mov	QWORD PTR [rax+16], 1
	mov	QWORD PTR [rax+24], 0
	mov	QWORD PTR [rax+8], rdx
	lea	rdx, [rax+2016]
.L24:
	mov	QWORD PTR [rax+48], 0
	mov	QWORD PTR [rax+32], 0
	add	rax, 32
	mov	QWORD PTR [rax+8], 0
	mov	QWORD PTR [rax+24], 0
	cmp	rax, rdx
	jne	.L24
	xor	eax, eax
	ret
	.cfi_endproc
.LFE7:
	.size	init_mem_table, .-init_mem_table
	.section	.rodata.str1.1
.LC1:
	.string	"0:Uninited entry in linktable."
.LC2:
	.string	"5:Memory table is full."
.LC3:
	.string	"4:The number of the entries is greater than the max number."
	.text
	.globl	malloc
	.type	malloc, @function
malloc:
.LFB9:
	.cfi_startproc
	mov	rdx, QWORD PTR mem_table[rip]
	mov	esi, 64
	mov	edi, edi
#APP
# 63 "main.c" 1
	pushfq
	popq rax
	and rax,(1<<9)
	cli
# 0 "" 2
#NO_APP
	mov	QWORD PTR interrupt_flag[rip], rax
	mov	rax, rdx
.L35:
	mov	rcx, QWORD PTR [rax+16]
	test	cl, 1
	jne	.L26
	mov	edi, OFFSET FLAT:.LC1
	jmp	.L37
.L26:
	test	cl, 2
	jne	.L28
	mov	r8, QWORD PTR [rax+8]
	cmp	r8, rdi
	jbe	.L29
	add	rdx, 32
	mov	esi, 63
.L32:
	test	BYTE PTR [rdx+16], 1
	je	.L30
	add	rdx, 32
	dec	esi
	jne	.L32
	mov	edi, OFFSET FLAT:.LC2
	jmp	.L37
.L29:
	jne	.L28
	or	rcx, 2
	mov	QWORD PTR [rax+16], rcx
	jmp	.L39
.L28:
	mov	rax, QWORD PTR [rax+24]
	test	rax, rax
	je	.L38
	dec	esi
	jne	.L35
	mov	edi, OFFSET FLAT:.LC3
	jmp	.L37
.L30:
	or	rcx, 2
	sub	r8, rdi
	mov	QWORD PTR [rax+16], rcx
	mov	rcx, rdi
	add	rcx, QWORD PTR [rax]
	or	QWORD PTR [rdx+16], 1
	mov	QWORD PTR [rdx+8], r8
	mov	QWORD PTR [rdx], rcx
	mov	rcx, QWORD PTR [rax+24]
	mov	QWORD PTR [rax+8], rdi
	mov	QWORD PTR [rdx+24], rcx
	mov	QWORD PTR [rax+24], rdx
.L39:
	mov	rdx, QWORD PTR interrupt_flag[rip]
#APP
# 69 "main.c" 1
	pushfq
	or QWORD[esp],rdx
	popf
# 0 "" 2
#NO_APP
	mov	rax, QWORD PTR [rax]
	ret
.L37:
	call	panic
.L38:
	mov	rax, QWORD PTR interrupt_flag[rip]
#APP
# 69 "main.c" 1
	pushfq
	or QWORD[esp],rax
	popf
# 0 "" 2
#NO_APP
	xor	eax, eax
	ret
	.cfi_endproc
.LFE9:
	.size	malloc, .-malloc
	.section	.rodata.str1.1
.LC4:
	.string	"6:There are entries which are uninited in the linktable."
.LC5:
	.string	"9:No entry which starts at this address."
.LC6:
	.string	"8:You can't free the kernel entry."
.LC7:
	.string	"7:Try to free unused item."
.LC8:
	.string	"4:Entries' number is greater than the max number."
	.text
	.globl	free
	.type	free, @function
free:
.LFB10:
	.cfi_startproc
	push	r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
#APP
# 63 "main.c" 1
	pushfq
	popq rax
	and rax,(1<<9)
	cli
# 0 "" 2
#NO_APP
	push	r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	test	rdi, rdi
	mov	r12, rdi
	mov	QWORD PTR interrupt_flag[rip], rax
	push	rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	push	rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	je	.L56
	mov	rbx, QWORD PTR mem_table[rip]
	xor	ebp, ebp
	xor	r13d, r13d
.L47:
	test	BYTE PTR [rbx+16], 1
	jne	.L43
	mov	edi, OFFSET FLAT:.LC4
	call	panic
.L43:
	cmp	r12, QWORD PTR [rbx]
	je	.L44
	mov	rax, QWORD PTR [rbx+24]
	test	rax, rax
	jne	.L45
	mov	edi, OFFSET FLAT:.LC5
	jmp	.L54
.L45:
	inc	r13d
	cmp	r13d, 64
	je	.L46
	mov	rbp, rbx
	mov	rbx, rax
	jmp	.L47
.L44:
	test	rbp, rbp
	mov	edi, OFFSET FLAT:.LC6
	je	.L54
	test	BYTE PTR [rbx+16], 2
	mov	edi, OFFSET FLAT:.LC7
	je	.L54
	mov	rax, QWORD PTR [rbx+24]
	test	BYTE PTR [rax+16], 2
	je	.L50
	mov	QWORD PTR [rbx+16], 1
	jmp	.L51
.L50:
	mov	rdx, QWORD PTR [rbx+8]
	add	r12, rdx
	cmp	QWORD PTR [rax], r12
	jne	.L51
	add	rdx, QWORD PTR [rax+8]
	mov	QWORD PTR [rbx+16], 1
	mov	QWORD PTR [rbx+8], rdx
	mov	rdx, QWORD PTR [rax+24]
	mov	QWORD PTR [rax+16], 0
	mov	QWORD PTR [rax], 0
	mov	QWORD PTR [rax+8], 0
	mov	QWORD PTR [rax+24], 0
	mov	QWORD PTR [rbx+24], rdx
.L51:
	test	BYTE PTR [rbp+16], 2
	jne	.L55
	mov	rax, QWORD PTR [rbp+8]
	mov	rdx, rax
	add	rdx, QWORD PTR [rbp+0]
	cmp	QWORD PTR [rbx], rdx
	jne	.L55
	add	rax, QWORD PTR [rbx+8]
	mov	QWORD PTR [rbp+8], rax
	mov	rax, QWORD PTR [rbx+24]
	mov	QWORD PTR [rbx+16], 0
	mov	QWORD PTR [rbp+24], rax
	mov	QWORD PTR [rbx], 0
	mov	QWORD PTR [rbx+8], 0
	mov	QWORD PTR [rbx+24], 0
	jmp	.L55
.L46:
	mov	edi, OFFSET FLAT:.LC8
.L54:
	call	panic
.L55:
	mov	rax, QWORD PTR interrupt_flag[rip]
.L56:
#APP
# 69 "main.c" 1
	pushfq
	or QWORD[esp],rax
	popf
# 0 "" 2
#NO_APP
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 32
	pop	rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 24
	pop	r12
	.cfi_restore 12
	.cfi_def_cfa_offset 16
	pop	r13
	.cfi_restore 13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10:
	.size	free, .-free
	.section	.rodata.str1.1
.LC9:
	.string	"Memory table entry read number is too big,change to the max number."
.LC10:
	.string	"Memory Table Number:"
.LC11:
	.string	" Start:"
.LC12:
	.string	" Length:"
.LC13:
	.string	" Flag:"
	.text
	.globl	print_mem_table
	.type	print_mem_table, @function
print_mem_table:
.LFB11:
	.cfi_startproc
	push	r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
#APP
# 63 "main.c" 1
	pushfq
	popq rax
	and rax,(1<<9)
	cli
# 0 "" 2
#NO_APP
	push	rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	cmp	edi, 64
	mov	ebp, edi
	mov	QWORD PTR interrupt_flag[rip], rax
	push	rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	jbe	.L58
	mov	edi, OFFSET FLAT:.LC9
	call	print_str
.L58:
	lea	eax, [rbp-1]
	cmp	eax, 64
	mov	eax, 64
	cmovae	ebp, eax
	xor	ebx, ebx
.L61:
	mov	edi, OFFSET FLAT:.LC10
	mov	r12, rbx
	call	print_str
	mov	rdi, rbx
	sal	r12, 5
	inc	rbx
	call	print_hex
	mov	edi, OFFSET FLAT:.LC11
	call	print_str
	mov	rax, QWORD PTR mem_table[rip]
	mov	rdi, QWORD PTR [rax+r12]
	call	print_hex
	mov	edi, OFFSET FLAT:.LC12
	call	print_str
	mov	rax, QWORD PTR mem_table[rip]
	mov	rdi, QWORD PTR [rax+8+r12]
	call	print_hex
	mov	edi, OFFSET FLAT:.LC13
	call	print_str
	mov	rax, QWORD PTR mem_table[rip]
	mov	rdi, QWORD PTR [rax+16+r12]
	call	print_hex
	mov	edi, 10
	call	print_char
	cmp	ebp, ebx
	ja	.L61
	mov	rax, QWORD PTR interrupt_flag[rip]
#APP
# 69 "main.c" 1
	pushfq
	or QWORD[esp],rax
	popf
# 0 "" 2
#NO_APP
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 24
	pop	rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 16
	pop	r12
	.cfi_restore 12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11:
	.size	print_mem_table, .-print_mem_table
	.globl	print_mem_table_link
	.type	print_mem_table_link, @function
print_mem_table_link:
.LFB12:
	.cfi_startproc
	push	r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	mov	r12d, edi
#APP
# 63 "main.c" 1
	pushfq
	popq rax
	and rax,(1<<9)
	cli
# 0 "" 2
#NO_APP
	push	rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	cmp	edi, 64
	mov	QWORD PTR interrupt_flag[rip], rax
	push	rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	jbe	.L65
	mov	edi, OFFSET FLAT:.LC9
	call	print_str
.L65:
	lea	eax, [r12-1]
	mov	rbx, QWORD PTR mem_table[rip]
	cmp	eax, 64
	mov	eax, 64
	cmovae	r12d, eax
	xor	ebp, ebp
.L70:
	mov	edi, OFFSET FLAT:.LC10
	call	print_str
	mov	rdi, rbp
	call	print_hex
	mov	edi, OFFSET FLAT:.LC11
	call	print_str
	mov	rdi, QWORD PTR [rbx]
	call	print_hex
	mov	edi, OFFSET FLAT:.LC12
	call	print_str
	mov	rdi, QWORD PTR [rbx+8]
	call	print_hex
	mov	edi, OFFSET FLAT:.LC13
	call	print_str
	mov	rdi, QWORD PTR [rbx+16]
	call	print_hex
	mov	edi, 10
	call	print_char
	mov	rbx, QWORD PTR [rbx+24]
	test	rbx, rbx
	je	.L71
	lea	eax, [rbp+1]
	inc	rbp
	cmp	r12d, ebp
	ja	.L70
	cmp	eax, 64
	jne	.L71
	mov	edi, OFFSET FLAT:.LC8
	call	panic
.L71:
	mov	rax, QWORD PTR interrupt_flag[rip]
#APP
# 69 "main.c" 1
	pushfq
	or QWORD[esp],rax
	popf
# 0 "" 2
#NO_APP
	pop	rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 24
	pop	rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 16
	pop	r12
	.cfi_restore 12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE12:
	.size	print_mem_table_link, .-print_mem_table_link
	.section	.rodata.str1.1
.LC14:
	.string	"0:Can't get RAM size."
	.text
	.globl	get_RAM_size
	.type	get_RAM_size, @function
get_RAM_size:
.LFB13:
	.cfi_startproc
	movzx	r8d, WORD PTR ds:24573
	mov	r9b, BYTE PTR ds:24575
	xor	ecx, ecx
	mov	rsi, QWORD PTR RAMbase[rip]
	mov	rax, QWORD PTR MaxRAMAddr[rip]
	mov	edx, 20480
.L75:
	cmp	rdx, r8
	jae	.L86
	cmp	DWORD PTR [rdx+16], 1
	jne	.L76
	cmp	r9b, 24
	jne	.L77
	test	BYTE PTR [rdx+20], 1
	jne	.L78
.L77:
	mov	rdi, QWORD PTR [rdx+8]
	cmp	rdi, rcx
	jbe	.L76
	mov	rsi, QWORD PTR [rdx]
	mov	rcx, rdi
.L76:
	lea	rdi, [rsi+rcx]
	cmp	rdi, rax
	cmova	rax, rdi
.L78:
	add	rdx, 24
	jmp	.L75
.L86:
	test	rcx, rcx
	mov	QWORD PTR RAMlength[rip], rcx
	mov	QWORD PTR RAMbase[rip], rsi
	mov	QWORD PTR MaxRAMAddr[rip], rax
	jne	.L82
	mov	edi, OFFSET FLAT:.LC14
	call	panic
	or	eax, -1
.L82:
	ret
	.cfi_endproc
.LFE13:
	.size	get_RAM_size, .-get_RAM_size
	.globl	init_paging
	.type	init_paging, @function
init_paging:
.LFB14:
	.cfi_startproc
	mov	rdx, QWORD PTR MaxRAMAddr[rip]
	mov	eax, 1
	cmp	rdx, 1073741823
	jbe	.L88
	test	edx, 1073741823
	lea	rax, [rdx+1]
	jne	.L89
	shr	rax, 30
	jmp	.L88
.L89:
	shr	rax, 30
	inc	eax
.L88:
	lea	r8d, [rax+258]
	mov	ecx, 131584
	xor	edi, edi
	mov	edx, r8d
	sal	r8d, 9
	sal	edx, 12
	or	edx, 3
	mov	esi, edx
	mov	QWORD PTR ds:1048576, rsi
.L90:
	cmp	edi, eax
	mov	r10d, edi
	jae	.L92
	lea	r9d, [rcx-131584]
	xor	edx, edx
.L93:
	lea	esi, [r9+rdx]
	lea	r11d, [rcx+rdx]
	inc	rdx
	sal	esi, 21
	sal	r11d, 3
	or	sil, -93
	cmp	rdx, 512
	mov	esi, esi
	mov	QWORD PTR [r11d], rsi
	jne	.L93
	lea	edx, [3+rcx*8]
	add	r10d, r8d
	inc	rdi
	sal	r10d, 3
	add	ecx, 512
	mov	esi, edx
	mov	QWORD PTR [r10d], rsi
	jmp	.L90
.L92:
	add	eax, r8d
	sal	eax, 3
	mov	QWORD PTR MemoryTableAddr[rip], rax
	mov	eax, 1048576
#APP
# 402 "main.c" 1
	MOV CR3,rax
# 0 "" 2
#NO_APP
	xor	eax, eax
	ret
	.cfi_endproc
.LFE14:
	.size	init_paging, .-init_paging
	.section	.rodata.str1.1
.LC15:
	.string	"3:Kernel's table end address is smaller than RAM's useable base."
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB16:
	.cfi_startproc
	xor	eax, eax
	mov	WORD PTR cursor_offset[rip], 0
	call	get_RAM_size
	xor	eax, eax
	call	init_paging
	xor	eax, eax
	call	init_mem_table
	mov	rdi, QWORD PTR MemoryTableAddr[rip]
	mov	rax, QWORD PTR RAMbase[rip]
	lea	rdx, [rdi+512]
	cmp	rdx, rax
	ja	.L96
	mov	edi, OFFSET FLAT:.LC15
	call	panic
	xor	edi, edi
	jmp	.L98
.L96:
	add	edi, 512
	sub	edi, eax
.L98:
	call	malloc
	mov	edi, 1
	call	malloc
	mov	edi, 4
	mov	QWORD PTR a[rip], rax
	call	malloc
	mov	edi, 4
	mov	QWORD PTR b[rip], rax
	call	print_mem_table_link
	mov	rdi, QWORD PTR a[rip]
	call	free
	mov	edi, 4
	call	print_mem_table_link
	mov	rdi, QWORD PTR b[rip]
	call	free
	mov	edi, 4
	call	print_mem_table_link
#APP
# 406 "main.c" 1
	mov al,0b00000000/*启动8259的所有中断*/
out 0x21,al/*Master 8259A,OCW1*/
out	0xA1,al/*Slave 8259A,OCW1*/
mov al,0x11
out 0x20,al/*开始*/
mov al,0x20/*偏移*/
out 0x21,al/*ICW2*/
mov al,1<<2
out 0x21,al/*ICW3*/
mov al,1
out 0x21,al/*ICW4*/
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
	xor	eax, eax
	call	update_cursor
	xor	eax, eax
	ret
	.cfi_endproc
.LFE16:
	.size	main, .-main
	.comm	b,8,8
	.comm	a,8,8
	.comm	mem_table,8,8
	.comm	interrupt_flag,8,8
	.comm	cursor_offset,2,2
	.comm	MemoryTableAddr,8,8
	.comm	MaxRAMAddr,8,8
	.comm	RAMlength,8,8
	.comm	RAMbase,8,8
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
