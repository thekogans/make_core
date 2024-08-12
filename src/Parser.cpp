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

#include "thekogans/util/StringUtils.h"
#include "thekogans/util/Exception.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/util/Version.h"
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Function.h"
#include "thekogans/make/core/Parser.h"

namespace thekogans {
    namespace make {
        namespace core {

            Tokenizer::Tokenizer (
                    const char *expression_,
                    const core::thekogans_make &config_) :
                    expression (expression_),
                    config (config_) {
                if (expression == 0) {
                    THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                        THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                }
            }

            Tokenizer::Token Tokenizer::GetToken () {
                if (!stack.empty ()) {
                    Token token = stack.back ();
                    stack.pop_back ();
                    return token;
                }
                else {
                    while (*expression != 0 && isspace (*expression)) {
                        ++expression;
                    }
                    switch (*expression) {
                        case '\0': {
                            return Token (Token::END);
                        }
                        case '|': {
                            ++expression;
                            if (*expression == '|') {
                                ++expression;
                                return Token (Token::OR);
                            }
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s", "Missing '|'.");
                        }
                        case '&': {
                            ++expression;
                            if (*expression == '&') {
                                ++expression;
                                return Token (Token::AND);
                            }
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s", "Missing '&'.");
                        }
                        case '=': {
                            ++expression;
                            if (*expression == '=') {
                                ++expression;
                                return Token (Token::EQ);
                            }
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s", "Missing '='.");
                        }
                        case '!': {
                            ++expression;
                            if (*expression == '=') {
                                ++expression;
                                return Token (Token::NE);
                            }
                            return Token (Token::NOT);
                        }
                        case '<': {
                            ++expression;
                            if (*expression == '=') {
                                ++expression;
                                return Token (Token::LE);
                            }
                            return Token (Token::LT);
                        }
                        case '>': {
                            ++expression;
                            if (*expression == '=') {
                                ++expression;
                                return Token (Token::GE);
                            }
                            return Token (Token::GT);
                        }
                        case '(': {
                            ++expression;
                            return Token (Token::LP);
                        }
                        case ')': {
                            ++expression;
                            return Token (Token::RP);
                        }
                        case '\'': {
                            ++expression;
                            std::string value;
                            while (*expression != '\0' && *expression != '\'') {
                                switch (*expression) {
                                    case '\\':
                                        ++expression;
                                        if (*expression != 0 && IsEscapableCh (*expression)) {
                                            value += *expression++;
                                        }
                                        else {
                                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                                "Invalid escape char: %d", *expression);
                                        }
                                        break;
                                    case '$': {
                                        ++expression;
                                        util::TenantBuffer buffer (
                                            util::HostEndian,
                                            const_cast<char *> (expression),
                                            strlen (expression));
                                        value += Function::ParseAndExec (config, buffer).ToString ();
                                        expression += buffer.readOffset;
                                        break;
                                    }
                                    default:
                                        value += *expression++;
                                        break;
                                }
                            }
                            if (*expression != 0) {
                                assert (*expression == '\'');
                                ++expression;
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "%s", "Missing '\''.");
                            }
                            return Token (Token::VALUE, Value (value));
                        }
                        case '$': {
                            ++expression;
                            util::TenantBuffer buffer (
                                util::HostEndian,
                                const_cast<char *> (expression),
                                strlen (expression));
                            Value value = Function::ParseAndExec (config, buffer);
                            expression += buffer.readOffset;
                            return Token (Token::VALUE, value);
                        }
                        default: {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Invalid char: %d", *expression);
                        }
                    }
                }
                return Token ();
            }

            namespace {
                bool Eval (const Tokenizer::Token &token) {
                    switch (token.value.type) {
                        case Value::TYPE_Unknown:
                            return false;
                        case Value::TYPE_bool:
                            return token.value.ToString () == VALUE_TRUE;
                        case Value::TYPE_int:
                            return util::stringToi32 (token.value.ToString ().c_str ()) != 0;
                        case Value::TYPE_float:
                            return util::stringTof32 (token.value.ToString ().c_str ()) != 0.0f;
                        case Value::TYPE_string:
                        case Value::TYPE_GUID:
                        case Value::TYPE_Version:
                            return !token.value.ToString ().empty ();
                    }
                    return false;
                }

                bool operator == (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) ==
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) ==
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) ==
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () == right.value.ToString ();
                }

                bool operator != (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) !=
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) !=
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) !=
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () != right.value.ToString ();
                }

                bool operator < (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_bool ||
                            right.value.type == Value::TYPE_bool) {
                        return left.value.ToString () == VALUE_FALSE &&
                            right.value.ToString () == VALUE_TRUE;
                    }
                    else if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) <
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) <
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) <
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () < right.value.ToString ();
                }

                bool operator > (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_bool ||
                            right.value.type == Value::TYPE_bool) {
                        return left.value.ToString () == VALUE_TRUE &&
                            right.value.ToString () == VALUE_FALSE;
                    }
                    else if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) >
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) >
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) >
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () > right.value.ToString ();
                }

                bool operator <= (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_bool ||
                            right.value.type == Value::TYPE_bool) {
                        return left.value.ToString () == VALUE_FALSE ||
                            right.value.ToString () == VALUE_TRUE;
                    }
                    else if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) <=
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) <=
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) <=
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () <= right.value.ToString ();
                }

                bool operator >= (
                        const Tokenizer::Token &left,
                        const Tokenizer::Token &right) {
                    if (left.value.type == Value::TYPE_bool ||
                            right.value.type == Value::TYPE_bool) {
                        return left.value.ToString () == VALUE_TRUE ||
                            right.value.ToString () == VALUE_FALSE;
                    }
                    else if (left.value.type == Value::TYPE_int ||
                            right.value.type == Value::TYPE_int) {
                        return util::stringToi32 (left.value.ToString ().c_str ()) >=
                            util::stringToi32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_float ||
                            right.value.type == Value::TYPE_float) {
                        return util::stringTof32 (left.value.ToString ().c_str ()) >=
                            util::stringTof32 (right.value.ToString ().c_str ());
                    }
                    else if (left.value.type == Value::TYPE_Version ||
                            right.value.type == Value::TYPE_Version) {
                        return util::Version (left.value.ToString ()) >=
                            util::Version (right.value.ToString ());
                    }
                    return left.value.ToString () >= right.value.ToString ();
                }
            }

            bool Parser::Parse () {
                return LogicalOrExpression ();
            }

            bool Parser::LogicalOrExpression () {
                bool result = LogicalAndExpression ();
                Tokenizer::Token token;
                for (token = tokenizer.GetToken ();
                        token.type == Tokenizer::Token::OR;
                        token = tokenizer.GetToken ()) {
                    result |= LogicalAndExpression ();
                }
                tokenizer.PushBack (token);
                return result;
            }

            bool Parser::LogicalAndExpression () {
                bool result = RelationalExpression ();
                Tokenizer::Token token;
                for (token = tokenizer.GetToken ();
                        token.type == Tokenizer::Token::AND;
                        token = tokenizer.GetToken ()) {
                    result &= RelationalExpression ();
                }
                tokenizer.PushBack (token);
                return result;
            }

            bool Parser::RelationalExpression () {
                Tokenizer::Token left = PrimaryExpression ();
                if (left.type == Tokenizer::Token::LP) {
                    bool result = LogicalOrExpression ();
                    Tokenizer::Token right = PrimaryExpression ();
                    if (right.type != Tokenizer::Token::RP) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "%s", "Missing ')'.");
                    }
                    return result;
                }
                Tokenizer::Token op = tokenizer.GetToken ();
                switch (op.type) {
                    case Tokenizer::Token::EQ: {
                        return left == PrimaryExpression ();
                    }
                    case Tokenizer::Token::NE: {
                        return left != PrimaryExpression ();
                    }
                    case Tokenizer::Token::LT: {
                        return left < PrimaryExpression ();
                    }
                    case Tokenizer::Token::GT: {
                        return left > PrimaryExpression ();
                    }
                    case Tokenizer::Token::LE: {
                        return left <= PrimaryExpression ();
                    }
                    case Tokenizer::Token::GE: {
                        return left >= PrimaryExpression ();
                    }
                    default: {
                        tokenizer.PushBack (op);
                        if (left.type == Tokenizer::Token::VALUE) {
                            return Eval (left);
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s", "Expecting value.");
                        }
                    }
                }
            }

            Tokenizer::Token Parser::PrimaryExpression () {
                Tokenizer::Token token = tokenizer.GetToken ();
                if (token.type == Tokenizer::Token::NOT) {
                    bool result;
                    token = tokenizer.GetToken ();
                    if (token.type == Tokenizer::Token::LP) {
                        result = LogicalOrExpression ();
                        Tokenizer::Token right = tokenizer.GetToken ();
                        if (right.type != Tokenizer::Token::RP) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s", "Missing ')'.");
                        }
                    }
                    else {
                        result = Eval (token);
                    }
                    token = Tokenizer::Token (
                        Tokenizer::Token::VALUE,
                        Value (!result));
                }
                return token;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
