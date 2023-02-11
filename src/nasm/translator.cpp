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

static char NASM_DATA[] = ""
	"section .data\n"
	"\tfalse equ 0\n"
	"\ttrue equ 1\n"
	"\tnull equ 0\n"
	"\t_SYS_EXIT_ equ 0x3c\n"
	"\t_SYS_WRITE_ equ 0x1\n"
	"\t_SYS_STDOUT_ equ 1\n";

static char NASM_BSS[] = ""
	"\nsection .bss\n"
	"\t_CHR_BSS_ resb 1\n";

static char NASM_END[] = ""
	"int_print:\n"
	"\tpush rax\n"
	"\tpush rbx\n"
	"\tpush rcx\n"
	"\tpush rdx\n"
	"\txor rcx, rcx\n"
	"\tcmp rax, 0\n"
	"\tjge .decomp\n"
	".negate:\n"
	"\tmov rdx, rax\n"
	"\tmov rax, '-'\n"
	"\tcall chr_print\n"
	"\tmov rax, rdx\n"
	"\tneg rax\n"
	".decomp:\n"
	"\tmov rbx, 10\n"
	"\txor rdx, rdx\n"
	"\tdiv rbx\n"
	"\tadd rdx, '0'\n"
	"\tpush rdx\n"
	"\tinc rcx\n"
	"\tcmp rax, 0\n"
	"\tje .print\n"
	"\tjmp .decomp\n"
	".print:\n"
	"\tcmp rcx, 0\n"
	"\tje .close\n"
	"\tdec rcx\n"
	"\tpop rax\n"
	"\tcall chr_print\n"
	"\tjmp .print\n"
	".close:\n"
	"\tpop rdx\n"
	"\tpop rcx\n"
	"\tpop rbx\n"
	"\tpop rax\n"
	"\tret\n\n"
	"eol_print:\n"
	"\tpush rax\n"
	"\tmov rax, 0xA\n"
	"\tcall chr_print\n"
	"\tpop rax\n"
	"\tret\n\n"
	"chr_print:\n"
	"\tpush rax\n"
	"\tpush rdi\n"
	"\tpush rsi\n"
	"\tpush rdx\n"
	"\tpush rcx\n"
	"\tmov [_CHR_BSS_], al\n"
	"\tmov rax, _SYS_WRITE_\n"
	"\tmov rdi, _SYS_STDOUT_\n"
	"\tmov rsi, _CHR_BSS_\n"
	"\tmov rdx, 1\n"
	"\tsyscall\n"
	"\tpop rcx\n"
	"\tpop rdx\n"
	"\tpop rsi\n"
	"\tpop rdi\n"
	"\tpop rax\n"
	"\tret\n\n"
	"sys_exit:\n"
	"\tmov rax, _SYS_EXIT_\n"
	"\txor rdi, rdi\n"
	"\tsyscall\n";

extern std::ostream& translator::translate(std::ostream& out) {
	out << "global _start\n\n";
	out << NASM_DATA;
	parser::stmts::statement* program = nullptr;
	for (parser::stmts::statement* item: _target) {
		if (item->type == parser::stmts::stmt_type::PROGRAM) {
			program = item;
			continue;
		}
		translate_statement(out, item);
	}
	out << NASM_BSS;
	translate_statement(out, program);
	return out << NASM_END;
}

extern std::ostream& translator::translate_statement(std::ostream& out, parser::stmts::statement* node) {
	if (node == nullptr) return out;
	using namespace parser;
	switch (node->type) {
		case stmts::stmt_type::VAR:
			return translate_var_statement(out, static_cast<stmts::var*>(node));
		case stmts::stmt_type::PROGRAM:
			return translate_program_statement(out, static_cast<stmts::program*>(node));
		case stmts::stmt_type::BLOCK:
			return translate_block_statement(out, static_cast<stmts::block*>(node));
		case stmts::stmt_type::PRINT:
			return translate_print_statement(out, static_cast<stmts::print*>(node));
		case stmts::stmt_type::IF:
			return translate_if_statement(out, static_cast<stmts::if_*>(node));
		case stmts::stmt_type::WHILE:
			return translate_while_statement(out, static_cast<stmts::while_*>(node));
		case stmts::stmt_type::EACH:	
			return translate_each_statement(out, static_cast<stmts::each*>(node));
		case stmts::stmt_type::EXPRESSION:
			return translate_expression_statement(out, static_cast<stmts::expression*>(node));
	}
	return out;
}

extern std::ostream& translator::translate_var_statement(std::ostream& out, parser::stmts::var* node) {
	return out << '\t' << std::string(node->name.start, node->name.size) << " dq 0\n";
}

extern std::ostream& translator::translate_if_statement(std::ostream& out, parser::stmts::if_* node) {
	translate_expression(out, node->condition);
	out << "\ttest rax, rax\n\tje .cmp_stmt_" << ++_index << '\n';
	translate_statement(out, node->target);
	return out << ".cmp_stmt_" << _index << ":\n";
}

extern std::ostream& translator::translate_while_statement(std::ostream& out, parser::stmts::while_* node) {
	std::size_t index = ++_index;
	out << ".while_stmt_" << index << ":\n";
	translate_expression(out, node->condition);
	out << "\ttest rax, rax\n\tje .cmp_stmt_" << index << '\n';
	translate_statement(out, node->target);
	return out << "\tjmp .while_stmt_" << index << "\n.cmp_stmt_" << index << ":\n";
}

extern std::ostream& translator::translate_each_statement(std::ostream& out, parser::stmts::each* node) {
	parser::exprs::variable* variable = static_cast<parser::exprs::variable*>(node->variable);
	
	if (node->from) {
		translate_expression(out, node->from);
	} else {
		out << "\tpush 0\n";
	}

	out << "\tpop qword [" << std::string(variable->name.start, variable->name.size) << "]\n";

	std::size_t index = ++_index;
	out << ".each_stmt_" << index << ":\n";
	translate_variable_expression(out, variable);
	translate_expression(out, node->to);
	out << "\tpop rbx\n\tpop rax\n\tcmp rax, rbx\n\tjge .cmp_stmt_" << index << '\n';
	translate_statement(out, node->target);
	out << "\tinc qword [" << std::string(variable->name.start, variable->name.size);
	return out << "]\n\tjmp .each_stmt_" << index << "\n.cmp_stmt_" << index << ":\n";
}

extern std::ostream& translator::translate_program_statement(std::ostream& out, parser::stmts::program* node) {
	return translate_statement(out << "\nsection .text\n_start:\n", node->target) << "\tcall sys_exit\n\n";
}

extern std::ostream& translator::translate_block_statement(std::ostream& out, parser::stmts::block* node) {
	for (parser::stmts::statement* item: node->stmts) {
		translate_statement(out, item);
	}
	return out;
}

extern std::ostream& translator::translate_print_statement(std::ostream& out, parser::stmts::print* node) {
	for (parser::exprs::expression* item: node->target) {
		translate_expression(out, item) << "\tpop rax\n\tcall int_print\n\tmov rax, ' '\n\tcall chr_print\n";
	}
	return out << "\tcall eol_print\n";
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
	parser::exprs::variable* variable = static_cast<parser::exprs::variable*>(node->name);
	translate_expression(out, node->value);
	out << "\tpop qword [" << std::string(variable->name.start, variable->name.size) << "]\n";
	return translate_variable_expression(out, variable);
}

extern std::ostream& translator::translate_binary_expression(std::ostream& out, parser::exprs::binary* node) {
	translate_expression(out, node->left);
	translate_expression(out, node->right);

	out << "\tpop rbx\n\tpop rax\n";
	switch (node->operation.type) {
		case lexer::token_type::PLUS:
			out << "\tadd rax, rbx\n";
			break;
		case lexer::token_type::MINUS:
			out << "\tsub rax, rbx\n";
			break;
		case lexer::token_type::STAR:
			out << "\timul rax, rbx\n";
			break;
		case lexer::token_type::SLASH:
			out << "\tidiv rbx\n";
			break;
		case lexer::token_type::PERCENT:
			out << "\txor rdx, rdx\n\tidiv rbx\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::EQUAL_EQUAL:
			out << "\tmov rdx, 0\n\tcmp rax, rbx\n\tjne .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 1\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::BANG_EQUAL:
			out << "\tmov rdx, 1\n\tcmp rax, rbx\n\tjne .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 0\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::GREATER_EQUAL:
			out << "\tmov rdx, 0\n\tcmp rax, rbx\n\tjl .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 1\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::LESS:
			out << "\tmov rdx, 1\n\tcmp rax, rbx\n\tjl .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 0\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::LESS_EQUAL:
			out << "\tmov rdx, 0\n\tcmp rax, rbx\n\tjg .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 1\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		case lexer::token_type::GREATER:
			out << "\tmov rdx, 1\n\tcmp rax, rbx\n\tjg .cmp_stmt_" << ++_index << '\n';
			out << "\tmov rdx, 0\n.cmp_stmt_" << _index << ":\n\tmov rax, rdx\n";
			break;
		default: break;
	}
	return out << "\tpush rax\n";
}

extern std::ostream& translator::translate_unary_expression(std::ostream& out, parser::exprs::unary* node) {
	translate_expression(out, node->target);
	out << "\tpop rax\n";
	switch (node->operation.type) {
		case lexer::token_type::MINUS:
			out << "\tneg rax\n";
			break;
		case lexer::token_type::BANG:
			out << "\tnot rax\n";
			break;
		default: break;
	}
	return out << "\tpush rax\n";
}

extern std::ostream& translator::translate_grouping_expression(std::ostream& out, parser::exprs::grouping* node) {
	return translate_expression(out, node->target);
}

extern std::ostream& translator::translate_variable_expression(std::ostream& out, parser::exprs::variable* node) {
	return out << "\tpush qword [" << std::string(node->name.start, node->name.size) << "]\n";
}

extern std::ostream& translator::translate_literal_expression(std::ostream& out, parser::exprs::literal* node) {
	return out << "\tpush " << std::string(node->value.start, node->value.size) << "\n";
}
