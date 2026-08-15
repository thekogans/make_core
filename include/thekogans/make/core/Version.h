// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_make_core.
//
// libthekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_core_Version_h)
#define __thekogans_make_core_Version_h

#include "thekogans/util/Version.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            _LIB_THEKOGANS_MAKE_CORE_DECL const util::Version & _LIB_THEKOGANS_MAKE_CORE_API GetVersion ();

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Version_h)
