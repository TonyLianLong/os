	.file	"timer.c"
	.intel_syntax noprefix
	.text
	.globl	timer_exec
	.type	timer_exec, @function
timer_exec:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	ebx
	push	edx
	.cfi_offset 3, -12
	xor	ebx, ebx
.L6:
	mov	eax, DWORD PTR timer_list
	add	eax, ebx
	cmp	BYTE PTR [eax], 0
	je	.L3
	dec	DWORD PTR [eax+4]
	jne	.L3
	call	[DWORD PTR [eax+12]]
	mov	eax, DWORD PTR timer_list
	add	eax, ebx
	cmp	BYTE PTR [eax+16], 0
	je	.L5
	mov	edx, DWORD PTR [eax+8]
	mov	DWORD PTR [eax+4], edx
	jmp	.L3
.L5:
	mov	BYTE PTR [eax], 0
.L3:
	add	ebx, 20
	cmp	ebx, 5120
	jne	.L6
#APP
# 22 "timer.c" 1
	PUSH AX
	MOV AL,0x20
	outb 0x20
	outb 0x0A0
	POP AX
	leave
	iretd
# 0 "" 2
#NO_APP
	pop	eax
	pop	ebx
	.cfi_restore 3
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	timer_exec, .-timer_exec
	.globl	init_timer
	.type	init_timer, @function
init_timer:
.LFB1:
	.cfi_startproc
	mov	DWORD PTR timer_list, 3151872
	xor	eax, eax
.L10:
	mov	BYTE PTR [eax+3151872], 0
	add	eax, 20
	cmp	eax, 5120
	jne	.L10
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 12
	push	142
	push	OFFSET FLAT:timer_exec
	push	8
	call	write_idt_entry
	add	esp, 16
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	init_timer, .-init_timer
	.globl	set_timer
	.type	set_timer, @function
set_timer:
.LFB2:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	movzx	eax, BYTE PTR [ebp+8]
	mov	edx, DWORD PTR [ebp+12]
	mov	ecx, DWORD PTR [ebp+20]
	imul	eax, eax, 20
	add	eax, DWORD PTR timer_list
	mov	DWORD PTR [eax+4], edx
	mov	DWORD PTR [eax+8], edx
	mov	edx, DWORD PTR [ebp+16]
	mov	BYTE PTR [eax], 1
	mov	BYTE PTR [eax+16], cl
	mov	DWORD PTR [eax+12], edx
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	set_timer, .-set_timer
	.globl	add_timer
	.type	add_timer, @function
add_timer:
.LFB3:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	movzx	eax, BYTE PTR [ebp+16]
	push	eax
	push	DWORD PTR [ebp+12]
	push	DWORD PTR [ebp+8]
	push	0
	call	set_timer
	xor	eax, eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	add_timer, .-add_timer
	.globl	istimer
	.type	istimer, @function
istimer:
.LFB4:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	movzx	eax, BYTE PTR [ebp+8]
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	imul	eax, eax, 20
	add	eax, DWORD PTR timer_list
	mov	al, BYTE PTR [eax]
	ret
	.cfi_endproc
.LFE4:
	.size	istimer, .-istimer
	.globl	clear_timer
	.type	clear_timer, @function
clear_timer:
.LFB5:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	movzx	eax, BYTE PTR [ebp+8]
	imul	eax, eax, 20
	add	eax, DWORD PTR timer_list
	mov	BYTE PTR [eax], 0
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	clear_timer, .-clear_timer
	.comm	timer_list,4,4
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
