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

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "thekogans/util/Path.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/Exception.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/PackageResolver.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                std::string Trim (const std::string &text) {
                    auto start = std::find_if_not (text.begin (), text.end (), ::isspace);
                    auto end = std::find_if_not (text.rbegin (), text.rend (), ::isspace).base ();
                    return (start < end) ? std::string (start, end) : std::string ();
                }

                void Tokenize (
                        const std::string &text,
                        std::vector<std::string> &tokens,
                        const std::string &separators = " ") {
                    std::string trimmed = Trim (text);
                    std::string token;
                    token.reserve (trimmed.size ());
                    bool inQuotes = false;
                    bool isEscaped = false;
                    for (size_t i = 0; i < trimmed.size (); ++i) {
                        char ch = trimmed[i];
                        if (isEscaped) {
                            token += ch;
                            isEscaped = false;
                        }
                        else if (ch == '\\') {
                            isEscaped = true;
                        }
                        else if (ch == '"') {
                            inQuotes = !inQuotes;
                        }
                        else if (separators.find_first_of (ch) != std::string::npos && !inQuotes) {
                            if (!token.empty ()) {
                                tokens.push_back (token);
                                token.clear ();
                            }
                        }
                        else {
                            token += ch;
                        }
                    }
                    if (inQuotes) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("Unterminated \" in '%s'", text.c_str ());
                    }
                    if (isEscaped) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("Can't end on \\ in '%s'", text.c_str ());
                    }
                    if (!token.empty ()) {
                        tokens.push_back (token);
                    }
                }

                void ParseConstraints (
                        const std::string &text,
                        std::vector<Package::Constraint> &constraints) {
                    std::vector<std::string> tokens;
                    Tokenize (text, tokens, " ,");
                    size_t i = 0;
                    while (i < tokens.size ()) {
                        Package::Constraint constraint (tokens[i++]);
                        if (i < tokens.size ()) {
                            util::Version::OP op = util::Version::stringToOP (tokens[i]);
                            if (op != util::Version::NOP) {
                                constraint.op = op;
                                // consume operator
                                if (++i < tokens.size ()) {
                                    constraint.version = tokens[i++];
                                }
                                else {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Can't end on '%s' in '%s'", util::Version::OPTostring (op).c_str (), text.c_str ());
                                }
                            }
                        }
                        constraints.push_back (constraint);
                    }
                }

                std::string ResolveVariables (
                        std::string text,
                        const std::unordered_map<std::string, std::string> &variables) {
                    bool replaced = true;
                    while (replaced) {
                        replaced = false;
                        size_t start = 0;
                        while ((start = text.find ("${", start)) != std::string::npos) {
                            size_t end = text.find ("}", start);
                            if (end == std::string::npos) {
                                break;
                            }
                            std::string variable = text.substr (start + 2, end - (start + 2));
                            auto it = variables.find (variable);
                            if (it != variables.end ()) {
                                text.replace (start, end + 1 - start, it->second);
                                replaced = true;
                            }
                            else {
                                start = end + 1;
                            }
                        }
                    }
                    return text;
                }
            }

            PackageResolver::PackageResolver () {
                std::string::size_type start = 0;
                std::string::size_type end = _TOOLCHAIN_PKG_CONFIG_PATH.find_first_of (":", start);
                while (end != std::string::npos) {
                    std::string path = Trim (_TOOLCHAIN_PKG_CONFIG_PATH.substr (start, end - start));
                    if (!path.empty ()) {
                        searchPaths.push_back (path);
                    }
                    start = end + 1;
                    end = _TOOLCHAIN_PKG_CONFIG_PATH.find_first_of (":", start);
                }
                std::string path = Trim (_TOOLCHAIN_PKG_CONFIG_PATH.substr (start));
                if (!path.empty ()) {
                    searchPaths.push_back (path);
                }
                // FIXME: As per 'Provides' requirement stated here;
                // https://manpages.ubuntu.com/manpages/jammy/man5/pc.5.html,
                // we build a cache of all packages that can act as providers.
                // More often then not, this is a collosal waste of time and
                // will not be used in a build system. Perhaps implement some
                // sort of a switch to turn off this cache.
                for (const auto &path : searchPaths) {
                    if (util::Path (path).Exists ()) {
                        util::Directory directory (path);
                        util::Directory::Entry entry;
                        for (bool gotEntry = directory.GetFirstEntry (entry);
                                gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                            if (entry.type == util::Directory::Entry::File &&
                                    util::Path (entry.name).GetExtension () == "pc") {
                                ExtractProvides (ToSystemPath (MakePath (path, entry.name)));
                            }
                        }
                    }
                }
            }

            Package::SharedPtr PackageResolver::Resolve (
                    const Package::Constraint &requirement,
                    bool static_) {
                if (cache.count (requirement.name) != 0) {
                    return cache[requirement.name];
                }
                std::vector<Package::SharedPtr> packages;
                std::unordered_set<std::string> visited;
                std::string resolvedName = ResolveName (requirement, "PackageResolver");
                if (!resolvedName.empty () && dfs (resolvedName, static_, packages, visited)) {
                    std::vector<std::string> libs;
                    // Process libraries from leaf node dependencies up to the root (Bottom-Up)
                    for (auto package : packages) {
                        auto Collect = [&] (const std::vector<std::string> &flags) {
                            for (auto it = flags.rbegin (); it != flags.rend (); ++it) {
                                libs.push_back (*it);
                            }
                        };
                        Collect (package->libs_public);
                        if (static_) {
                            Collect (package->libs_private);
                        }
                    }
                    std::reverse (libs.begin (), libs.end ());
                    std::vector<std::string> cflags;
                    std::unordered_set<std::string> visited_cflags;
                    // Process Cflags from root node down to leaf dependencies (Top-Down)
                    for (auto it = packages.rbegin (); it != packages.rend (); ++it) {
                        auto Collect = [&] (const std::vector<std::string> &flags) {
                            for (const auto &flag : flags) {
                                if (visited_cflags.insert (flag).second) {
                                    cflags.push_back (flag);
                                }
                            }
                        };
                        Collect ((*it)->cflags_public);
                        if (static_) {
                            Collect ((*it)->cflags_private);
                        }
                    }
                    // Sort various known flags in to their own bins.
                    Package::SharedPtr package = cache[resolvedName];
                    for (const auto &cflag : cflags) {
                        if (cflag.rfind ("-I", 0) == 0) {
                            package->include_directories.push_back (cflag.substr (2));
                        }
                        else if (cflag.rfind ("-D", 0) == 0) {
                            package->preprocessor_definitions.push_back (cflag.substr (2));
                        }
                        else {
                            package->other_cflags.push_back (cflag);
                        }
                    }
                    std::unordered_set<std::string> visited_libs;
                    for (size_t i = 0, count = libs.size (); i < count; ++i) {
                        std::string prefix = libs[i].substr (0, 2);
                        if (libs[i].rfind ("-L", 0)  == 0) {
                            std::string library_directory = libs[i].substr (2);
                            if (visited_libs.insert (library_directory).second) {
                                package->library_directories.push_back (library_directory);
                            }
                        }
                        else if (libs[i].rfind ("-F", 0)  == 0) {
                            std::string framework_directory = libs[i].substr (2);
                            if (visited_libs.insert (framework_directory).second) {
                                package->framework_directories.push_back (framework_directory);
                            }
                        }
                        else if (libs[i].rfind ("-l", 0)  == 0) {
                            std::string library_name = libs[i].substr (2);
                            if (visited_libs.insert (library_name).second) {
                                package->library_names.push_back  (library_name);
                            }
                        }
                        else if (libs[i].rfind ("-framework", 0)  == 0) {
                            std::string framework_name = Trim (libs[i].substr (10));
                            if (framework_name.empty () && i + 1 < count) {
                                framework_name = libs[++i];
                            }
                            if (visited_libs.insert (framework_name).second) {
                                package->framework_names.push_back (framework_name);
                            }
                        }
                        else {
                            package->other_libs.push_back (libs[i]);
                        }
                    }
                    return package;
                }
                return nullptr;
            }

            void PackageResolver::ExtractProvides (const std::string &path) {
                std::ifstream file (path);
                std::string line;
                std::unordered_map<std::string, std::string> variables;
                variables["pcfiledir"] = util::Path (path).GetDirectory ();
                std::string provides;
                std::string version = "0.0.0";
                while (std::getline (file, line)) {
                    line = Trim (line);
                    if (line.empty () || line[0] == '#') {
                        continue;
                    }
                    size_t colon = line.find (':');
                    size_t equal = line.find ('=');
                    if (equal != std::string::npos && (colon == std::string::npos || equal < colon)) {
                        std::string variable = Trim (line.substr (0, equal));
                        std::string value = Trim (line.substr (equal + 1));
                        variables[variable] = ResolveVariables (value, variables);
                    }
                    else if (colon != std::string::npos) {
                        std::string key = Trim (line.substr (0, colon));
                        std::string value = Trim (line.substr (colon + 1));
                        if (key == "Provides") {
                            provides = value;
                        }
                        else if (key == "Version") {
                            version = value;
                        }
                    }
                }
                if (!provides.empty ()) {
                    std::string name = util::Path (path).GetFileName ();
                    provides = ResolveVariables (provides, variables);
                    version = ResolveVariables (version, variables);
                    std::vector<Package::Constraint> capabilities;
                    ParseConstraints (provides, capabilities);
                    for (auto &capability : capabilities) {
                        if (capability.version.IsEmpty ()) {
                            capability.op = util::Version::EQ;
                            capability.version = version;
                        }
                        providers[capability.name].push_back ({name, capability});
                    }
                }
            }

            std::string PackageResolver::ResolveName (
                    const Package::Constraint &requirement,
                    const std::string &context) {
                // Direct match takes precedence.
                if (!FindFile (requirement.name).empty ()) {
                    Package::SharedPtr package = ParseFile (requirement.name);
                    if (package->MatchesRequirement (requirement)) {
                        return requirement.name;
                    }
                }
                std::vector<Provider> providers_;
                // Package not found, check providers
                auto it = providers.find (requirement.name);
                if (it != providers.end ()) {
                    for (const auto &provider : it->second) {
                        Package::SharedPtr package = ParseFile (provider.name);
                        if (package->MatchesRequirement (requirement)) {
                            providers_.push_back (provider);
                        }
                    }
                }
                if (!providers_.empty ()) {
                    // Sort providers: highest capability version,
                    // with physical version fallback.
                    std::sort (providers_.begin (), providers_.end (),
                        [&] (const Provider &a, const Provider &b) {
                            Package::SharedPtr package_a = ParseFile (a.name);
                            Package::SharedPtr package_b = ParseFile (b.name);
                            util::Version version_a (a.capability.version.IsEmpty () ? package_a->version : a.capability.version);
                            util::Version version_b (b.capability.version.IsEmpty () ? package_b->version : b.capability.version);
                            int result = version_a.Compare (version_b);
                            if (result != 0) {
                                return result > 0; // Rank highest capability version first
                            }
                            return package_a->CompareVersion (package_b->version) > 0; // Fallback to physical version sorting
                        }
                    );
                    return providers_[0].name;
                }
                else {
                    THEKOGANS_UTIL_LOG_SUBSYSTEM_ERROR (
                        THEKOGANS_MAKE_CORE,
                        "Dependency resolution failure! Package '%s' requires '%s %s %s', but no valid matches were discovered.",
                        context.c_str (),
                        requirement.name.c_str (),
                        util::Version::OPTostring (requirement.op).c_str (),
                        requirement.version.ToString ().c_str ());
                    return std::string ();
                }
            }

            bool PackageResolver::dfs (
                    const std::string &name,
                    bool static_,
                    std::vector<Package::SharedPtr> &packages,
                    std::unordered_set<std::string> &visited) {
                if (!visited.insert (name).second) {
                    return true;
                }
                Package::SharedPtr package = ParseFile (name);
                // Audit the package being loaded against conflicts declared by packages already visited.
                for (auto package_ : packages) {
                    for (const auto &conflict : package_->conflicts) {
                        if (package->MatchesRequirement (conflict)) {
                            THEKOGANS_UTIL_LOG_SUBSYSTEM_ERROR (
                                THEKOGANS_MAKE_CORE,
                                "Package Conflict! '%s' conflicts with '%s %s %s'. Found version: %s.",
                                package_->fileName.c_str (),
                                package->fileName.c_str (),
                                util::Version::OPTostring (conflict.op).c_str (),
                                conflict.version.ToString ().c_str (),
                                package->version.ToString ().c_str ());
                            return false;
                        }
                    }
                }
                // Audit previously loaded packages against conflicts declared inside this newly discovered package.
                for (const auto &conflict : package->conflicts) {
                    Package::SharedPtr conflictTarget = ParseFile (conflict.name);
                    if (conflictTarget->MatchesRequirement (conflict)) {
                        THEKOGANS_UTIL_LOG_SUBSYSTEM_ERROR (
                            THEKOGANS_MAKE_CORE,
                            "Package Conflict! '%s' conflicts with '%s %s %s'. Loaded package version is: %s.",
                            package->fileName.c_str (),
                            conflict.name.c_str (),
                            util::Version::OPTostring (conflict.op).c_str (),
                            conflict.version.ToString ().c_str (),
                            conflictTarget->version.ToString ().c_str ());
                        return false;
                    }
                }
                for (const auto &dependency : package->requires_public) {
                    if (!dfs (ResolveName (dependency, name), static_, packages, visited)) {
                        return false;
                    }
                }
                if (static_) {
                    for (const auto &dependency : package->requires_private) {
                        if (!dfs (ResolveName (dependency, name), static_, packages, visited)) {
                            return false;
                        }
                    }
                }
                packages.push_back (package);
                return true;
            }

            Package::SharedPtr PackageResolver::ParseFile (const std::string &name) {
                if (cache.count (name) != 0) {
                    return cache[name];
                }
                std::string path = FindFile (name);
                if (path.empty ()) {
                    return nullptr;
                }
                Package::SharedPtr package (new Package (path));
                std::unordered_map<std::string, std::string> variables;
                variables["pcfiledir"] = util::Path (path).GetDirectory ();
                std::vector<std::pair<std::string, std::string>> properties;
                std::ifstream file (path);
                std::string line;
                while (std::getline (file, line)) {
                    line = Trim (line);
                    if (line.empty () || line[0] == '#') {
                        continue;
                    }
                    size_t colon = line.find (':');
                    size_t equal = line.find ('=');
                    if (equal != std::string::npos && (colon == std::string::npos || equal < colon)) {
                        std::string variable = Trim (line.substr (0, equal));
                        std::string value = Trim (line.substr (equal + 1));
                        variables[variable] = ResolveVariables (value, variables);
                    }
                    else if (colon != std::string::npos) {
                        std::string key = Trim (line.substr(0, colon));
                        std::string value = Trim (line.substr(colon + 1));
                        properties.push_back ({key, value});
                    }
                }
                for (const auto &[key, value] : properties) {
                    std::string resolved = ResolveVariables (value, variables);
                    if (key == "Name") {
                        package->name = resolved;
                    }
                    else if (key == "Version") {
                        package->version = resolved;
                    }
                    else if (key == "Description") {
                        package->description = resolved;
                    }
                    else if (key == "URL") {
                        package->url = resolved;
                    }
                    else if (key == "Requires") {
                        ParseConstraints (resolved, package->requires_public);
                    }
                    else if (key == "Requires.private") {
                        ParseConstraints (resolved, package->requires_private);
                    }
                    else if (key == "Conflicts") {
                        ParseConstraints (resolved, package->conflicts);
                    }
                    else if (key == "Provides") {
                        ParseConstraints (resolved, package->provides);
                    }
                    else if (key == "Libs") {
                        Tokenize (resolved, package->libs_public);
                    }
                    else if (key == "Libs.private") {
                        Tokenize (resolved, package->libs_private);
                    }
                    else if (key == "Cflags") {
                        Tokenize (resolved, package->cflags_public);
                    }
                    else if (key == "Cflags.private") {
                        Tokenize (resolved, package->cflags_private);
                    }
                }
                cache[name] = package;
                return package;
            }

            std::string PackageResolver::FindFile (const std::string &name) {
                for (const auto &path : searchPaths) {
                    std::string candidate = util::MakePath (path, name + ".pc");
                    if (util::Path (candidate).Exists ()) {
                        return candidate;
                    }
                }
                return std::string ();
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
