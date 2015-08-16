global api_main
global exit
extern main
api_main:
	CALL main
	PUSH 0
	CALL exit
	POP EAX
exit:
	MOV EAX,0xFF
	MOV EBX,[ESP]
	INT 0x20