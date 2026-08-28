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
#include <vector>
#include "thekogans/util/StringUtils.h"
#include "thekogans/util/Path.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Package.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                void GetPackagePaths (std::vector<std::string> &paths) {
                    std::string::size_type start = 0;
                    std::string::size_type end = _TOOLCHAIN_PKG_CONFIG_PATH.find_first_of (":", start);
                    while (end != std::string::npos) {
                        std::string path = util::TrimSpaces (
                            _TOOLCHAIN_PKG_CONFIG_PATH.substr (start, end - start).c_str ());
                        if (!path.empty ()) {
                            paths.push_back (path);
                        }
                        start = end + 1;
                        end = _TOOLCHAIN_PKG_CONFIG_PATH.find_first_of (":", start);
                    }
                    std::string path = util::TrimSpaces (
                        _TOOLCHAIN_PKG_CONFIG_PATH.substr (start).c_str ());
                    if (!path.empty ()) {
                        paths.push_back (path);
                    }
                }
            }

            Package::Package (
                    const std::string &path_,
                    const std::string &name_,
                    const std::string &version_,
                    const std::string &config_,
                    const std::string &type_) :
                    path (path_),
                    name (name_),
                    version (version_),
                    config (config_),
                    type (type_) {
                std::vector<std::string> paths;
                if (path.empty ()) {
                    GetPackagePaths (paths);
                }
                else {
                    paths.push_back (path);
                }
                for (auto path : paths) {
                    std::ifstream file (ToSystemPath (MakePath (path, name + ".pc")));
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
                                    // Resolve variables inside the property value
                                    std::string rawValue = ResolveVariables (util::TrimSpaces (line.substr (colonPos + 1).c_str ()));
                                    properties[key] = rawValue;
                                }
                                else if (equalPos != std::string::npos) {
                                    // It's a variable
                                    std::string key = util::TrimSpaces (line.substr (0, equalPos).c_str ());
                                    // Variables can reference previously defined variables
                                    std::string rawValue = ResolveVariables (util::TrimSpaces (line.substr (equalPos + 1).c_str ()));
                                    variables[key] = rawValue;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            Package::SharedPtr Package::GetConfig (
                    const std::string &path,
                    const std::string &name,
                    const std::string &version,
                    const std::string &config,
                    const std::string &type) {
                return new Package (path, name, version, config, type);
            }

            bool Package::IsInstalled (
                    const std::string &package,
                    const std::string &version) {
                std::vector<std::string> paths;
                GetPackagePaths (paths);
                for (auto path : paths) {
                    if (util::Path (ToSystemPath (MakePath (path, package + ".pc"))).Exists ()) {
                        return true;
                    }
                }
                return false;
            }

            void Package::GetLibs (std::set<std::string> &libs) const {
                Properties::const_iterator it = properties.find ("Libs");
                if (it != properties.end ()) {
                    libs.insert (it->second);
                }
                if (type == TYPE_STATIC) {
                    Properties::const_iterator it = properties.find ("Libs.private");
                    if (it != properties.end ()) {
                        libs.insert (it->second);
                    }
                }
            }

            void Package::GetCFlags (std::set<std::string> &c_flags) const {
                Properties::const_iterator it = properties.find ("Cflags");
                if (it != properties.end ()) {
                    c_flags.insert (it->second);
                }
                if (type == TYPE_STATIC) {
                    Properties::const_iterator it = properties.find ("Cflags.private");
                    if (it != properties.end ()) {
                        c_flags.insert (it->second);
                    }
                }
            }

            // Recursively resolves ${variable} blocks within a string
            std::string Package::ResolveVariables (std::string value) {
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
