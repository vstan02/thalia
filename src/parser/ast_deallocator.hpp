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

#ifndef THALIA_PARSER_AST_DEALLOCATOR
#define THALIA_PARSER_AST_DEALLOCATOR

#include "parser/ast_visitor.hpp"

namespace thalia::parser {
	class ast_deallocator: public ast_visitor {
		public:
			explicit ast_deallocator(std::vector<stmts::statement*> target)
				: ast_visitor(target) {}

			void dealloc();

		private:
      void dealloc_statement(stmts::statement* node);
      void dealloc_program_statement(stmts::program* node);
      void dealloc_block_statement(stmts::block* node);
      void dealloc_print_statement(stmts::print* node);
      void dealloc_expression_statement(stmts::expression* node);
      void dealloc_if_statement(stmts::if_* node);
      void dealloc_while_statement(stmts::while_* node);
      void dealloc_each_statement(stmts::each* node);

			void dealloc_expression(exprs::expression* node);
			void dealloc_assign_expression(exprs::assign* node);
			void dealloc_binary_expression(exprs::binary* node);
			void dealloc_unary_expression(exprs::unary* node);
			void dealloc_grouping_expression(exprs::grouping* node);
	};
}

#endif // THALIA_PARSER_AST_DEALLOCATOR
