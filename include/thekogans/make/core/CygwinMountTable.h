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

#if !defined (__thekogans_make_core_CygwinMountTable_h)
#define __thekogans_make_core_CygwinMountTable_h

#if defined (TOOLCHAIN_OS_Windows)

#include <string>
#include <vector>
#include "thekogans/util/Singleton.h"
#include "thekogans/util/SpinLock.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct _LIB_THEKOGANS_MAKE_CORE_DECL CygwinMountTable :
                    public util::Singleton<CygwinMountTable, util::SpinLock> {
                struct Entry {
                    std::string host;
                    std::string cygwin;
                    std::string type;
                    std::string flags;
                };

            private:
                std::vector<Entry> hostEntries;
                std::vector<Entry> cygwinEntries;

            public:
                CygwinMountTable ();

                void DumpEntries () const;

                std::string ToHostPath (const std::string &cygwinPath) const;
                std::string ToCygwinPath (const std::string &hostPath) const;
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (TOOLCHAIN_OS_Windows)

#endif // !defined (__thekogans_make_core_CygwinMountTable_h)
