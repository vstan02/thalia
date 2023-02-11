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

#include "parser/ast_deallocator.hpp"

using namespace thalia::parser;

extern void ast_deallocator::dealloc() {
	for (stmts::statement* item: _target) {
		dealloc_statement(item);
	}
}

extern void ast_deallocator::dealloc_statement(stmts::statement* node) {
	if (node == nullptr) return;
	switch (node->type) {
		case stmts::stmt_type::EXPRESSION:
			return dealloc_expression_statement(static_cast<stmts::expression*>(node));
		case stmts::stmt_type::BLOCK:
			return dealloc_block_statement(static_cast<stmts::block*>(node));
		case stmts::stmt_type::PRINT:
			return dealloc_print_statement(static_cast<stmts::print*>(node));
		case stmts::stmt_type::PROGRAM:
			return dealloc_program_statement(static_cast<stmts::program*>(node));
		case stmts::stmt_type::IF:
			return dealloc_if_statement(static_cast<stmts::if_*>(node));
		case stmts::stmt_type::WHILE:
			return dealloc_while_statement(static_cast<stmts::while_*>(node));
		case stmts::stmt_type::EACH:	
			return dealloc_each_statement(static_cast<stmts::each*>(node));
		default:
			delete node;
	}
}

extern void ast_deallocator::dealloc_expression_statement(stmts::expression* node) {
	dealloc_expression(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_block_statement(stmts::block* node) {
	for (stmts::statement* item: node->stmts) {
		dealloc_statement(item);
	}
	delete node;
}

extern void ast_deallocator::dealloc_program_statement(stmts::program* node) {
	dealloc_statement(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_if_statement(stmts::if_* node) {
	dealloc_expression(node->condition);
	dealloc_statement(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_while_statement(stmts::while_* node) {
	dealloc_expression(node->condition);
	dealloc_statement(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_each_statement(stmts::each* node) {
	if (node->from) {
		dealloc_expression(node->from);
	}

	dealloc_expression(node->variable);
	dealloc_expression(node->to);
	dealloc_statement(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_print_statement(stmts::print* node) {
	for (exprs::expression* item: node->target) {
		dealloc_expression(item);
	}
	delete node;
}

extern void ast_deallocator::dealloc_expression(exprs::expression* node) {
	if (node == nullptr) return;
	switch (node->type) {
		case exprs::expr_type::ASSIGN:
			return dealloc_assign_expression(static_cast<exprs::assign*>(node));
		case exprs::expr_type::BINARY:
			return dealloc_binary_expression(static_cast<exprs::binary*>(node));
		case exprs::expr_type::UNARY:
			return dealloc_unary_expression(static_cast<exprs::unary*>(node));
		case exprs::expr_type::GROUPING:
			return dealloc_grouping_expression(static_cast<exprs::grouping*>(node));
		default:
			delete node;
	}
}

extern void ast_deallocator::dealloc_assign_expression(exprs::assign* node) {
	dealloc_expression(node->name);
	dealloc_expression(node->value);
	delete node;
}

extern void ast_deallocator::dealloc_binary_expression(exprs::binary* node) {
	dealloc_expression(node->left);
	dealloc_expression(node->right);
	delete node;
}

extern void ast_deallocator::dealloc_unary_expression(exprs::unary* node) {
	dealloc_expression(node->target);
	delete node;
}

extern void ast_deallocator::dealloc_grouping_expression(exprs::grouping* node) {
	dealloc_expression(node->target);
	delete node;
}
