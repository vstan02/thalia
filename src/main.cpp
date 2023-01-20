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
#include <string>

#include <lexer/lexer.hpp>

extern int main() {
  using namespace thalia;
  lexer::lexer lexer("23 + 56");
  std::vector<lexer::token> tokens = lexer.scan();

  std::cout << "Tokens (" << tokens.size() << "):\n";
  for (const lexer::token& token: tokens) {
  	std::cout << "=> line[" << token.line << "] type["
  		<< token.type <<  "] -> \""
  		<< std::string(token.start, token.size) << "\"\n";
  }
	return 0;
}
