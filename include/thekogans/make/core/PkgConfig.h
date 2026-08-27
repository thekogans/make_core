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

#if !defined (__thekogans_make_core_PkgConfig_h)
#define __thekogans_make_core_PkgConfig_h

#include <string>
#include <set>
#include <unordered_map>
#include "thekogans/util/RefCounted.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            // Standard properties
            // name;
            // version;
            // description;
            // url;
            // libs;
            // libs_private;
            // cflags;
            // cflags_private;
            // requires_;
            // requires_private;
            // conflicts;
            struct _LIB_THEKOGANS_MAKE_CORE_DECL PkgConfig : public util::RefCounted {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (PkgConfig)

            private:
                std::string path;
                std::string name;
                std::string version;
                std::string config;
                std::string type;
                using Variables = std::unordered_map<std::string, std::string>;
                Variables variables;
                using Properties = std::unordered_map<std::string, std::string>;
                Properties properties;

                PkgConfig (
                    const std::string &path_,
                    const std::string &name_,
                    const std::string &version_,
                    const std::string &config_,
                    const std::string &type_);

            public:
                static PkgConfig::SharedPtr GetConfig (
                    const std::string &path_,
                    const std::string &name_,
                    const std::string &version_,
                    const std::string &config_,
                    const std::string &type_);

                static bool IsInstalled (
                    const std::string &package,
                    const std::string &version);

                void GetLibs (std::set<std::string> &libs) const;
                void GetCFlags (std::set<std::string> &c_flags) const;

            private:
                std::string ResolveVariables (std::string value);
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_PkgConfig_h)
