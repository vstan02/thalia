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
.ll1:
	push qword [i]
	push 10
	pop rbx
	pop rax
	cmp rax, rbx
	jge .ll2
	push qword [i]
	pop rax
	call int_print
	mov rax, ' '
	call chr_print
	call eol_print
	push 2
	pop rax
	add qword [i], rax
	jmp .ll1
.ll2:
	call sys_exit

