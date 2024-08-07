global _start

extern sys_exit
extern chr_print
extern eol_print
extern int_print

section .data
	n dq 0
	i dq 0
	j dq 0

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

