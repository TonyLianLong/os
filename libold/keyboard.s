	.file	"keyboard.c"
	.intel_syntax noprefix
	.text
	.globl	mouse
	.type	mouse, @function
mouse:
.LFB5:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 20
	push	96
	call	read_port
	mov	dl, BYTE PTR mouse_mode
	add	esp, 16
	cmp	dl, 1
	je	.L4
	jb	.L3
	cmp	dl, 2
	je	.L5
	cmp	dl, 3
	jne	.L2
	jmp	.L6
.L3:
	cmp	al, -6
	jne	.L2
	mov	ax, WORD PTR graphics_default_text_color
	sub	esp, 12
	mov	BYTE PTR mouse_mode, 1
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR graphics_default_text_color+2
	mov	BYTE PTR [esp+2], al
	push	77
	jmp	.L10
.L4:
	and	eax, 7
	sub	esp, 12
	mov	BYTE PTR mouseclick, al
	mov	ax, WORD PTR graphics_default_text_color
	mov	BYTE PTR mouse_mode, 2
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR graphics_default_text_color+2
	mov	BYTE PTR [esp+2], al
	push	70
.L10:
	call	print_char
.L11:
	add	esp, 16
	jmp	.L2
.L5:
	movzx	edx, WORD PTR screen_wid
	movsx	eax, al
	add	eax, DWORD PTR mousex
	cmp	eax, edx
	mov	DWORD PTR mousex, eax
	jbe	.L7
	mov	DWORD PTR mousex, edx
.L7:
	mov	ax, WORD PTR graphics_default_text_color
	sub	esp, 12
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR graphics_default_text_color+2
	mov	BYTE PTR [esp+2], al
	push	83
	call	print_char
	mov	BYTE PTR mouse_mode, 3
	jmp	.L11
.L6:
	movzx	edx, WORD PTR screen_hei
	movsx	eax, al
	add	eax, DWORD PTR mousey
	cmp	eax, edx
	mov	DWORD PTR mousey, eax
	jbe	.L8
	mov	DWORD PTR mousey, edx
.L8:
	mov	BYTE PTR mouse_mode, 1
.L2:
#APP
# 128 "keyboard.c" 1
	PUSH AX
	MOV AL,0x20
	outb 0x20
	outb 0x0A0
	POP AX
	leave
	iretd
# 0 "" 2
#NO_APP
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	mouse, .-mouse
	.globl	keyboard
	.type	keyboard, @function
keyboard:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	ebx
	sub	esp, 32
	.cfi_offset 3, -12
	push	96
	call	read_port
	add	esp, 16
	mov	BYTE PTR [ebp-11], -1
	mov	BYTE PTR [ebp-10], -1
	mov	BYTE PTR [ebp-9], -1
	cmp	al, 54
	je	.L24
	cmp	al, 42
	jne	.L13
.L24:
	mov	BYTE PTR shift_pressed, 1
.L13:
	cmp	al, -74
	je	.L25
	cmp	al, -86
	jne	.L15
.L25:
	mov	BYTE PTR shift_pressed, 0
.L15:
	cmp	al, -70
	jne	.L17
	cmp	BYTE PTR CAPS_LOCK, 0
	sete	BYTE PTR CAPS_LOCK
	jmp	.L18
.L17:
	cmp	al, 57
	ja	.L18
	movzx	eax, al
	mov	bl, BYTE PTR keycode_list[eax]
	mov	al, BYTE PTR CAPS_LOCK
	cmp	BYTE PTR shift_pressed, al
	je	.L19
	lea	eax, [ebx-97]
	cmp	al, 25
	ja	.L19
	sub	ebx, 32
.L19:
	mov	ax, WORD PTR [ebp-11]
	sub	esp, 12
	mov	WORD PTR [esp], ax
	mov	al, BYTE PTR [ebp-9]
	mov	BYTE PTR [esp+2], al
	movsx	eax, bl
	push	eax
	call	print_char
	mov	eax, DWORD PTR write_buffer_pointer
	add	esp, 16
	cmp	BYTE PTR keyboard_buffer[eax], 10
	jne	.L20
	mov	BYTE PTR run_program, 1
	jmp	.L21
.L20:
	mov	edx, DWORD PTR read_buffer_pointer
	dec	edx
	cmp	eax, edx
	je	.L21
	mov	BYTE PTR keyboard_buffer[eax], bl
	inc	eax
	mov	DWORD PTR write_buffer_pointer, eax
	add	eax, -128
	jne	.L21
	mov	DWORD PTR write_buffer_pointer, 0
	jmp	.L21
.L18:
	push	edx
	movzx	eax, al
	push	edx
	push	OFFSET FLAT:keycode_str
	push	eax
	call	itox
	mov	ax, WORD PTR graphics_default_text_color
	mov	DWORD PTR [esp], OFFSET FLAT:keycode_str
	mov	WORD PTR [esp+4], ax
	mov	al, BYTE PTR graphics_default_text_color+2
	mov	BYTE PTR [esp+6], al
	call	print_string
	add	esp, 16
.L21:
#APP
# 56 "keyboard.c" 1
	PUSH AX
	MOV AL,0x20
	outb 0x20
	outb 0x0A0
	POP AX
	leave
	iretd
# 0 "" 2
#NO_APP
	mov	ebx, DWORD PTR [ebp-4]
	leave
	.cfi_restore 5
	.cfi_restore 3
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	keyboard, .-keyboard
	.globl	init_keyboard
	.type	init_keyboard, @function
init_keyboard:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 12
	push	142
	push	OFFSET FLAT:keyboard
	push	9
	call	write_idt_entry
	add	esp, 16
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	init_keyboard, .-init_keyboard
	.globl	get_char
	.type	get_char, @function
get_char:
.LFB2:
	.cfi_startproc
	mov	ecx, DWORD PTR write_buffer_pointer
	mov	edx, DWORD PTR read_buffer_pointer
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	dec	ecx
	cmp	edx, ecx
	mov	al, BYTE PTR keyboard_buffer[edx]
	je	.L38
	test	al, al
	je	.L36
	inc	edx
	cmp	edx, 128
	je	.L37
	mov	DWORD PTR read_buffer_pointer, edx
	jmp	.L36
.L37:
	mov	DWORD PTR read_buffer_pointer, 0
	jmp	.L36
.L38:
	xor	eax, eax
.L36:
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	get_char, .-get_char
	.globl	get_string
	.type	get_string, @function
get_string:
.LFB3:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	cmp	DWORD PTR [ebp+12], 0
	je	.L44
.L47:
	xor	eax, eax
	jmp	.L45
.L46:
	mov	edx, DWORD PTR [ebp+8]
	mov	BYTE PTR [edx], al
	mov	eax, 1
	jmp	.L45
.L44:
	call	get_char
	cmp	al, -1
	jne	.L46
	jmp	.L47
.L45:
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	get_string, .-get_string
	.globl	wait_KBC
	.type	wait_KBC, @function
wait_KBC:
.LFB4:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 8
.L50:
	sub	esp, 12
	push	100
	call	read_port
	add	esp, 16
	test	al, 2
	jne	.L50
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	wait_KBC, .-wait_KBC
	.globl	init_mouse
	.type	init_mouse, @function
init_mouse:
.LFB6:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 8
	mov	ax, WORD PTR screen_wid
	shr	ax
	movzx	eax, ax
	mov	DWORD PTR mousex, eax
	mov	ax, WORD PTR screen_hei
	shr	ax
	movzx	eax, ax
	mov	DWORD PTR mousey, eax
	call	wait_KBC
	push	eax
	push	eax
	push	96
	push	100
	call	write_port
	call	wait_KBC
	pop	eax
	pop	edx
	push	71
	push	96
	call	write_port
	call	wait_KBC
	pop	ecx
	pop	eax
	push	212
	push	100
	call	write_port
	call	wait_KBC
	pop	eax
	pop	edx
	push	244
	push	96
	call	write_port
	add	esp, 12
	push	142
	push	OFFSET FLAT:mouse
	push	116
	call	write_idt_entry
	add	esp, 16
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	init_mouse, .-init_mouse
	.comm	mousey,4,4
	.comm	mousex,4,4
	.globl	mouseclick
	.bss
	.type	mouseclick, @object
	.size	mouseclick, 1
mouseclick:
	.zero	1
	.globl	mouse_mode
	.type	mouse_mode, @object
	.size	mouse_mode, 1
mouse_mode:
	.zero	1
	.comm	keycode_str,7,4
	.globl	keycode_list
	.section	.rodata
	.align 4
	.type	keycode_list, @object
	.size	keycode_list, 58
keycode_list:
	.byte	0
	.byte	0
	.byte	49
	.byte	50
	.byte	51
	.byte	52
	.byte	53
	.byte	54
	.byte	55
	.byte	56
	.byte	57
	.byte	48
	.byte	45
	.byte	43
	.byte	8
	.byte	9
	.byte	113
	.byte	119
	.byte	101
	.byte	114
	.byte	116
	.byte	121
	.byte	117
	.byte	105
	.byte	111
	.byte	112
	.byte	91
	.byte	93
	.byte	10
	.byte	0
	.byte	97
	.byte	115
	.byte	100
	.byte	102
	.byte	103
	.byte	104
	.byte	106
	.byte	107
	.byte	108
	.byte	59
	.byte	39
	.byte	0
	.byte	0
	.byte	0
	.byte	122
	.byte	120
	.byte	99
	.byte	118
	.byte	98
	.byte	110
	.byte	109
	.byte	44
	.byte	46
	.byte	47
	.byte	0
	.byte	0
	.byte	0
	.byte	32
	.globl	CAPS_LOCK
	.bss
	.type	CAPS_LOCK, @object
	.size	CAPS_LOCK, 1
CAPS_LOCK:
	.zero	1
	.globl	shift_pressed
	.type	shift_pressed, @object
	.size	shift_pressed, 1
shift_pressed:
	.zero	1
	.globl	write_buffer_pointer
	.align 4
	.type	write_buffer_pointer, @object
	.size	write_buffer_pointer, 4
write_buffer_pointer:
	.zero	4
	.globl	read_buffer_pointer
	.align 4
	.type	read_buffer_pointer, @object
	.size	read_buffer_pointer, 4
read_buffer_pointer:
	.zero	4
	.comm	keyboard_buffer,128,4
	.globl	run_program
	.type	run_program, @object
	.size	run_program, 1
run_program:
	.zero	1
	.comm	kernel_data_selector,4,4
	.comm	kernel_code_selector,4,4
	.comm	mem_list,4,4
	.comm	mem_size,4,4
	.comm	mem_table_size,4,4
	.comm	mem_table,4,4
	.comm	graphics_vram,4,4
	.comm	graphics_default_text_color,3,1
	.comm	graphics_cursor_color,3,1
	.comm	graphics_background_color,3,1
	.comm	screen_hei,2,2
	.comm	screen_wid,2,2
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
