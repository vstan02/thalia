global _start

extern sys_exit
extern chr_print
extern eol_print
extern int_print

section .data
	i dq 0

section .text
_start:
	push 0
	pop qword [i]
	push qword [i]
	pop rax
.ll1:
	push qword [i]
	push 10
	pop rbx
	pop rax
	mov rdx, 1
	cmp rax, rbx
	jl .ll3
	mov rdx, 0
.ll3:
	mov rax, rdx
	push rax
	test rax, rax
	je .ll2
	push qword [i]
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
	jne .ll5
	mov rdx, 1
.ll5:
	mov rax, rdx
	push rax
	test rax, rax
	je .ll4
	push qword [i]
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
.ll4:
	push qword [i]
	push 1
	pop rbx
	pop rax
	add rax, rbx
	push rax
	pop qword [i]
	push qword [i]
	pop rax
	jmp .ll1
.ll2:
	push 0
	pop qword [i]
.ll6:
	push qword [i]
	push 10
	pop rbx
	pop rax
	cmp rax, rbx
	jge .ll7
	push qword [i]
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
	push 2
	pop rax
	add qword [i], rax
	jmp .ll6
.ll7:
	call sys_exit

