	.file	"string.c"
	.intel_syntax noprefix
	.text
	.globl	strlen
	.type	strlen, @function
strlen:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 16
	mov	DWORD PTR [ebp-4], 0
	mov	DWORD PTR [ebp-4], 0
	jmp	.L2
.L3:
	mov	eax, DWORD PTR [ebp-4]
	add	eax, 1
	mov	DWORD PTR [ebp-4], eax
.L2:
	mov	eax, DWORD PTR [ebp-4]
	mov	edx, eax
	mov	eax, DWORD PTR [ebp+8]
	add	eax, edx
	movzx	eax, BYTE PTR [eax]
	test	al, al
	jne	.L3
	mov	eax, DWORD PTR [ebp-4]
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	strlen, .-strlen
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
