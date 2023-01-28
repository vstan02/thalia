/* Exprs - Thalia expressions
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

#ifndef THALIA_PARSER_EXPRS
#define THALIA_PARSER_EXPRS

#include <memory>

#include "lexer/token.hpp"

namespace thalia::parser::exprs {
	enum expr_type {
		ASSIGN,
		BINARY,
		UNARY,
		GROUPING,
		LITERAL,
		VARIABLE
	};

	struct expression {
		expr_type type;

		expression(expr_type type): type(type) {}
	};

	struct assign: expression {
		lexer::token name;
		expression* value;

		assign(lexer::token name, expression* value)
			: expression(expr_type::ASSIGN), name(name), value(value) {}
	};

	struct binary: expression {
		expression* left;
		expression* right;
		lexer::token operation;

		binary(lexer::token op, expression* left, expression* right)
			: expression(expr_type::BINARY), left(left), right(right), operation(op) {}
	};

	struct grouping: expression {
		expression* target;

		grouping(expression* target)
			: expression(expr_type::GROUPING), target(target) {}
	};

	struct literal: expression {
		lexer::token value;

		literal(lexer::token value)
			: expression(expr_type::LITERAL), value(value) {}
	};

	struct variable: expression {
		lexer::token name;

		variable(lexer::token name)
			: expression(expr_type::VARIABLE), name(name) {}
	};

	struct unary: expression {
		expression* target;
		lexer::token operation;

		unary(lexer::token operation, expression* target)
			: expression(expr_type::UNARY), target(target), operation(operation) {}
	};
}

#endif // THALIA_PARSER_EXPRS
