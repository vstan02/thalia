global str_len

section .text
	; input:
	; : rax = string
	; output:
	; : rax = string length
	str_len:
		push rbx
		xor rbx, rbx

	.iter:
		cmp [rax+rbx], byte 0x0
		je .close
		inc rbx
		jmp .iter

	.close:
		mov rax, rbx
		pop rbx
		ret
