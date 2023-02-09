/* Ast deallocator - Thalia syntax tree deallocator
 * Copyright (C) 2021 Stan Vlad <vstan02@protonmail.com>
 *
 * This file is part of Thalia.
 *
 * Thalia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "nasm/translator.hpp"

using namespace thalia::nasm;

static char NASM_START[] = ""
	"global _start\n\n"
	"section .data\n"
	"PI equ 3\n"
	"_SYS_EXIT_ equ 0x3c\n"
	"_SYS_WRITE_ equ 0x1\n"
	"_SYS_STDOUT_ equ 1\n\n"
	"section .bss\n"
	"_CHR_BSS_ resb 1\n\n"
	"section .text\n";

static char NASM_END[] = ""
	"int_print:\n"
	"push rax\n"
	"push rbx\n"
	"push rcx\n"
	"push rdx\n"
	"xor rcx, rcx\n"
	"cmp rax, 0\n"
	"jge .decomp\n"
	".negate:\n"
	"mov rdx, rax\n"
	"mov rax, '-'\n"
	"call chr_print\n"
	"mov rax, rdx\n"
	"neg rax\n"
	".decomp:\n"
	"mov rbx, 10\n"
	"xor rdx, rdx\n"
	"div rbx\n"
	"add rdx, '0'\n"
	"push rdx\n"
	"inc rcx\n"
	"cmp rax, 0\n"
	"je .print\n"
	"jmp .decomp\n"
	".print:\n"
	"cmp rcx, 0\n"
	"je .close\n"
	"dec rcx\n"
	"pop rax\n"
	"call chr_print\n"
	"jmp .print\n"
	".close:\n"
	"pop rdx\n"
	"pop rcx\n"
	"pop rbx\n"
	"pop rax\n"
	"ret\n\n"
	"eol_print:\n"
	"push rax\n"
	"mov rax, 0xA\n"
	"call chr_print\n"
	"pop rax\n"
	"ret\n\n"
	"chr_print:\n"
	"push rax\n"
	"push rdi\n"
	"push rsi\n"
	"push rdx\n"
	"push rcx\n"
	"mov [_CHR_BSS_], al\n"
	"mov rax, _SYS_WRITE_\n"
	"mov rdi, _SYS_STDOUT_\n"
	"mov rsi, _CHR_BSS_\n"
	"mov rdx, 1\n"
	"syscall\n"
	"pop rcx\n"
	"pop rdx\n"
	"pop rsi\n"
	"pop rdi\n"
	"pop rax\n"
	"ret\n\n"
	"sys_exit:\n"
	"mov rax, _SYS_EXIT_\n"
	"xor rdi, rdi\n"
	"syscall\n";

extern std::ostream& translator::translate(std::ostream& out) {
	out << NASM_START;
	for (parser::stmts::statement* item: _target) {
		translate_statement(out, item);
	}
	return out << NASM_END;
}

extern std::ostream& translator::translate_statement(std::ostream& out, parser::stmts::statement* node) {
	if (node == nullptr) return out;
	using namespace parser;
	switch (node->type) {
		case stmts::stmt_type::PROGRAM:
			return translate_program_statement(out, static_cast<stmts::program*>(node));
		case stmts::stmt_type::BLOCK:
			return translate_block_statement(out, static_cast<stmts::block*>(node));
		case stmts::stmt_type::PRINT:
			return translate_print_statement(out, static_cast<stmts::print*>(node));
		case stmts::stmt_type::EXPRESSION:
			return translate_expression_statement(out, static_cast<stmts::expression*>(node));
	}
	return out;
}

extern std::ostream& translator::translate_program_statement(std::ostream& out, parser::stmts::program* node) {
	return translate_statement(out << "_start:\n", node->target) << "call sys_exit\n\n";
}

extern std::ostream& translator::translate_block_statement(std::ostream& out, parser::stmts::block* node) {
	for (parser::stmts::statement* item: node->stmts) {
		translate_statement(out, item);
	}
	return out;
}

extern std::ostream& translator::translate_print_statement(std::ostream& out, parser::stmts::print* node) {
	for (parser::exprs::expression* item: node->target) {
		translate_expression(out, item) << "pop rax\ncall int_print\nmov rax, ' '\ncall chr_print\n";
	}
	return out << "call eol_print\n\n";
}

extern std::ostream& translator::translate_expression_statement(std::ostream& out, parser::stmts::expression* node) {
	return translate_expression(out, node->target);
}

extern std::ostream& translator::translate_expression(std::ostream& out, parser::exprs::expression* node) {
	if (node == nullptr) return out;
	using namespace parser;
	switch (node->type) {
		case exprs::expr_type::ASSIGN:
			return translate_assign_expression(out, static_cast<exprs::assign*>(node));
		case exprs::expr_type::BINARY:
			return translate_binary_expression(out, static_cast<exprs::binary*>(node));
		case exprs::expr_type::UNARY:
			return translate_unary_expression(out, static_cast<exprs::unary*>(node));
		case exprs::expr_type::GROUPING:
			return translate_grouping_expression(out, static_cast<exprs::grouping*>(node));
		case exprs::expr_type::LITERAL:
			return translate_literal_expression(out, static_cast<exprs::literal*>(node));
		case exprs::expr_type::VARIABLE:
			return translate_variable_expression(out, static_cast<exprs::variable*>(node));
	}
	return out;
}

extern std::ostream& translator::translate_assign_expression(std::ostream& out, parser::exprs::assign* node) {
	return out;
}

extern std::ostream& translator::translate_binary_expression(std::ostream& out, parser::exprs::binary* node) {
	translate_expression(out, node->left);
	translate_expression(out, node->right);

	out << "pop rbx\npop rax\n";
	switch (node->operation.type) {
		case lexer::token_type::PLUS:
			out << "add rax, rbx\n";
			break;
		case lexer::token_type::MINUS:
			out << "sub rax, rbx\n";
			break;
		case lexer::token_type::STAR:
			out << "imul rax, rbx\n";
			break;
		case lexer::token_type::SLASH:
			out << "idiv rbx\n";
			break;
		case lexer::token_type::EQUAL_EQUAL:
			out << "mov rdx, 0\ncmp rax, rbx\njne .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 1\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		case lexer::token_type::BANG_EQUAL:
			out << "mov rdx, 1\ncmp rax, rbx\njne .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 0\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		case lexer::token_type::GREATER_EQUAL:
			out << "mov rdx, 0\ncmp rax, rbx\njl .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 1\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		case lexer::token_type::LESS:
			out << "mov rdx, 1\ncmp rax, rbx\njl .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 0\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		case lexer::token_type::LESS_EQUAL:
			out << "mov rdx, 0\ncmp rax, rbx\njg .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 1\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		case lexer::token_type::GREATER:
			out << "mov rdx, 1\ncmp rax, rbx\njg .cmp_stmt_" << ++_index << '\n';
			out << "mov rdx, 0\n.cmp_stmt_" << _index << ":\nmov rax, rdx\n";
			break;
		default: break;
	}
	return out << "push rax\n";
}

extern std::ostream& translator::translate_unary_expression(std::ostream& out, parser::exprs::unary* node) {
	translate_expression(out, node->target);
	out << "pop rax\n";
	switch (node->operation.type) {
		case lexer::token_type::MINUS:
			out << "neg rax\n";
			break;
		case lexer::token_type::BANG:
			out << "not rax\n";
			break;
		default: break;
	}
	return out << "push rax\n";
}

extern std::ostream& translator::translate_grouping_expression(std::ostream& out, parser::exprs::grouping* node) {
	return translate_expression(out, node->target);
}

extern std::ostream& translator::translate_variable_expression(std::ostream& out, parser::exprs::variable* node) {
	return out << "push " << std::string(node->name.start, node->name.size) << "\n";
}

extern std::ostream& translator::translate_literal_expression(std::ostream& out, parser::exprs::literal* node) {
	return out << "push " << std::string(node->value.start, node->value.size) << "\n";
}
