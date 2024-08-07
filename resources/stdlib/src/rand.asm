global rand
global srand

section .data
	NEXT dq 1

section .text
	; output:
	; : rax = integer
	rand:
		push rbx
		push rdx

		mov rax, [NEXT]
		mov rbx, 1103515245 * 12345
		mul rbx
		mov [NEXT], rax

		mov rbx, 65536
		div rbx
		xor rdx, rdx
		mov rbx, 32768
	    div rbx
	    mov rax, rdx

	    pop rdx
	    pop rbx
	    ret

	; input:
	; : rax = integer
	srand:
		mov [NEXT], rax
		ret
