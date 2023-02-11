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

#ifndef THALIA_PARSER_AST_VIEW
#define THALIA_PARSER_AST_VIEW

#include "parser/exprs.hpp"
#include "parser/stmts.hpp"
#include "parser/ast_visitor.hpp"

namespace thalia::parser {
	class ast_view: public ast_visitor {
		public:
			explicit ast_view(std::vector<stmts::statement*> target): ast_visitor(target) {}

			void print();

		private:
			[[nodiscard]] std::string tab(std::size_t deep = 0) const;

			void print_name(const char* category, const char* name);

			void print_statement(stmts::statement* node, std::size_t deep = 0);
			void print_print_statement(stmts::print* node, std::size_t deep = 0);
			void print_block_statement(stmts::block* node, std::size_t deep = 0);
			void print_program_statement(stmts::program* node, std::size_t deep = 0);
			void print_expression_statement(stmts::expression* node, std::size_t deep = 0);
			void print_var_statement(stmts::var* node, std::size_t deep = 0);
			void print_if_statement(stmts::if_* node, std::size_t deep = 0);
			void print_while_statement(stmts::while_* node, std::size_t deep = 0);
			void print_each_statement(stmts::each* node, std::size_t deep = 0);

			void print_expression(exprs::expression* node, std::size_t deep = 0);
			void print_assign_expression(exprs::assign* node, std::size_t deep = 0);
			void print_binary_expression(exprs::binary* node, std::size_t deep = 0);
			void print_unary_expression(exprs::unary* node, std::size_t deep = 0);
			void print_literal_expression(exprs::literal* node, std::size_t deep = 0);
			void print_variable_expression(exprs::variable* node, std::size_t deep = 0);
			void print_grouping_expression(exprs::grouping* node, std::size_t deep = 0);
	};
}

#endif // THALIA_PARSER_AST_VIEW
