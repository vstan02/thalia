/* AST View - Thalia syntax tree printer
 * Copyright (C) 2023 Stan Vlad <vstan02@protonmail.com>
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

#include <string>
#include <iostream>
#include <termcolor/termcolor.hpp>

#include "parser/ast_view.hpp"

using namespace thalia::parser;

extern void ast_view::print() {
	std::size_t index = 0;
	std::cout << "[";
	for (parser::stmts::statement* node: _target) {
		if (index++ > 0) {
			std::cout << ',';
		}
		print_statement(node, 1);
	}

	if (index > 0 && index == _target.size()) {
		std::cout << '\n';
	}
	std::cout << "]\n";
}

extern void ast_view::print_statement(stmts::statement *node, std::size_t deep) {
	using type = stmts::stmt_type;
	if (node == nullptr) return;
	std::cout << '\n';
	switch (node->type) {
		case type::PROGRAM:
			return print_program_statement(static_cast<stmts::program*>(node), deep);
		case type::PRINT:
			return print_print_statement(static_cast<stmts::print*>(node), deep);
		case type::BLOCK:
			return print_block_statement(static_cast<stmts::block*>(node), deep);
		case type::EXPRESSION:
			return print_expression_statement(static_cast<stmts::expression*>(node), deep);
	}
}

extern void ast_view::print_expression_statement(stmts::expression* node, std::size_t deep) {
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Statement", "Expression");
	std::cout << termcolor::bright_white << " {\n";
	print_expression(node->target, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern void ast_view::print_print_statement(stmts::print* node, std::size_t deep) {
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Statement", "Print");
	std::cout << termcolor::bright_white << " [";
	
	std::size_t index = 0;
	for (parser::exprs::expression* expr: node->target) {
		if (index++ > 0) {
			std::cout << ',';
		}
		std::cout << '\n';
		print_expression(expr, deep + 1);
	}

	if (index > 0 && index == node->target.size()) {
		std::cout << '\n';
	}
	std::cout << tab(deep) << ']';
}

extern void ast_view::print_block_statement(stmts::block* node, std::size_t deep) {
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Statement", "Block");
	std::cout << termcolor::bright_white << " [";
	
	std::size_t index = 0;
	for (parser::stmts::statement* stmt: node->stmts) {
		if (index++ > 0) {
			std::cout << ',';
		}
		print_statement(stmt, deep + 1);
	}

	if (index > 0 && index == node->stmts.size()) {
		std::cout << '\n';
	}
	std::cout << tab(deep) << ']';
}

extern void ast_view::print_program_statement(stmts::program* node, std::size_t deep) {
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Statement", "Program");
	std::cout << termcolor::bright_white << " {";
	print_statement(node->target, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern void ast_view::print_expression(exprs::expression* node, std::size_t deep) {
	using type = exprs::expr_type;
	if (node == nullptr) return;
	switch (node->type) {
		case type::ASSIGN:
			return print_assign_expression(static_cast<exprs::assign*>(node), deep);
		case type::BINARY:
			return print_binary_expression(static_cast<exprs::binary*>(node), deep);
		case type::UNARY:
			return print_unary_expression(static_cast<exprs::unary*>(node), deep);
		case type::GROUPING:
			return print_grouping_expression(static_cast<exprs::grouping*>(node), deep);
		case type::LITERAL:
			return print_literal_expression(static_cast<exprs::literal*>(node), deep);
		case type::VARIABLE:
			return print_variable_expression(static_cast<exprs::variable*>(node), deep);
	}
}

extern void ast_view::print_assign_expression(exprs::assign* node, std::size_t deep) {
	std::string name(node->name.start, node->name.size);
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Assign");
	std::cout << termcolor::bright_white << " {\n" << tab(deep + 1) << "'" << name << "',\n";
	print_expression(node->value, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern void ast_view::print_binary_expression(exprs::binary* node, std::size_t deep) {
	std::string op(node->operation.start, node->operation.size);
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Binary");
	std::cout << termcolor::bright_white << " {\n" << tab(deep + 1) << "'" << op << "',\n";
	print_expression(node->left, deep + 1);
	std::cout << ",\n";
	print_expression(node->right, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern void ast_view::print_unary_expression(exprs::unary* node, std::size_t deep) {
	std::string op(node->operation.start, node->operation.size);
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Unary");
	std::cout << termcolor::bright_white << " {\n" << tab(deep + 1) << "'" << op << "',\n";
	print_expression(node->target, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern void ast_view::print_literal_expression(exprs::literal* node, std::size_t deep) {
	std::string value(node->value.start, node->value.size);
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Literal");
	std::cout << termcolor::bright_white << " { '" << value << "' }";
}

extern void ast_view::print_variable_expression(exprs::variable* node, std::size_t deep) {
	std::string name(node->name.start, node->name.size);
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Variable");
	std::cout << termcolor::bright_white << " { '" << name << "' }";
}

extern void ast_view::print_grouping_expression(exprs::grouping* node, std::size_t deep) {
	std::cout << termcolor::bright_white << tab(deep);
	print_name("Expression", "Grouping");
	std::cout << termcolor::bright_white << " {\n";
	print_expression(node->target, deep + 1);
	std::cout << '\n' << tab(deep) << '}';
}

extern std::string ast_view::tab(std::size_t deep) const {
	return std::string(deep * 2, ' ');
}

extern void ast_view::print_name(const char* category, const char* name) {
	std::cout << termcolor::bright_blue << category << termcolor::bright_white << "::" << termcolor::bright_yellow << name;
}
