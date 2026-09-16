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

#if !defined (__thekogans_make_core_PackageResolver_h)
#define __thekogans_make_core_PackageResolver_h

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "thekogans/util/Singleton.h"
#include "thekogans/make/core/Config.h"
#include "thekogans/make/core/Package.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct PackageResolver : util::Singleton<PackageResolver> {
            private:
                std::vector<std::string> searchPaths;
                struct Provider {
                    std::string name;
                    Package::Constraint capability;
                };
                using Providers = std::unordered_map<std::string, std::vector<Provider>>;
                Providers providers;
                std::unordered_map<std::string, Package::SharedPtr> cache;

            public:
                PackageResolver ();

                Package::SharedPtr Resolve (
                    const Package::Constraint &requirement,
                    bool static_);

            private:
                void ExtractProvides (const std::string &path);
                std::string ResolveName (
                    const Package::Constraint &requirement,
                    const std::string &context);
                bool dfs (
                    const std::string &name,
                    bool static_,
                    std::vector<Package::SharedPtr> &packages,
                    std::unordered_set<std::string> &visited);
                Package::SharedPtr ParseFile (const std::string &name);
                std::string FindFile (const std::string &name);
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_PackageResolver_h)
