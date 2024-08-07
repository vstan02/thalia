global arr_print
global int_print
global eol_print
global chr_print
global str_print

extern str_len

section .data
	SYS_WRITE equ 0x1
	SYS_STDOUT equ 1

	INT_SIZE equ 8

section .bss
	CHR_BSS resb 1

section .text
	; | input:
	; : rax = integer array
	; : rdi = array size
	arr_print:
		push rbx
		push rcx

		mov rbx, rax
		xor rcx, rcx

		mov rax, '['
		call chr_print

		cmp rdi, 0
		je .close

	.iter:
		mov rax, [rbx + rcx * INT_SIZE]
		call int_print

		inc rcx
		cmp rcx, rdi
		je .close

		mov rax, ','
		call chr_print
		mov rax, ' '
		call chr_print

		jmp .iter

	.close:
		mov rax, ']'
		call chr_print

		mov rax, rbx

		pop rcx
		pop rbx
		ret

	; input:
	; : rax = integer
	int_print:
		push rax
		push rbx
		push rcx
		push rdx
		
		xor rcx, rcx
		cmp rax, 0
		jge .decomp

	.negate:
		mov rdx, rax

		mov rax, '-'
		call chr_print

		mov rax, rdx
		neg rax

	.decomp:
		mov rbx, 10
		xor rdx, rdx
		div rbx
		add rdx, '0'
		push rdx
		inc rcx

		cmp rax, 0
		je .print
		jmp .decomp

	.print:
		cmp rcx, 0
		je .close

		dec rcx
		pop rax
		call chr_print

		jmp .print

	.close:
		pop rdx
		pop rcx
		pop rbx
		pop rax
		ret

	eol_print:
		push rax

		mov rax, 0xA
		call chr_print

		pop rax
		ret

	; input:
	; : rax = char
	chr_print:
		push rax
		push rdi
		push rsi
		push rdx
		push rcx

		mov [CHR_BSS], al
		mov rax, SYS_WRITE
		mov rdi, SYS_STDOUT
		mov rsi, CHR_BSS
		mov rdx, 1
		syscall

		pop rcx
		pop rdx
		pop rsi
		pop rdi
		pop rax
		ret

	; input:
	; : rax = string
	str_print:
		push rax
		push rdi
		push rsi
		push rdx
		push rcx

		mov rsi, rax
		call str_len
		mov rdx, rax
		mov rax, SYS_WRITE
		mov rdi, SYS_STDOUT
		syscall

		pop rcx
		pop rdx
		pop rsi
		pop rdi
		pop rax
		ret
