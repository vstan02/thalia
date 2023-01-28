/* Parser - Thalia syntax analyzer
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

#ifndef THALIA_PARSER_PARSER
#define THALIA_PARSER_PARSER

#include <cstddef>
#include <vector>
#include <memory>

#include "lexer/lexer.hpp"
#include "parser/exprs.hpp"

namespace thalia::parser {
	class parser {
		public:
			explicit parser(const lexer::lexer& lexer)
				: _target(lexer), _index(0), _tokens() {}

			exprs::expression* parse();

		private:
			lexer::lexer _target;
			std::size_t _index;
			std::vector<lexer::token> _tokens;

		private:
			bool match(std::vector<lexer::token_type>);
			void consume(lexer::token_type, const char*);

			exprs::expression* primary_expression();
			exprs::expression* unary_expression();
			exprs::expression* factor_expression();
			exprs::expression* term_expression();
			exprs::expression* comparison_expression();
			exprs::expression* equality_expression();

			exprs::expression* expression() { return equality_expression(); }

			void advance() { !at_end() && (++_index); }

			bool at_end() const { return _tokens[_index].type == lexer::token_type::END; }
			bool check(lexer::token_type type) const { return !at_end() && _tokens[_index].type == type; }
	};
}

#endif // THALIA_PARSER_PARSER
