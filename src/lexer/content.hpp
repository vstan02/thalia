/* Content - Content for the lexical analyzer
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

#ifndef THALIA_LEXER_CONTENT
#define THALIA_LEXER_CONTENT

#include <cstddef>

namespace thalia::lexer {
	class content {
		public:
			content(char const* target);

			std::size_t line() const;

			bool is_eof() const;
			bool is_eol() const;

			void skip_whitespaces();

			char const* word() const;
			std::size_t size() const;

			void start_word();
			char advance();

			char operator[](std::size_t index) const;
			void operator>>(std::size_t index);

		private:
			std::size_t _line;
			char const* _start;
			char const* _current;

		private:
			void skip_comments();
	};
}

#endif // THALIA_LEXER_CONTENT
