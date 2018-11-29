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

#if !defined (__thekogans_make_core_Installer_h)
#define __thekogans_make_core_Installer_h

#include <string>
#include <set>
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct thekogans_make;

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Installer {
                std::string config;
                std::string type;
                std::string runtime_type;
                bool hide_commands;
                bool parallel_build;
                std::set<std::string> installedProjects;

                Installer (
                    const std::string &config_,
                    const std::string &type_,
                    const std::string &runtime_type_,
                    bool hide_commands_,
                    bool parallel_build_) :
                    config (config_),
                    type (type_),
                    runtime_type (runtime_type_),
                    hide_commands (hide_commands_),
                    parallel_build (parallel_build_) {}

                void InstallLibrary (const std::string &project_root);
                void InstallProgram (const std::string &project_root);
                void InstallPlugin (const std::string &project_root);
                void InstallPluginHosts (const std::string &project_root);

            private:
                void InstallLibrary (
                    const thekogans_make &DebugSharedShared,
                    const thekogans_make &DebugSharedStatic,
                    const thekogans_make &DebugStaticShared,
                    const thekogans_make &DebugStaticStatic,
                    const thekogans_make &ReleaseSharedShared,
                    const thekogans_make &ReleaseSharedStatic,
                    const thekogans_make &ReleaseStaticShared,
                    const thekogans_make &ReleaseStaticStatic);
                void InstallDependency (const thekogans_make &dependency);
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Installer_h)
