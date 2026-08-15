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

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include "thekogans/util/StringUtils.h"
#include "thekogans/make/core/PkgConfig.h"

namespace thekogans {
    namespace make {
        namespace core {

            PkgConfig::PkgConfig (const std::string &path) {
                std::ifstream file (path);
                if (file.is_open ()) {
                    std::string line;
                    while (std::getline (file, line)) {
                        line = util::TrimSpaces  (line.c_str ());
                        // Skip empty lines and comments
                        if (!line.empty () && line[0] != '#') {
                            // Check for Property definition (Keyword: Value)
                            size_t colonPos = line.find (':');
                            // Check for Variable definition (key=value)
                            size_t equalPos = line.find ('=');
                            if (colonPos != std::string::npos &&
                                    (equalPos == std::string::npos || colonPos < equalPos)) {
                                // It's a property
                                std::string key = util::TrimSpaces (line.substr (0, colonPos).c_str ());
                                std::string rawValue = util::TrimSpaces (line.substr (colonPos + 1).c_str ());
                                // Resolve variables inside the property value
                                properties[key] = ResolveVariables (rawValue);
                            }
                            else if (equalPos != std::string::npos) {
                                // It's a variable
                                std::string key = util::TrimSpaces (line.substr (0, equalPos).c_str ());
                                std::string rawValue = util::TrimSpaces (line.substr (equalPos + 1).c_str ());
                                // Variables can reference previously defined variables
                                variables[key] = ResolveVariables (rawValue);
                            }
                        }
                    }
                }
            }

            // Recursively resolves ${variable} blocks within a string
            std::string PkgConfig::ResolveVariables (std::string value) {
                std::regex variableRegex (R"(\$\{([^}]+)\})");
                std::smatch match;
                // Max loop depth to prevent infinite recursion on self-referential variables
                int depth = 0;
                while (std::regex_search (value, match, variableRegex) && depth++ < 10) {
                    std::string fullMatch = match[0].str ();
                    std::string variable = match[1].str ();
                    auto it = variables.find (variable);
                    std::string replacement = it != variables.end () ? it->second : "";
                    // Replace the first occurrence of the variable match
                    size_t pos = value.find (fullMatch);
                    if (pos != std::string::npos) {
                        value.replace (pos, fullMatch.length (), replacement);
                    }
                }
                return value;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
