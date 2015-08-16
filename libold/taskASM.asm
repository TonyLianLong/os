global lldt
global ltr
global _far_jmp
ltr:
	LTR [esp+4]
	ret
lldt:
	LLDT [esp+4]
	ret
_far_jmp:
	JMP FAR [ESP+4]
	ret
