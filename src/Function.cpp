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

#include <cassert>
#include <iostream>
#include "thekogans/util/Exception.h"
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Function.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                // Believe it or not, but just declaring map static
                // does not guarantee proper ctor call order!? Wrapping
                // it in an accessor function does.
                Function::Map &GetMap () {
                    static Function::Map map;
                    return map;
                }

                void SkipSpaces (util::Buffer &buffer) {
                    while (buffer.GetDataAvailableForReading () > 0 &&
                            isspace (*buffer.GetReadPtr ())) {
                        buffer.AdvanceReadOffset (1);
                    }
                }

                bool GetToken (
                        util::Buffer &buffer,
                        util::ui32 token) {
                    SkipSpaces (buffer);
                    if (buffer.GetDataAvailableForReading () > 0 &&
                            *buffer.GetReadPtr () == token) {
                        buffer.AdvanceReadOffset (1);
                        return true;
                    }
                    return false;
                }

                util::ui32 ParseIndex (util::Buffer &buffer) {
                    std::string index;
                    SkipSpaces (buffer);
                    while (buffer.GetDataAvailableForReading () > 0) {
                        util::ui8 ch;
                        buffer >> ch;
                        if (isdigit (ch)) {
                            index += ch;
                        }
                        else {
                            --buffer.readOffset;
                            break;
                        }
                    }
                    if (!GetToken (buffer, ']')) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Invalid index in: %s (near %u)",
                            buffer.data,
                            buffer.readOffset);
                    }
                    return atoi (index.c_str ());
                }

                Function::Identifier ParseIdentifier (
                        const thekogans_make &config,
                        util::Buffer &buffer) {
                    Function::Identifier identifier (std::string (), util::NIDX32);
                    SkipSpaces (buffer);
                    while (buffer.GetDataAvailableForReading () > 0) {
                        util::i8 ch;
                        buffer >> ch;
                        switch (ch) {
                            case '\\': {
                                buffer >> ch;
                                if (IsEscapableCh (ch)) {
                                    identifier.first += ch;
                                }
                                else {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Invalid escape sequence in identifier in: %s (near %u)",
                                        buffer.data,
                                        buffer.readOffset);
                                }
                                break;
                            }
                            case '\'':
                            case '"': {
                                identifier.first += ParseQuotedString (config, buffer, ch);
                                break;
                            }
                            case '[': {
                                identifier.second = ParseIndex (buffer);
                                return identifier;
                            }
                            case '$': {
                                identifier.first +=
                                    Function::ParseAndExec (config, buffer).ToString ();
                                break;
                            }
                            default: {
                                if (identifier.first.empty ()) {
                                    if (isalpha (ch) || ch == '_') {
                                        identifier.first += ch;
                                    }
                                    else {
                                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                            "Invalid identifier in: %s (near %u)",
                                            buffer.data,
                                            buffer.readOffset);
                                    }
                                }
                                else {
                                    if (isalnum (ch) || ch == '_') {
                                        identifier.first += ch;
                                    }
                                    else {
                                        if (isspace (ch) && GetToken (buffer, '[')) {
                                            identifier.second = ParseIndex (buffer);
                                        }
                                        else {
                                            --buffer.readOffset;
                                        }
                                        return identifier;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    return identifier;
                }

                std::string ParseOption (
                        const thekogans_make &config,
                        util::Buffer &buffer) {
                    std::string option;
                    while (buffer.GetDataAvailableForReading () > 0) {
                        util::i8 ch;
                        buffer >> ch;
                        if (ch == ':' || ch == ')' || isspace (ch)) {
                            --buffer.readOffset;
                            break;
                        }
                        switch (ch) {
                            case '\\': {
                                buffer >> ch;
                                if (IsEscapableCh (ch)) {
                                    option += ch;
                                }
                                else {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Invalid escape sequence in option in: %s (near %u)",
                                        buffer.data,
                                        buffer.readOffset);
                                }
                                break;
                            }
                            case '\'':
                            case '"': {
                                option += ParseQuotedString (config, buffer, ch);
                                break;
                            }
                            case '$': {
                                option += Function::ParseAndExec (config, buffer).ToString ();
                                break;
                            }
                            default: {
                                option += ch;
                                break;
                            }
                        }
                    }
                    return option;
                }

                std::string ParseValue (
                        const thekogans_make &config,
                        util::Buffer &buffer) {
                    std::string value;
                    while (buffer.GetDataAvailableForReading () > 0) {
                        util::i8 ch;
                        buffer >> ch;
                        if (ch == ')' || isspace (ch)) {
                            --buffer.readOffset;
                            break;
                        }
                        switch (ch) {
                            case '\\': {
                                buffer >> ch;
                                if (IsEscapableCh (ch)) {
                                    value += ch;
                                }
                                else {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Invalid escape sequence in value in: %s (near %u)",
                                        buffer.data,
                                        buffer.readOffset);
                                }
                                break;
                            }
                            case '\'':
                            case '"': {
                                value += ParseQuotedString (config, buffer, ch);
                                break;
                            }
                            case '$': {
                                value += Function::ParseAndExec (config, buffer).ToString ();
                                break;
                            }
                            default: {
                                value += ch;
                                break;
                            }
                        }
                    }
                    return value;
                }

                Function::Parameter ParseParameter (
                        const thekogans_make &config,
                        util::Buffer &buffer) {
                    std::string option;
                    std::string value;
                    if (GetToken (buffer, '-')) {
                        option = ParseOption (config, buffer);
                        if (option.empty ()) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Empty option in: %s (near %u)",
                                buffer.data,
                                buffer.readOffset);
                        }
                        if (GetToken (buffer, ':')) {
                            value = ParseValue (config, buffer);
                        }
                    }
                    return Function::Parameter (option, value);
                }

                Function::Identifier Parse (
                        const thekogans_make &config,
                        util::Buffer &buffer,
                        Function::Parameters &parameters) {
                    Function::Identifier identifier = ParseIdentifier (config, buffer);
                    if (!identifier.first.empty () && identifier.second == util::NIDX32) {
                        while (1) {
                            Function::Parameter parameter = ParseParameter (config, buffer);
                            if (parameter.first.empty ()) {
                                break;
                            }
                            parameters.push_back (parameter);
                        }
                    }
                    return identifier;
                }
            }

            Value Function::ParseAndExec (
                    const thekogans_make &config,
                    util::Buffer &buffer) {
                if (GetToken (buffer, '(')) {
                    Parameters parameters;
                    Identifier identifier = Parse (config, buffer, parameters);
                    if (!GetToken (buffer, ')')) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Syntax error, missing ')': %s (near %u)",
                            buffer.data,
                            buffer.readOffset);
                    }
                    return Exec (config, identifier, parameters);
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Syntax error, missing '(': %s (near %u)",
                        buffer.data,
                        buffer.readOffset);
                }
            }

            Value Function::Exec (
                    const thekogans_make &config,
                    const Identifier &identifier,
                    const Parameters &parameters) {
                Value result;
                {
                    Map::iterator it = GetMap ().find (identifier.first);
                    if (it != GetMap ().end ()) {
                        UniquePtr function = it->second ();
                        result = function->Exec (config, parameters);
                    }
                    else if (parameters.empty ()) {
                        result = config.LookupSymbol (identifier.first);
                        if (identifier.second != util::NIDX32) {
                            result = identifier.second < result.value.size () ?
                                Value (result.type, result.value[identifier.second]) : Value ();
                        }
                    }
                }
                return result;
            }

            Function::MapInitializer::MapInitializer (
                    const std::string &name,
                    Factory factory) {
                std::pair<Map::iterator, bool> result =
                    GetMap ().insert (Map::value_type (name, factory));
                assert (result.second);
                if (!result.second) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Duplicate Function: %s", name.c_str ());
                }
                it = result.first;
            }

            Function::MapInitializer::~MapInitializer () {
                GetMap ().erase (it);
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
