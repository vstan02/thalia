global _start

section .data
	false equ 0
	true equ 1
	null equ 0
	_SYS_EXIT_ equ 0x3c
	_SYS_WRITE_ equ 0x1
	_SYS_STDOUT_ equ 1
	n dq 0
	i dq 0
	j dq 0

section .bss
	_CHR_BSS_ resb 1

section .text
_start:
	push 5
	pop qword [n]
	push qword [n]
	pop rax
	push 0
	pop qword [i]
.ll1:
	push qword [i]
	push qword [n]
	pop rbx
	pop rax
	cmp rax, rbx
	jge .ll2
	push 0
	pop qword [j]
.ll3:
	push qword [j]
	push qword [n]
	push qword [i]
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	pop rbx
	pop rax
	cmp rax, rbx
	jge .ll4
	push 0
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
	inc qword [j]
	jmp .ll3
.ll4:
	push 0
	pop qword [j]
.ll5:
	push qword [j]
	push qword [i]
	push 1
	pop rbx
	pop rax
	add rax, rbx
	push rax
	pop rbx
	pop rax
	cmp rax, rbx
	jge .ll6
	push 1
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
	inc qword [j]
	jmp .ll5
.ll6:
	call eol_print
	inc qword [i]
	jmp .ll1
.ll2:
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

