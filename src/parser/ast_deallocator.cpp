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
