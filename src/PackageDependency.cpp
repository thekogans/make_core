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

#include "thekogans/make/core/PackageResolver.h"
#include "thekogans/make/core/PackageDependency.h"

namespace thekogans {
    namespace make {
        namespace core {

            PackageDependency::PackageDependency (
                    const thekogans_make &dependent,
                    bool isPrivate,
                    const Package::Constraint &requirement_) :
                    Dependency (dependent, isPrivate),
                    requirement (requirement_),
                    package (
                        PackageResolver::Instance ()->Resolve (
                            requirement, dependent.type == TYPE_STATIC)) {
            }

            bool PackageDependency::EquivalentTo (const Dependency &dependency) const {
                const PackageDependency *packageDependency =
                    dynamic_cast<const PackageDependency *> (&dependency);
                return packageDependency != nullptr &&
                    packageDependency->requirement == requirement;
            }

            void PackageDependency::GetIncludeDirectories (std::set<std::string> &include_directories) const {
                for (const auto &include_directory : package->include_directories) {
                    include_directories.insert (include_directory);
                }
            }

            void PackageDependency::GetLibraryDirectories (std::set<std::string> &library_directories) const {
                for (const auto &library_directory : package->library_directories) {
                    library_directories.insert (library_directory);
                }
            }

            void PackageDependency::GetFrameworkDirectories (std::set<std::string> &framework_directories) const {
                for (const auto &framework_directory : package->framework_directories) {
                    framework_directories.insert (framework_directory);
                }
            }

            void PackageDependency::GetLinkerFlags (std::set<std::string> &linker_flags) const {
                for (const auto &linker_flag : package->other_libs) {
                    linker_flags.insert (linker_flag);
                }
            }

            void PackageDependency::GetCFlags (std::set<std::string> &c_flags) const {
                for (const auto &c_flag : package->other_cflags) {
                    c_flags.insert (c_flag);
                }
            }

            void PackageDependency::GetCPreprocessorDefinitions (std::set<std::string> &c_preprocessor_definitions) const {
                for (const auto &preprocessor_definition : package->preprocessor_definitions) {
                    c_preprocessor_definitions.insert (preprocessor_definition);
                }
            }

            void PackageDependency::GetLinkLibraries (std::vector<std::string> &link_libraries) const {
                for (const auto &library_name : package->library_names) {
                    std::string library = "-l" + library_name;
                    if (link_libraries.empty () || link_libraries.back () != library) {
                        link_libraries.push_back (library);
                    }
                }
                for (const auto &framework_name : package->framework_names) {
                    std::string framework = "-framework " + framework_name;
                    if (link_libraries.empty () || link_libraries.back () != framework) {
                        link_libraries.push_back (framework);
                    }
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
