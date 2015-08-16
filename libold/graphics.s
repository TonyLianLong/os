	.file	"graphics.c"
	.intel_syntax noprefix
	.text
	.globl	graphics_pixel_write
	.type	graphics_pixel_write, @function
graphics_pixel_write:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movzx	ecx, WORD PTR screen_wid
	mov	ebp, esp
	.cfi_def_cfa_register 5
	mov	eax, DWORD PTR [ebp+8]
	push	ebx
	.cfi_offset 3, -12
	mov	edx, DWORD PTR [ebp+12]
	cmp	eax, ecx
	ja	.L1
	movzx	ebx, WORD PTR screen_hei
	cmp	edx, ebx
	ja	.L1
	imul	ecx, edx
	mov	bl, BYTE PTR [ebp+16]
	add	ecx, eax
	lea	ecx, [ecx+ecx*2]
	add	ecx, DWORD PTR graphics_vram
	mov	BYTE PTR [ecx], bl
	movzx	ecx, WORD PTR screen_wid
	mov	bl, BYTE PTR [ebp+17]
	imul	ecx, edx
	add	ecx, eax
	lea	ecx, [ecx+ecx*2]
	add	ecx, DWORD PTR graphics_vram
	mov	BYTE PTR [ecx+1], bl
	movzx	ecx, WORD PTR screen_wid
	imul	edx, ecx
	add	eax, edx
	mov	dl, BYTE PTR [ebp+18]
	lea	eax, [eax+eax*2]
	add	eax, DWORD PTR graphics_vram
	mov	BYTE PTR [eax+2], dl
.L1:
	pop	ebx
	.cfi_restore 3
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	graphics_pixel_write, .-graphics_pixel_write
	.globl	graphics_draw_hline
	.type	graphics_draw_hline, @function
graphics_draw_hline:
.LFB2:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	esi
	.cfi_offset 6, -12
	mov	esi, DWORD PTR [ebp+16]
	push	ebx
	.cfi_offset 3, -16
	mov	ebx, DWORD PTR [ebp+8]
	add	esi, ebx
	jmp	.L5
.L6:
	push	eax
	mov	eax, DWORD PTR [ebp+20]
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR [ebp+22]
	mov	BYTE PTR [esp+2], al
	push	DWORD PTR [ebp+12]
	push	ebx
	inc	ebx
	call	graphics_pixel_write
	add	esp, 12
.L5:
	cmp	ebx, esi
	jne	.L6
	lea	esp, [ebp-8]
	pop	ebx
	.cfi_restore 3
	pop	esi
	.cfi_restore 6
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	graphics_draw_hline, .-graphics_draw_hline
	.globl	graphics_draw_box
	.type	graphics_draw_box, @function
graphics_draw_box:
.LFB3:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	edi
	push	esi
	push	ebx
	push	ecx
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	mov	edi, DWORD PTR [ebp+12]
	mov	eax, DWORD PTR [ebp+20]
	mov	esi, DWORD PTR [ebp+8]
	add	esi, DWORD PTR [ebp+16]
	add	eax, edi
	mov	DWORD PTR [ebp-16], eax
	jmp	.L9
.L10:
	push	edx
	mov	edx, DWORD PTR [ebp+24]
	mov	WORD PTR [esp], dx
	mov	dl, BYTE PTR [ebp+26]
	mov	BYTE PTR [esp+2], dl
	push	edi
	push	ebx
	inc	ebx
	call	graphics_pixel_write
	add	esp, 12
.L12:
	cmp	ebx, esi
	jne	.L10
	inc	edi
.L9:
	cmp	edi, DWORD PTR [ebp-16]
	je	.L8
	mov	ebx, DWORD PTR [ebp+8]
	jmp	.L12
.L8:
	lea	esp, [ebp-12]
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
.LFE3:
	.size	graphics_draw_box, .-graphics_draw_box
	.globl	show_char
	.type	show_char, @function
show_char:
.LFB4:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	edi
	push	esi
	push	ebx
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	xor	ebx, ebx
	sub	esp, 8
	mov	eax, DWORD PTR [ebp+8]
	movsx	ecx, BYTE PTR [ebp+16]
	add	eax, 8
	imul	ecx, ecx, 12
	mov	DWORD PTR [ebp-16], eax
	jmp	.L15
.L17:
	movzx	eax, BYTE PTR [ebx+32256+ecx]
	bt	eax, esi
	jnc	.L16
	push	eax
	mov	eax, DWORD PTR [ebp+20]
	mov	DWORD PTR [ebp-20], ecx
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR [ebp+22]
	mov	BYTE PTR [esp+2], al
	mov	eax, DWORD PTR [ebp-16]
	push	edi
	sub	eax, esi
	push	eax
	call	graphics_pixel_write
	mov	ecx, DWORD PTR [ebp-20]
	add	esp, 12
.L16:
	dec	esi
	jne	.L17
	inc	ebx
	cmp	ebx, 12
	je	.L14
.L15:
	mov	edi, DWORD PTR [ebp+12]
	mov	esi, 8
	add	edi, ebx
	jmp	.L17
.L14:
	lea	esp, [ebp-12]
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
.LFE4:
	.size	show_char, .-show_char
	.globl	show_string
	.type	show_string, @function
show_string:
.LFB5:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	xor	eax, eax
	mov	ebp, esp
	.cfi_def_cfa_register 5
	or	ecx, -1
	push	edi
	.cfi_offset 7, -12
	mov	edi, DWORD PTR [ebp+16]
	push	esi
	push	ebx
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	mov	ebx, DWORD PTR [ebp+8]
	repnz scasb
	mov	edi, DWORD PTR [ebp+16]
	not	ecx
	lea	esi, [edi-1+ecx]
	jmp	.L25
.L26:
	push	eax
	mov	eax, DWORD PTR [ebp+20]
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR [ebp+22]
	mov	BYTE PTR [esp+2], al
	movsx	eax, BYTE PTR [edi]
	inc	edi
	push	eax
	push	DWORD PTR [ebp+12]
	push	ebx
	add	ebx, 8
	call	show_char
	add	esp, 16
.L25:
	cmp	edi, esi
	jne	.L26
	lea	esp, [ebp-12]
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
.LFE5:
	.size	show_string, .-show_string
	.globl	set_background
	.type	set_background, @function
set_background:
.LFB8:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	esi
	push	ebx
	sub	esp, 16
	.cfi_offset 6, -12
	.cfi_offset 3, -16
#APP
# 123 "graphics.c" 1
	cli
# 0 "" 2
#NO_APP
	movzx	ebx, BYTE PTR [ebp+9]
	movzx	ecx, BYTE PTR [ebp+10]
	movzx	eax, BYTE PTR [ebp+8]
	mov	edx, ebx
	mov	esi, ecx
	sal	esi, 16
	sal	edx, 8
	or	edx, esi
	mov	esi, eax
	or	edx, eax
	sal	esi, 24
	or	edx, esi
	mov	esi, eax
	mov	DWORD PTR [ebp-20], edx
	mov	edx, ecx
	sal	esi, 16
	sal	edx, 8
	or	edx, esi
	mov	esi, ebx
	or	edx, ebx
	sal	eax, 8
	sal	ebx, 16
	sal	esi, 24
	or	eax, ebx
	or	edx, esi
	or	eax, ecx
	sal	ecx, 24
	or	eax, ecx
	movzx	ecx, WORD PTR screen_hei
	mov	DWORD PTR [ebp-16], edx
	movzx	edx, WORD PTR screen_wid
	mov	DWORD PTR [ebp-12], eax
	mov	eax, DWORD PTR graphics_vram
	imul	edx, ecx
	lea	edx, [edx+edx*2]
	lea	ecx, [eax+edx]
	jmp	.L32
.L31:
	mov	ebx, DWORD PTR [ebp-20+edx*4]
	mov	DWORD PTR [eax], ebx
	add	eax, 4
	cmp	edx, 2
	je	.L32
	inc	edx
	jmp	.L33
.L32:
	xor	edx, edx
.L33:
	cmp	eax, ecx
	jb	.L31
#APP
# 139 "graphics.c" 1
	sti
# 0 "" 2
#NO_APP
	add	esp, 16
	pop	ebx
	.cfi_restore 3
	pop	esi
	.cfi_restore 6
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	set_background, .-set_background
	.globl	graphics_init
	.type	graphics_init, @function
graphics_init:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 12
	mov	ax, WORD PTR ds:1810
	mov	WORD PTR screen_wid, ax
	mov	eax, DWORD PTR ds:1812
	mov	WORD PTR screen_hei, ax
	mov	eax, DWORD PTR ds:2304
	push	1
	push	OFFSET FLAT:cursor_flash
	push	9
	mov	DWORD PTR graphics_vram, eax
	call	add_timer
	mov	BYTE PTR graphics_background_color+1, 0
	mov	BYTE PTR graphics_background_color, -1
	mov	ax, WORD PTR graphics_background_color
	mov	BYTE PTR [esp+2], 0
	mov	DWORD PTR cursor_x, 0
	mov	DWORD PTR cursor_y, 0
	mov	WORD PTR [esp], ax
	mov	DWORD PTR now_cursor_x, 0
	mov	DWORD PTR now_cursor_y, 0
	mov	BYTE PTR graphics_background_color+2, 0
	call	set_background
	add	esp, 16
	mov	BYTE PTR graphics_cursor_color+2, -1
	mov	BYTE PTR graphics_cursor_color+1, -1
	mov	BYTE PTR graphics_cursor_color, -1
	mov	BYTE PTR graphics_default_text_color+2, 0
	mov	BYTE PTR graphics_default_text_color+1, 0
	mov	BYTE PTR graphics_default_text_color, 0
	mov	BYTE PTR now_cursor, 0
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	graphics_init, .-graphics_init
	.globl	clear_cursor
	.type	clear_cursor, @function
clear_cursor:
.LFB9:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	eax
	mov	ax, WORD PTR graphics_background_color
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR graphics_background_color+2
	mov	BYTE PTR [esp+2], al
	mov	eax, DWORD PTR now_cursor_y
	push	8
	add	eax, 11
	push	eax
	push	DWORD PTR now_cursor_x
	call	graphics_draw_hline
	add	esp, 16
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	clear_cursor, .-clear_cursor
	.globl	show_cursor
	.type	show_cursor, @function
show_cursor:
.LFB10:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	eax
	mov	ax, WORD PTR graphics_cursor_color
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR graphics_cursor_color+2
	mov	BYTE PTR [esp+2], al
	mov	eax, DWORD PTR now_cursor_y
	push	8
	add	eax, 11
	push	eax
	push	DWORD PTR now_cursor_x
	call	graphics_draw_hline
	add	esp, 16
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.size	show_cursor, .-show_cursor
	.globl	cursor_flash
	.type	cursor_flash, @function
cursor_flash:
.LFB11:
	.cfi_startproc
	cmp	BYTE PTR now_cursor, 0
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	je	.L42
	call	clear_cursor
	mov	eax, DWORD PTR cursor_x
	mov	DWORD PTR now_cursor_x, eax
	mov	eax, DWORD PTR cursor_y
	mov	DWORD PTR now_cursor_y, eax
	jmp	.L43
.L42:
	mov	eax, DWORD PTR cursor_x
	mov	DWORD PTR now_cursor_x, eax
	mov	eax, DWORD PTR cursor_y
	mov	DWORD PTR now_cursor_y, eax
	call	show_cursor
.L43:
	cmp	BYTE PTR now_cursor, 0
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	sete	BYTE PTR now_cursor
	ret
	.cfi_endproc
.LFE11:
	.size	cursor_flash, .-cursor_flash
	.globl	print_char
	.type	print_char, @function
print_char:
.LFB6:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	edi
	push	esi
	push	ebx
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	push	ebx
	movsx	ebx, BYTE PTR [ebp+8]
	cmp	bl, 13
	je	.L45
	cmp	bl, 10
	jne	.L47
	jmp	.L62
.L47:
	cmp	bl, 9
	mov	eax, DWORD PTR cursor_x
	jne	.L48
	mov	ebx, 32
	cdq
	lea	ecx, [eax+32]
	idiv	ebx
	sub	ecx, edx
	mov	DWORD PTR cursor_x, ecx
	jmp	.L56
.L48:
	xor	edi, edi
	cmp	bl, 8
	jne	.L49
	sub	eax, 8
	test	eax, eax
	mov	DWORD PTR cursor_x, eax
	jns	.L58
	movzx	eax, WORD PTR screen_wid
	sub	DWORD PTR cursor_y, 12
	and	eax, -8
	sub	eax, 8
	mov	DWORD PTR cursor_x, eax
.L58:
	mov	edi, 1
	mov	ebx, 32
.L49:
	imul	ebx, ebx, 12
	mov	DWORD PTR [ebp-16], ebx
	xor	ebx, ebx
	jmp	.L50
.L53:
	mov	ecx, DWORD PTR [ebp-16]
	mov	eax, 8
	sub	eax, esi
	movzx	edx, BYTE PTR [ebx+32256+ecx]
	bt	edx, esi
	jnc	.L51
	mov	edx, DWORD PTR [ebp+12]
	push	ecx
	mov	WORD PTR [esp], dx
	mov	dl, BYTE PTR [ebp+14]
	jmp	.L61
.L51:
	push	edx
	mov	dx, WORD PTR graphics_background_color
	mov	WORD PTR [esp], dx
	mov	dl, BYTE PTR graphics_background_color+2
.L61:
	mov	BYTE PTR [esp+2], dl
	mov	edx, DWORD PTR cursor_y
	add	eax, DWORD PTR cursor_x
	add	edx, ebx
	push	edx
	push	eax
	call	graphics_pixel_write
	add	esp, 12
	dec	esi
	jne	.L53
	inc	ebx
	cmp	ebx, 12
	je	.L54
.L50:
	mov	esi, 8
	jmp	.L53
.L54:
	mov	eax, DWORD PTR cursor_x
	movzx	edx, WORD PTR screen_wid
	cmp	eax, edx
	jle	.L55
.L62:
	mov	DWORD PTR cursor_x, 0
	add	DWORD PTR cursor_y, 12
	jmp	.L56
.L55:
	mov	edx, edi
	test	dl, dl
	jne	.L56
	add	eax, 8
	mov	DWORD PTR cursor_x, eax
.L56:
	lea	esp, [ebp-12]
	pop	ebx
	.cfi_remember_state
	.cfi_restore 3
	pop	esi
	.cfi_restore 6
	pop	edi
	.cfi_restore 7
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	jmp	cursor_flash
.L45:
	.cfi_restore_state
	lea	esp, [ebp-12]
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
.LFE6:
	.size	print_char, .-print_char
	.globl	print_string
	.type	print_string, @function
print_string:
.LFB7:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	xor	eax, eax
	mov	ebp, esp
	.cfi_def_cfa_register 5
	or	ecx, -1
	push	edi
	.cfi_offset 7, -12
	mov	edi, DWORD PTR [ebp+8]
	push	esi
	push	ebx
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	mov	ebx, DWORD PTR [ebp+8]
	repnz scasb
	not	ecx
	lea	esi, [ebx-1+ecx]
	jmp	.L64
.L65:
	mov	eax, DWORD PTR [ebp+12]
	push	edi
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR [ebp+14]
	mov	BYTE PTR [esp+2], al
	movsx	eax, BYTE PTR [ebx]
	inc	ebx
	push	eax
	call	print_char
	pop	eax
	pop	edx
.L64:
	cmp	ebx, esi
	jne	.L65
	lea	esp, [ebp-12]
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
.LFE7:
	.size	print_string, .-print_string
	.comm	now_cursor,1,1
	.comm	now_cursor_y,4,4
	.comm	now_cursor_x,4,4
	.comm	cursor_y,4,4
	.comm	cursor_x,4,4
	.comm	graphics_vram,4,4
	.comm	graphics_default_text_color,3,1
	.comm	graphics_cursor_color,3,1
	.comm	graphics_background_color,3,1
	.comm	screen_hei,2,2
	.comm	screen_wid,2,2
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
