/* Thalia - A fast, general-purpose programming language
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

#include <iostream>

#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <parser/ast_deallocator.hpp>
#include <nasm/translator.hpp>

extern int main() {
  using namespace thalia;
  lexer::lexer lexer("-PI + (-3 * 45) - 7 / 2 * 9 + 5 >= 5 * 2 - (3 + (4 + 78) * 3) + 1 * 4");

  parser::parser parser(lexer);
  parser::exprs::expression* ast = parser.parse();
  parser::ast_deallocator deallocator(ast);

  nasm::translator translator(ast);
  translator.translate(std::cout);

  deallocator.dealloc();
	return 0;
}
