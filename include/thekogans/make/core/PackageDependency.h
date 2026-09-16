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

#if !defined (__thekogans_make_core_PackageDependency_h)
#define __thekogans_make_core_PackageDependency_h

#include <string>
#include "thekogans/make/core/Package.h"
#include "thekogans/make/core/thekogans_make.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct PackageDependency : public thekogans_make::Dependency {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (PackageDependency)

                Package::Constraint requirement;
                Package::SharedPtr package;

                PackageDependency (
                    const thekogans_make &dependent,
                    bool isPrivate,
                    const Package::Constraint &requirement_);

                virtual bool EquivalentTo (const Dependency &dependency) const override;

                virtual void GetIncludeDirectories (std::set<std::string> &include_directories) const override;
                virtual void GetLibraryDirectories (std::set<std::string> &library_directories) const override;
                virtual void GetFrameworkDirectories (std::set<std::string> &framework_directories) const override;
                virtual void GetLinkerFlags (std::set<std::string> &linker_flags) const override;
                virtual void GetCFlags (std::set<std::string> &c_flags) const override;
                virtual void GetCPreprocessorDefinitions (std::set<std::string> &c_preprocessor_definitions) const override;
                virtual void GetLinkLibraries (std::vector<std::string> &link_libraries) const override;
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_PackageDependency_h)
