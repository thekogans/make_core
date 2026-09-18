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

#include <sstream>
#include "thekogans/util/Environment.h"
#include "thekogans/util/XMLUtils.h"
#include "thekogans/make/core/LibraryDependency.h"

namespace thekogans {
    namespace make {
        namespace core {

            bool LibraryDependency::EquivalentTo (const Dependency &dependency) const {
                const LibraryDependency *libraryDependency =
                    dynamic_cast<const LibraryDependency *> (&dependency);
                return libraryDependency != nullptr && libraryDependency->name == name;
            }

            void LibraryDependency::GetLinkLibraries (std::set<std::string> &link_libraries) const {
                    std::string prefix;
                #if !defined (TOOLCHAIN_OS_Windows)
                    prefix += "-l";
                #endif // !defined (TOOLCHAIN_OS_Windows)
                    link_libraries.insert (prefix + name);
            }

            std::string LibraryDependency::ToString (util::ui32 indentationLevel) const {
                util::Attributes attributes;
                attributes.push_back (util::Attribute (thekogans_make::ATTR_NAME, name));
                return util::OpenTag (indentationLevel, thekogans_make::TAG_LIBRARY, attributes, true, true);
            }

            void LibraryDependency::ListDependencies (util::ui32 indentationLevel) const {
                std::cout <<
                    std::string (indentationLevel * 2, ' ') <<
                    "library: " << name << std::endl;
                std::cout.flush ();
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
