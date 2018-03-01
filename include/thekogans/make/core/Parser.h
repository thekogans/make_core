// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of thekogans_make_core.
//
// thekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// thekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with thekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_core_Parser_h)
#define __thekogans_make_core_Parser_h

#include <string>
#include <list>
#include <set>
#include "thekogans/make/core/Config.h"
#include "thekogans/make/core/Function.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct thekogans_make;

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Tokenizer {
                const char *expression;
                const thekogans_make &config;
                struct Token {
                    enum Type {
                        END,              // end of expression
                        OR,               // '||'
                        AND,              // '&&'
                        EQ,               // '=='
                        NOT,              // '!'
                        NE,               // '!='
                        LT,               // '<'
                        GT,               // '>'
                        LE,               // '<='
                        GE,               // '>='
                        LP,               // '('
                        RP,               // ')'
                        VALUE             // string or function return
                    } mutable type;
                    Value value;

                    Token (Type type_ = END) :
                        type (type_) {}
                    Token (
                        Type type_,
                        const Value &value_) :
                        type (type_),
                        value (value_) {}
                };
                std::list<Token> stack;

                Tokenizer (
                    const char *expression_,
                    const core::thekogans_make &config_);

                Token GetToken ();

                void PushBack (const Token &token) {
                    stack.push_back (token);
                }
            };

            // Grammar:
            //
            // <logical-or-expression>  ::= <logical-and-expression>
            //                            | <logical-or-expression> || <logical-and-expression>
            // <logical-and-expression> ::= <relational-expression>
            //                            | <logical-and-expression> && <relational-expression>
            // <relational-expression>  ::= <primary-expression> == <primary-expression>
            //                            | <primary-expression> != <primary-expression>
            //                            | <primary-expression> < <primary-expression>
            //                            | <primary-expression> > <primary-expression>
            //                            | <primary-expression> <= <primary-expression>
            //                            | <primary-expression> >= <primary-expression>
            //                            | ( <logical-or-expression> )
            // <primary-expression>     ::= <literal>
            //                            | <function-call>
            // <function-call>          ::= $(identifier [arguments])
            struct _LIB_THEKOGANS_MAKE_CORE_DECL Parser {
                Tokenizer &tokenizer;
                const thekogans_make &config;

                Parser (
                    Tokenizer &tokenizer_,
                    const core::thekogans_make &config_) :
                    tokenizer (tokenizer_),
                    config (config_) {}

                bool Parse ();
                bool LogicalOrExpression ();
                bool LogicalAndExpression ();
                bool RelationalExpression ();
                Tokenizer::Token PrimaryExpression ();
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Parser_h)
