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

#include "lexer/content.hpp"

using namespace thalia::lexer;

extern void content::skip_whitespaces() {
	while (true) {
		switch ((*this)[0]) {
			case ' ':
			case '\r':
			case '\t': {
				advance();
				break;
			}
			case '\n': {
				++_line;
				advance();
				break;
			}
			case '/': {
				if ((*this)[1] != '/' && (*this)[1] != '*') return;
				skip_comments();
				break;
			}
			default: return;
		}
	}
}

extern void content::skip_comments() {
	if ((*this)[1] == '/') {
		while ((*this)[1] != '\n') advance();
	} else {
		while ((*this)[0] != '*' || (*this)[1] != '/') advance();
		advance();
	}
	advance();
}
