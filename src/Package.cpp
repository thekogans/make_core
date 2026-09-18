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

#include <string>
#include "thekogans/util/Version.h"
#include "thekogans/make/core/Package.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                struct Tokenizer {
                    const char *expression;
                    struct Token {
                        enum Type {
                            END,              // end of expression
                            IDENTIFIER,
                            OP,
                            VERSION
                        } type;
                        std::string value;

                        Token (
                            Type type_ = END,
                            const std::string &value_ = std::string ()) :
                            type (type_),
                            value (value_) {}
                    };

                    Tokenizer (const char *expression_) :
                        expression (expression_) {}

                    Token GetToken () {
                        while (*expression != '\0' && isspace (*expression)) {
                            ++expression;
                        }
                        switch (*expression) {
                            case '\0': {
                                return Token (Token::END);
                            }
                            case '=': {
                                ++expression;
                                return Token (Token::OP, "=");
                            }
                            case '!': {
                                ++expression;
                                if (*expression == '=') {
                                    ++expression;
                                    return Token (Token::OP, "!=");
                                }
                                else {
                                    // FIXME: throw
                                }
                            }
                            case '<': {
                                ++expression;
                                if (*expression == '=') {
                                    ++expression;
                                    return Token (Token::OP, "<=");
                                }
                                return Token (Token::OP, "<");
                            }
                            case '>': {
                                ++expression;
                                if (*expression == '=') {
                                    ++expression;
                                    return Token (Token::OP, ">=");
                                }
                                return Token (Token::OP, ">");
                            }
                            default: {
                                if (isalpha (*expression)) {
                                    Token token (Token::IDENTIFIER);
                                    while (*expression != '\0' && !isspace (*expression)) {
                                        token.value += *expression++;
                                    }
                                    return token;
                                }
                                else if (isdigit (*expression)) {
                                    return Token (Token::VERSION, util::Version (expression).ToString ());
                                }
                                else {
                                    // FIXME: throw
                                }
                            }
                        }
                        return Token ();
                    }
                };
            }

            Package::Constraint Package::Constraint::Parse (const std::string &requirement) {
                Package::Constraint constraint;
                if (!requirement.empty ()) {
                    Tokenizer tokenizer (requirement.c_str ());
                    Tokenizer::Token token = tokenizer.GetToken ();
                    if (token.type == Tokenizer::Token::IDENTIFIER) {
                        constraint.name = token.value;
                        token = tokenizer.GetToken ();
                        if (token.type == Tokenizer::Token::OP) {
                            constraint.op = util::Version::stringToOP (token.value);
                            token = tokenizer.GetToken ();
                            if (token.type == Tokenizer::Token::VERSION) {
                                constraint.version = util::Version (token.value);
                            }
                            else {
                                // FIXME: throw something.
                            }
                        }
                    }
                }
                return constraint;
            }

            std::string Package::Constraint::ToString () const {
                std::string value = name;
                if (op != util::Version::NOP) {
                    value += " " + util::Version::OPTostring (op) + " " + version.ToString ();
                }
                return value;
            }

            bool Package::Constraint::MatchesRequirement (const Constraint &requirement) const {
                if (name == requirement.name) {
                    if (op == util::Version::NOP || requirement.op == util::Version::NOP ||
                            version.IsEmpty () || requirement.version.IsEmpty ()) {
                        return true;
                    }
                    if (op == util::Version::EQ) {
                        return
                            version.SatisfiesConstraint (requirement.op, requirement.version);
                    }
                    if (op == util::Version::NEQ) {
                        if (requirement.op == util::Version::EQ) {
                            return version.Compare (requirement.version) != 0;
                        }
                        if (requirement.op == util::Version::NEQ) {
                            return version.Compare (requirement.version) == 0;
                        }
                        return true;
                    }
                    if (op == util::Version::GEQ) {
                        if (requirement.op == util::Version::EQ) {
                            return version.Compare (requirement.version) <= 0;
                        }
                        if (requirement.op == util::Version::LEQ) {
                            return version.Compare (requirement.version) <= 0;
                        }
                        if (requirement.op == util::Version::LT) {
                            return version.Compare (requirement.version) < 0;
                        }
                        return true;
                    }
                    if (op == util::Version::LEQ) {
                        if (requirement.op == util::Version::EQ) {
                            return version.Compare (requirement.version) >= 0;
                        }
                        if (requirement.op == util::Version::GEQ) {
                            return version.Compare (requirement.version) >= 0;
                        }
                        if (requirement.op == util::Version::GT) {
                            return version.Compare (requirement.version) > 0;
                        }
                    }
                    if (op == util::Version::GT) {
                        if (requirement.op == util::Version::EQ) {
                            return version.Compare (requirement.version) < 0;
                        }
                        if (requirement.op == util::Version::LEQ) {
                            return version.Compare (requirement.version) < 0;
                        }
                        if (requirement.op == util::Version::LT) {
                            return version.Compare (requirement.version) < 0;
                        }
                        return true;
                    }
                    if (op == util::Version::LT) {
                        if (requirement.op == util::Version::EQ) {
                            return version.Compare (requirement.version) > 0;
                        }
                        if (requirement.op == util::Version::GEQ) {
                            return version.Compare (requirement.version) > 0;
                        }
                        if (requirement.op == util::Version::GT) {
                            return version.Compare (requirement.version) > 0;
                        }
                        return true;
                    }
                }
                return false;
            }

            int Package::CompareVersion (const util::Version &targetVersion) const {
                return version.Compare (targetVersion);
            }

            bool Package::SatisfiesConstraint (
                    util::Version::OP op,
                    const util::Version &targetVersion) const {
                return version.SatisfiesConstraint (op, targetVersion);
            }

            bool Package::MatchesRequirement (const Constraint &requirement) const {
                if (fileName == requirement.name) {
                    return SatisfiesConstraint (requirement.op, requirement.version);
                }
                for (const auto &capability : provides) {
                    if (capability.MatchesRequirement (requirement)) {
                        return true;
                    }
                }
                return false;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
