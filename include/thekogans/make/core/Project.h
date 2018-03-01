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

#if !defined (__thekogans_make_core_Project_h)
#define __thekogans_make_core_Project_h

#include <string>
#include <list>
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Project {
                static bool Find (
                    const std::string &organization,
                    const std::string &project,
                    std::string &branch,
                    std::string &version,
                    const std::string &example);
                static bool IsInstalled (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example);
                static void GetVersions (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    std::list<std::string> &versions);
                static std::string GetLatestVersion (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch);
                static std::string GetRoot (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example);
                static std::string GetConfig (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example);
                static std::string GetLibrary (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config,
                    const std::string &type);
                static std::string GetProgram (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config,
                    const std::string &type);
                static std::string GetPlugin (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config);
                static void Cleanup (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch);
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Project_h)
