/* Translator - Thalia AST-to-Nasm translator
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

#ifndef THALIA_NASM_TRANSLATOR
#define THALIA_NASM_TRANSLATOR

#include <ostream>

#include "parser/exprs.hpp"
#include "parser/stmts.hpp"
#include "parser/ast_visitor.hpp"

namespace thalia::nasm {
	class translator: public parser::ast_visitor {
		public:
			explicit translator(std::vector<parser::stmts::statement*> target)
				: ast_visitor(target), _index(0) {}

			std::ostream& translate(std::ostream& out);

		private:
			std::size_t _index;

		private:
			std::ostream& translate_statement(std::ostream& out, parser::stmts::statement* node);
			std::ostream& translate_program_statement(std::ostream& out, parser::stmts::program* node);
			std::ostream& translate_block_statement(std::ostream& out, parser::stmts::block* node);
			std::ostream& translate_print_statement(std::ostream& out, parser::stmts::print* node);
			std::ostream& translate_expression_statement(std::ostream& out, parser::stmts::expression* node);

			std::ostream& translate_expression(std::ostream& out, parser::exprs::expression* node);
			std::ostream& translate_assign_expression(std::ostream& out, parser::exprs::assign* node);
			std::ostream& translate_binary_expression(std::ostream& out, parser::exprs::binary* node);
			std::ostream& translate_unary_expression(std::ostream& out, parser::exprs::unary* node);
			std::ostream& translate_literal_expression(std::ostream& out, parser::exprs::literal* node);
			std::ostream& translate_variable_expression(std::ostream& out, parser::exprs::variable* node);
			std::ostream& translate_grouping_expression(std::ostream& out, parser::exprs::grouping* node);
	};
}

#endif // THALIA_NASM_TRANSLATOR
