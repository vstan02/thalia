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
#include <fstream>
#include <exception>

#include <lexer/lexer.hpp>
#include <lexer/exception.hpp>

#include <parser/parser.hpp>
#include <parser/ast_deallocator.hpp>
#include <parser/ast_view.hpp>
#include <parser/exception.hpp>
// #include <nasm/translator.hpp>

extern int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "[Error]: Invalid file path.\n";
    return 0;
  }

  try {
    std::ifstream file(argv[1]);
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    using namespace thalia;
    lexer::lexer lexer(code.c_str());

    parser::parser parser(lexer);
    std::vector<parser::stmts::statement*> ast = parser.parse();
    parser::ast_deallocator deallocator(ast);

    // nasm::translator translator(ast);
    // translator.translate(std::cout);

    parser::ast_view view(ast);
    view.print();

    deallocator.dealloc();
  } catch (const thalia::lexer::exception& error) {
    std::cerr << "[Lexing error]: " << error.what() << '\n';
    std::cerr << "===> at line " << error.where() << '\n';
  } catch (const thalia::parser::exception& error) {
    std::cerr << "[Parsing error]: " << error.what() << '\n';
    std::cerr << "===> at line " << error.where() << '\n';
  } catch (const std::exception& error) {
    std::cerr << "[Error]: " << error.what() << '\n';
  }

	return 0;
}
