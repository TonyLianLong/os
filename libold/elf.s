	.file	"elf.c"
	.intel_syntax noprefix
	.text
	.globl	get_elf_header
	.type	get_elf_header, @function
get_elf_header:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	mov	eax, DWORD PTR [ebp+8]
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	get_elf_header, .-get_elf_header
	.globl	get_elf_entry
	.type	get_elf_entry, @function
get_elf_entry:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	mov	eax, DWORD PTR [ebp+8]
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	mov	eax, DWORD PTR [eax+24]
	ret
	.cfi_endproc
.LFE1:
	.size	get_elf_entry, .-get_elf_entry
	.globl	loadelf
	.type	loadelf, @function
loadelf:
.LFB2:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	eax, 1
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	edi
	push	esi
	push	ebx
	push	ecx
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	mov	edx, DWORD PTR [ebp+8]
	cmp	BYTE PTR [edx], 127
	jne	.L6
	cmp	BYTE PTR [edx+1], 69
	jne	.L6
	cmp	BYTE PTR [edx+2], 76
	jne	.L6
	cmp	BYTE PTR [edx+3], 70
	jne	.L6
	cmp	BYTE PTR [edx+4], 1
	mov	al, 2
	jne	.L6
	cmp	BYTE PTR [edx+6], 1
	mov	al, 3
	jne	.L6
	cmp	BYTE PTR [edx+7], 0
	mov	al, 4
	jne	.L6
	cmp	WORD PTR [edx+16], 2
	mov	al, 5
	jne	.L6
	cmp	WORD PTR [edx+18], 3
	mov	al, 6
	jne	.L6
	cmp	DWORD PTR [edx+20], 1
	mov	al, 7
	jne	.L6
	mov	ebx, DWORD PTR [edx+28]
	mov	DWORD PTR [ebp-16], 0
	add	ebx, edx
	jmp	.L7
.L10:
	cmp	DWORD PTR [ebx], 1
	jne	.L8
	cmp	DWORD PTR [ebx+24], 0
	je	.L8
	mov	eax, DWORD PTR [ebx+8]
	mov	ecx, DWORD PTR [ebx+16]
	lea	esi, [eax+ecx]
	cmp	esi, DWORD PTR [ebp+16]
	ja	.L21
	mov	esi, DWORD PTR [ebx+4]
	add	eax, DWORD PTR [ebp+12]
	add	esi, edx
	mov	edi, eax
	rep movsb
.L8:
	movzx	eax, WORD PTR [edx+42]
	inc	DWORD PTR [ebp-16]
	add	ebx, eax
.L7:
	movzx	eax, WORD PTR [edx+44]
	cmp	DWORD PTR [ebp-16], eax
	jl	.L10
	xor	eax, eax
	jmp	.L6
.L21:
	mov	eax, 8
.L6:
	pop	edx
	pop	ebx
	.cfi_restore 3
	pop	esi
	.cfi_restore 6
	pop	edi
	.cfi_restore 7
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	loadelf, .-loadelf
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
