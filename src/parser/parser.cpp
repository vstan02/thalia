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

#include <iostream>

#include "parser/parser.hpp"
#include "parser/exception.hpp"

using namespace thalia::parser;

extern exprs::expression* parser::parse() {
	if (_tokens.empty()) {
		_tokens = _target.scan();
	}

	if (_tokens.size() < 2) {
		return nullptr;
	}

	_index = 1;
	return equality_expression();
}

extern exprs::expression* parser::equality_expression() {
	using lexer::token_type;
	exprs::expression* result = comparison_expression();
	while (match({ token_type::EQUAL_EQUAL, token_type::BANG_EQUAL })) {
		lexer::token token = _tokens[_index - 1];
		result = new exprs::binary(token, result, comparison_expression());
	}
	return result;
}

extern exprs::expression* parser::comparison_expression() {
	using lexer::token_type;
	exprs::expression* result = term_expression();
	while (match({ token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL })) {
		lexer::token token = _tokens[_index - 1];
		result = new exprs::binary(token, result, term_expression());
	}
	return result;
}

extern exprs::expression* parser::term_expression() {
	using lexer::token_type;
	exprs::expression* result = factor_expression();
	while (match({ token_type::PLUS, token_type::MINUS })) {
		lexer::token token = _tokens[_index - 1];
		result = new exprs::binary(token, result, factor_expression());
	}
	return result;
}

extern exprs::expression* parser::factor_expression() {
	using lexer::token_type;
	exprs::expression* result = unary_expression();
	while (match({ token_type::STAR, token_type::SLASH })) {
		lexer::token token = _tokens[_index - 1];
		result = new exprs::binary(token, result, unary_expression());
	}
	return result;
}

extern exprs::expression* parser::unary_expression() {
	using lexer::token_type;
	if (match({ token_type::BANG, token_type::MINUS })) {
		lexer::token token = _tokens[_index - 1];
		return new exprs::unary(token, unary_expression());
	}
	return primary_expression();
}

extern exprs::expression* parser::primary_expression() {
	using lexer::token_type;
	if (check(token_type::IDENTIFIER)) {
		advance();
		return new exprs::variable(_tokens[_index - 1]);
	}

	if (check(token_type::LEFT_PAREN)) {
		advance();
		exprs::expression* result = expression();
		consume(lexer::token_type::RIGHT_PAREN, "Expect ')' after expression.");
		return new exprs::grouping(result);
	}

	advance();
	return new exprs::literal(_tokens[_index - 1]);
}

extern bool parser::match(std::vector<lexer::token_type> types) {
	for (lexer::token_type type: types) {
		if (check(type)) {
			advance();
			return true;
		}
	}
	return false;
}

extern void parser::consume(lexer::token_type type, const char* message) {
	if (check(type)) {
		return advance();
	}
	throw exception(message, _tokens[_index].line);
}
