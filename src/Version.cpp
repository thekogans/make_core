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

#include "thekogans/make/core/Version.h"

namespace thekogans {
    namespace make {
        namespace core {

            _LIB_THEKOGANS_MAKE_CORE_DECL const util::Version & _LIB_THEKOGANS_MAKE_CORE_API GetVersion () {
                util::ui32 major = 0;
                util::ui32 minor = 0;
                util::ui32 patch = 0;
            #if !THEKOGANS_UTIL_IS_MACRO_EMPTY (THEKOGANS_MAKE_CORE_MAJOR_VERSION)
                major = THEKOGANS_MAKE_CORE_MAJOR_VERSION;
            #endif
            #if !THEKOGANS_UTIL_IS_MACRO_EMPTY (THEKOGANS_MAKE_CORE_MINOR_VERSION)
                minor = THEKOGANS_MAKE_CORE_MINOR_VERSION;
            #endif
            #if !THEKOGANS_UTIL_IS_MACRO_EMPTY (THEKOGANS_MAKE_CORE_PATCH_VERSION)
                patch = THEKOGANS_MAKE_CORE_PATCH_VERSION;
            #endif
                static const util::Version version (major, minor, patch);
                return version;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
