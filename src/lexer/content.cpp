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

namespace thalia::lexer {
	content::content(char const* target)
		: _line(1), _start(target), _current(target) {}

	std::size_t content::size() const {
		return _current - _start;
	}

	char const* content::word() const {
		return _start;
	}

	char content::operator[](std::size_t index) const {
		return _current[index];
	}

	void content::operator>>(std::size_t size) {
		_start += size;
	}

	char content::advance() {
		return (++_current)[-1];
	}

	void content::start_word() {
		_start = _current;
	}

	std::size_t content::line() const {
		return _line;
	}

	bool content::is_eol() const {
		return (*this)[0] == '\n';
	}

	bool content::is_eof() const {
		return (*this)[0] == '\0';
	}

	void content::skip_whitespaces() {
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

	void content::skip_comments() {
		if ((*this)[1] == '/') {
			while ((*this)[1] != '\n') advance();
		} else {
			while ((*this)[0] != '*' || (*this)[1] != '/') advance();
			advance();
		}
		advance();
	}
}
