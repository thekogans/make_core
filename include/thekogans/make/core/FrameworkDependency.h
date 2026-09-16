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

#if !defined (__thekogans_make_core_FrameworkDependency_h)
#define __thekogans_make_core_FrameworkDependency_h

#include <string>
#include "thekogans/make/core/thekogans_make.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct FrameworkDependency : public thekogans_make::Dependency {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (FrameworkDependency)

                std::string name;

                FrameworkDependency (
                    const thekogans_make &dependent,
                    bool isPrivate,
                    const std::string &name_) :
                    Dependency (dependent, isPrivate),
                    name (name_) {}

                virtual bool EquivalentTo (const Dependency &dependency) const override;

                virtual void GetLinkLibraries (std::vector<std::string> &link_libraries) const override {
                    link_libraries.push_back ("-framework " + name);
                }

                virtual std::string ToString (util::ui32 indentationLevel = 0) const override;
                virtual void ListDependencies (util::ui32 indentationLevel = 0) const override;
            };

        } // namespace core

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_FrameworkDependency_h)
