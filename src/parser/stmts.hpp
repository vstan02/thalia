/* Stmts - Thalia statements
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

#ifndef THALIA_PARSER_STMTS
#define THALIA_PARSER_STMTS

#include <memory>
#include <vector>

#include "lexer/token.hpp"
#include "parser/exprs.hpp"

namespace thalia::parser::stmts {
	enum stmt_type {
		PROGRAM,
		EXPRESSION,
		BLOCK,
		PRINT,
    VAR,
    IF,
    WHILE,
    EACH
	};

	struct statement {
		stmt_type type;

		statement(stmt_type type): type(type) {}
	};

	struct expression: statement {
		exprs::expression* target;

		expression(exprs::expression* target)
			: statement(stmt_type::EXPRESSION), target(target) {}
	};

  struct print: statement {
    std::vector<exprs::expression*> target;

    print(std::vector<exprs::expression*> target)
      : statement(stmt_type::PRINT), target(target) {}
  };

  struct block: statement {
    std::vector<stmts::statement*> stmts;

    block(std::vector<stmts::statement*> stmts)
      : statement(stmt_type::BLOCK), stmts(stmts) {}
  };

  struct program: statement {
    statement* target;

    program(statement* block)
      : statement(stmt_type::PROGRAM), target(block) {}
  };

  struct var: statement {
    lexer::token name;

    var(lexer::token name)
      : statement(stmt_type::VAR), name(name) {}
  };

  struct if_: statement {
    exprs::expression* condition;
    statement* target;

    if_(exprs::expression* condition, statement* block)
      : statement(stmt_type::IF), condition(condition), target(block) {}
  };

  struct while_: statement {
    exprs::expression* condition;
    statement* target;

    while_(exprs::expression* condition, statement* block)
      : statement(stmt_type::WHILE), condition(condition), target(block) {}
  };

  struct each: statement {
    exprs::expression* variable;
    exprs::expression* from;
    exprs::expression* to;
    statement* target;

    each(exprs::expression* variable, exprs::expression* from, exprs::expression* to, statement* block)
      : statement(stmt_type::EACH), variable(variable), from(from), to(to), target(block) {}
  };
}

#endif // THALIA_PARSER_STMTS
