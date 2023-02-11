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

#include <utility>

#include "parser/parser.hpp"
#include "parser/exception.hpp"

using namespace thalia::parser;

extern std::vector<stmts::statement*> parser::parse() {
	if (_tokens.empty()) {
		_tokens = _target.scan();
	}
	
	if (_tokens.size() < 2) {
		return _ast;
	}

	_index = 1;

	if (_ast.empty()) {
		while (!at_end()) {
			_ast.push_back(declaration());
		}
	}
	return _ast;
}

extern stmts::statement* parser::declaration() {
	using lexer::token_type;
	switch (_tokens[_index].type) {
		case token_type::VAR:
			return var_declaration();
		default:
			return program_declaration();
	}
}

extern stmts::statement* parser::var_declaration() {
	advance();
	lexer::token token = _tokens[_index];
	advance();
	consume(lexer::token_type::SEMICOLON, "Expect ';' after value.");
	return new stmts::var(token);
}

extern stmts::statement* parser::program_declaration() {
	consume(lexer::token_type::PROGRAM, "Expect 'program'.");
	if (!check(lexer::token_type::LEFT_BRACE)) {
		throw exception("Expect '{' after 'program'.", _tokens[_index].line);
	}
	return new stmts::program(block_statement());
}

extern stmts::statement* parser::print_statement() {
	advance();
	std::vector<exprs::expression*> target;
	target.push_back(expression());
	while(check(lexer::token_type::COMMA)) {
		advance();
		target.push_back(expression());
	}
	consume(lexer::token_type::SEMICOLON, "Expect ';' after value.");
	return new stmts::print(target);
}

extern stmts::statement* parser::block_statement() {
	advance();
	std::vector<stmts::statement*> stmts;
	while (!check(lexer::token_type::RIGHT_BRACE)) {
		stmts.push_back(statement());
	}
	consume(lexer::token_type::RIGHT_BRACE, "Expect '}' after block.");
	return new stmts::block(stmts);
}

extern stmts::statement* parser::if_statement() {
	advance();
	exprs::expression* condition = expression();
	if (!check(lexer::token_type::LEFT_BRACE)) {
		throw exception("Expect '{' after 'if'.", _tokens[_index].line);
	}
	return new stmts::if_(condition, block_statement());
}

extern stmts::statement* parser::while_statement() {
	advance();
	exprs::expression* condition = expression();
	if (!check(lexer::token_type::LEFT_BRACE)) {
		throw exception("Expect '{' after 'while'.", _tokens[_index].line);
	}
	return new stmts::while_(condition, block_statement());
}

extern stmts::statement* parser::each_statement() {
	using lexer::token_type;
	advance();
	consume(token_type::IDENTIFIER, "Expect a variable name after 'each'.");
	exprs::variable* variable = new exprs::variable(_tokens[_index - 1]);
	
	consume(token_type::IN, "Expect the keyword 'in' after the variable in 'each'.");
	consume(token_type::LEFT_BRACKET, "Expect '[' after 'in'.");
	exprs::expression* to = expression();

	exprs::expression* from = nullptr;
	if (check(token_type::COMMA)) {
		advance();
		from = expression();
		std::swap(from, to);
	}
	
	consume(token_type::RIGHT_BRACKET, "Expect ']' after 'each' interval.");
	return new stmts::each(variable, from, to, block_statement());
}

extern stmts::statement* parser::expression_statement() {
	exprs::expression* target = expression();
	consume(lexer::token_type::SEMICOLON, "Expect ';' after value.");
	return new stmts::expression(target);
}

extern stmts::statement* parser::statement() {
	using lexer::token_type;
	switch (_tokens[_index].type) {
		case token_type::PRINT:
			return print_statement();
		case token_type::LEFT_BRACE:
			return block_statement();
		case token_type::IF:
			return if_statement();
		case token_type::WHILE:
			return while_statement();
		case token_type::EACH:
			return each_statement();
		default:
			return expression_statement();
	}
}

extern exprs::expression* parser::assign_expression() {
	using lexer::token_type;
	exprs::expression* result = equality_expression();

	if (!match({
		token_type::EQUAL,
		token_type::PLUS_EQUAL, 
		token_type::MINUS_EQUAL,
		token_type::SLASH_EQUAL,
		token_type::STAR_EQUAL,
		token_type::PERCENT_EQUAL 
	})) {
		return result;
	}

	if (result->type != exprs::expr_type::VARIABLE) {
		throw exception("Invalid assignment target.", _tokens[_index - 2].line);
	}

	lexer::token token = _tokens[_index - 1];
	if (token.type == token_type::EQUAL) {
		return new exprs::assign(result, expression());
	}

	token.type = to_operation(token.type);
	exprs::expression* value = new exprs::binary(token, result, expression());
	exprs::expression* variable = new exprs::variable((static_cast<exprs::variable*>(result))->name);
	return new exprs::assign(variable, value);
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
	while (match({ token_type::STAR, token_type::SLASH, token_type::PERCENT })) {
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

extern thalia::lexer::token_type parser::to_operation(lexer::token_type type) {
	using lexer::token_type;
	switch (type) {
		case token_type::PLUS_EQUAL:	
			return token_type::PLUS;
		case token_type::MINUS_EQUAL:	
			return token_type::MINUS;
		case token_type::STAR_EQUAL:	
			return token_type::STAR;
		case token_type::SLASH_EQUAL:	
			return token_type::SLASH;
		case token_type::PERCENT_EQUAL:	
			return token_type::PERCENT;
		default:
			return token_type::END;
	}
}
