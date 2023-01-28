/* Lexer - Thalia lexical analyzer
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

#include <cstring>

#include "lexer/lexer.hpp"
#include "lexer/exception.hpp"

using namespace thalia::lexer;

struct keyword {
	char const* value;
	std::size_t size;
	token_type type;
};

static const keyword keywords[4] = {
	{ "true", 4, token_type::TRUE },
	{ "false", 5, token_type::FALSE },
	{ "null", 4, token_type::NONE },
	{ "program", 7, token_type::PROGRAM }
};

extern std::vector<token> lexer::scan() {
	if (_tokens.empty()) {
		token current;
		_tokens.push_back(make_token(token_type::START));
		do {
			current = next_token();
			_tokens.push_back(current);
		} while(current.type != token_type::END);
	}

	return _tokens;
}

extern token lexer::next_token() {
	_code.skip_whitespaces();
	_code.start_word();

	if (_code.is_eof()) {
		return make_token(token_type::END);
	}

	char ch = _code.advance();
	if (is_alpha(ch)) {
		return make_id();
	}

	if (is_digit(ch)) {
		return make_number();
	}

	switch (ch) {
		case '(': return make_token(token_type::LEFT_PAREN);
		case ')': return make_token(token_type::RIGHT_PAREN);
		case '{': return make_token(token_type::LEFT_BRACE);
		case '}': return make_token(token_type::RIGHT_BRACE);
		case '[': return make_token(token_type::LEFT_BRACKET);
		case ']': return make_token(token_type::RIGHT_BRACKET);
		case ',': return make_token(token_type::COMMA);
		case '.': return make_token(token_type::DOT);
		case ';': return make_token(token_type::SEMICOLON);
		case '-': return make_token(token_type::MINUS);
		case '+': return make_token(token_type::PLUS);
		case '/': return make_token(token_type::SLASH);
		case '*': return make_token(token_type::STAR);
		case '%': return make_token(token_type::PERCENT);
		case '=': return choose_token('=', token_type::EQUAL_EQUAL, token_type::EQUAL);
		case '!': return choose_token('=', token_type::BANG_EQUAL, token_type::BANG);
		case '<': return choose_token('=', token_type::LESS_EQUAL, token_type::LESS);
		case '>': return choose_token('=', token_type::GREATER_EQUAL, token_type::GREATER);
		case '|': return assert_token('|', token_type::OR);
		case '&': return assert_token('&', token_type::AND);
		default: throw unexpected_character(_code.line());
	}
}

extern token lexer::choose_token(char expected, token_type first, token_type second) {
	if (_code[0] != expected) {
		return make_token(second);
	}

	_code.advance();
	return make_token(first);
}

extern token lexer::assert_token(char expected, token_type type) {
	if (_code[0] == expected) {
		throw unexpected_character(_code.line());
	}

	_code.advance();
	return make_token(type);
}

extern token lexer::make_number() {
	while (is_digit(_code[0])) _code.advance();
	if (_code[0] == '.' && is_digit(_code[1])) {
		_code.advance();
		while (is_digit(_code[0])) _code.advance();
	}
	return make_token(token_type::NUMBER);
}

extern token lexer::make_id() {
	while (is_alphanum(_code[0])) _code.advance();
	return make_token(id_token());
}

extern token_type lexer::id_token() {
	std::size_t size = _code.size();
	for (const auto& keyword: keywords) {
		if (size == keyword.size && std::memcmp(_code.word(), keyword.value, size) == 0) {
			return keyword.type;
		}
	}
	return token_type::IDENTIFIER;
}
