
global _start

section .data
	false equ 0
	true equ 1
	null equ 0
	_SYS_EXIT_ equ 0x3c
	_SYS_WRITE_ equ 0x1
	_SYS_STDOUT_ equ 1
	x dq 0

section .bss
	_CHR_BSS_ resb 1

section .text
_start:
	push 0
	pop qword [x]
.each_stmt_1:
	push qword [x]
	push 10
	pop rbx
	pop rax
	cmp rax, rbx
	jge .cmp_stmt_1
	push qword [x]
	push 2
	pop rbx
	pop rax
	xor rdx, rdx
	idiv rbx
	mov rax, rdx
	push rax
	push 0
	pop rbx
	pop rax
	mov rdx, 0
	cmp rax, rbx
	jne .cmp_stmt_2
	mov rdx, 1
.cmp_stmt_2:
	mov rax, rdx
	push rax
	test rax, rax
	je .cmp_stmt_3
	push qword [x]
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
	call eol_print
.cmp_stmt_3:
	inc qword [x]
	jmp .each_stmt_1
.cmp_stmt_1:
	call sys_exit

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

chr_print:
	push rax
	push rdi
	push rsi
	push rdx
	push rcx
	mov [_CHR_BSS_], al
	mov rax, _SYS_WRITE_
	mov rdi, _SYS_STDOUT_
	mov rsi, _CHR_BSS_
	mov rdx, 1
	syscall
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rax
	ret

sys_exit:
	mov rax, _SYS_EXIT_
	xor rdi, rdi
	syscall
