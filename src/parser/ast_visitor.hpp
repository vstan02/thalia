/* Ast visitor - Thalia syntax tree visitor
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

#ifndef THALIA_PARSER_AST_VISITOR
#define THALIA_PARSER_AST_VISITOR

#include "parser/exprs.hpp"

namespace thalia::parser {
	class ast_visitor {
		public:
			explicit ast_visitor(exprs::expression* target): _target(target) {}

		protected:
			exprs::expression* _target;
	};
}

#endif // THALIA_PARSER_AST_VISITOR
