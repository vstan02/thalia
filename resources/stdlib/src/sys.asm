global sys_exit
global sys_time

section .data
	SYS_TIME equ 0xc9
	SYS_EXIT equ 0x3c

section .text
	; output:
	; : rax = integer
	sys_time:
		push rdi

		mov rax, SYS_TIME
		xor rdi, rdi
		syscall

		pop rdi
		ret

	sys_exit:
		mov rax, SYS_EXIT
		xor rdi, rdi
		syscall
