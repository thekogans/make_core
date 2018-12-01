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

#include <string>
#include <list>
#include <set>
#include <iostream>
#include <fstream>
#include "thekogans/util/Path.h"
#include "thekogans/util/Plugins.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/XMLUtils.h"
#include "thekogans/util/SHA2.h"
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Manifest.h"
#include "thekogans/make/core/Project.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/core/Installer.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                void GetCommonFeatures (
                        const thekogans_make &DebugSharedShared,
                        const thekogans_make &DebugSharedStatic,
                        const thekogans_make &DebugStaticShared,
                        const thekogans_make &DebugStaticStatic,
                        const thekogans_make &ReleaseSharedShared,
                        const thekogans_make &ReleaseSharedStatic,
                        const thekogans_make &ReleaseStaticShared,
                        const thekogans_make &ReleaseStaticStatic,
                        std::set<std::string> &commonFeatures) {
                    for (std::set<std::string>::const_iterator
                            it = DebugSharedShared.features.begin (),
                            end = DebugSharedShared.features.end (); it != end; ++it) {
                        if (DebugSharedStatic.features.find (*it) != DebugSharedStatic.features.end () &&
                                DebugStaticShared.features.find (*it) != DebugStaticShared.features.end () &&
                                DebugStaticStatic.features.find (*it) != DebugStaticStatic.features.end () &&
                                ReleaseSharedShared.features.find (*it) != ReleaseSharedShared.features.end () &&
                                ReleaseSharedStatic.features.find (*it) != ReleaseSharedStatic.features.end () &&
                                ReleaseStaticShared.features.find (*it) != ReleaseStaticShared.features.end () &&
                                ReleaseStaticStatic.features.find (*it) != ReleaseStaticStatic.features.end ()) {
                            commonFeatures.insert (*it);
                        }
                    }
                }

                void GetUniqueFeatures (
                        const thekogans_make &DebugSharedShared,
                        const thekogans_make &DebugSharedStatic,
                        const thekogans_make &DebugStaticShared,
                        const thekogans_make &DebugStaticStatic,
                        const thekogans_make &ReleaseSharedShared,
                        const thekogans_make &ReleaseSharedStatic,
                        const thekogans_make &ReleaseStaticShared,
                        const thekogans_make &ReleaseStaticStatic,
                        const std::set<std::string> &commonFeatures,
                        std::set<std::string> &DebugSharedSharedFeatures,
                        std::set<std::string> &DebugSharedStaticFeatures,
                        std::set<std::string> &DebugStaticSharedFeatures,
                        std::set<std::string> &DebugStaticStaticFeatures,
                        std::set<std::string> &ReleaseSharedSharedFeatures,
                        std::set<std::string> &ReleaseSharedStaticFeatures,
                        std::set<std::string> &ReleaseStaticSharedFeatures,
                        std::set<std::string> &ReleaseStaticStaticFeatures) {
                    for (std::set<std::string>::const_iterator
                            it = DebugSharedShared.features.begin (),
                            end = DebugSharedShared.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            DebugSharedSharedFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = DebugSharedStatic.features.begin (),
                            end = DebugSharedStatic.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            DebugSharedStaticFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = DebugStaticShared.features.begin (),
                            end = DebugStaticShared.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            DebugStaticSharedFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = DebugStaticStatic.features.begin (),
                            end = DebugStaticStatic.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            DebugStaticStaticFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = ReleaseSharedShared.features.begin (),
                            end = ReleaseSharedShared.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            ReleaseSharedSharedFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = ReleaseSharedStatic.features.begin (),
                            end = ReleaseSharedStatic.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            ReleaseSharedStaticFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = ReleaseStaticShared.features.begin (),
                            end = ReleaseStaticShared.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            ReleaseStaticSharedFeatures.insert (*it);
                        }
                    }
                    for (std::set<std::string>::const_iterator
                            it = ReleaseStaticStatic.features.begin (),
                            end = ReleaseStaticStatic.features.end (); it != end; ++it) {
                        if (commonFeatures.find (*it) == commonFeatures.end ()) {
                            ReleaseStaticStaticFeatures.insert (*it);
                        }
                    }
                }

                bool ContainsDependency (
                        const thekogans_make &config,
                        const thekogans_make::Dependency &dependency) {
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = config.dependencies.begin (),
                            end = config.dependencies.end (); it != end; ++it) {
                        if ((*it)->EquivalentTo (dependency)) {
                            return true;
                        }
                    }
                    return false;
                }

                void GetCommonDependencies (
                        const thekogans_make &DebugSharedShared,
                        const thekogans_make &DebugSharedStatic,
                        const thekogans_make &DebugStaticShared,
                        const thekogans_make &DebugStaticStatic,
                        const thekogans_make &ReleaseSharedShared,
                        const thekogans_make &ReleaseSharedStatic,
                        const thekogans_make &ReleaseStaticShared,
                        const thekogans_make &ReleaseStaticStatic,
                        std::list<thekogans_make::Dependency *> &commonDependencies) {
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = DebugSharedShared.dependencies.begin (),
                            end = DebugSharedShared.dependencies.end (); it != end; ++it) {
                        if (ContainsDependency (DebugSharedStatic, **it) &&
                                ContainsDependency (DebugStaticShared, **it) &&
                                ContainsDependency (DebugStaticStatic, **it) &&
                                ContainsDependency (ReleaseSharedShared, **it) &&
                                ContainsDependency (ReleaseSharedStatic, **it) &&
                                ContainsDependency (ReleaseStaticShared, **it) &&
                                ContainsDependency (ReleaseStaticStatic, **it)) {
                            commonDependencies.push_back ((*it).get ());
                        }
                    }
                }

                bool DoesNotContainDependency (
                        const std::list<thekogans_make::Dependency *> &commonDependencies,
                        const thekogans_make::Dependency &dependency) {
                    for (std::list<thekogans_make::Dependency *>::const_iterator
                            it = commonDependencies.begin (),
                            end = commonDependencies.end (); it != end; ++it) {
                        if ((*it)->EquivalentTo (dependency)) {
                            return false;
                        }
                    }
                    return true;
                }

                void GetUniqueDependencies (
                        const thekogans_make &DebugSharedShared,
                        const thekogans_make &DebugSharedStatic,
                        const thekogans_make &DebugStaticShared,
                        const thekogans_make &DebugStaticStatic,
                        const thekogans_make &ReleaseSharedShared,
                        const thekogans_make &ReleaseSharedStatic,
                        const thekogans_make &ReleaseStaticShared,
                        const thekogans_make &ReleaseStaticStatic,
                        const std::list<thekogans_make::Dependency *> &commonDependencies,
                        std::list<thekogans_make::Dependency *> &DebugSharedSharedDependencies,
                        std::list<thekogans_make::Dependency *> &DebugSharedStaticDependencies,
                        std::list<thekogans_make::Dependency *> &DebugStaticSharedDependencies,
                        std::list<thekogans_make::Dependency *> &DebugStaticStaticDependencies,
                        std::list<thekogans_make::Dependency *> &ReleaseSharedSharedDependencies,
                        std::list<thekogans_make::Dependency *> &ReleaseSharedStaticDependencies,
                        std::list<thekogans_make::Dependency *> &ReleaseStaticSharedDependencies,
                        std::list<thekogans_make::Dependency *> &ReleaseStaticStaticDependencies) {
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = DebugSharedShared.dependencies.begin (),
                            end = DebugSharedShared.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            DebugSharedSharedDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = DebugSharedStatic.dependencies.begin (),
                            end = DebugSharedStatic.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            DebugSharedStaticDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = DebugStaticShared.dependencies.begin (),
                            end = DebugStaticShared.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            DebugStaticSharedDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = DebugStaticStatic.dependencies.begin (),
                            end = DebugStaticStatic.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            DebugStaticStaticDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = ReleaseSharedShared.dependencies.begin (),
                            end = ReleaseSharedShared.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            ReleaseSharedSharedDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = ReleaseSharedStatic.dependencies.begin (),
                            end = ReleaseSharedStatic.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            ReleaseSharedStaticDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = ReleaseStaticShared.dependencies.begin (),
                            end = ReleaseStaticShared.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            ReleaseStaticSharedDependencies.push_back ((*it).get ());
                        }
                    }
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = ReleaseStaticStatic.dependencies.begin (),
                            end = ReleaseStaticStatic.dependencies.end (); it != end; ++it) {
                        if (DoesNotContainDependency (commonDependencies, **it)) {
                            ReleaseStaticStaticDependencies.push_back ((*it).get ());
                        }
                    }
                }

                typedef std::pair<std::string, std::string> InstallPaths;

                void GetInstallPaths (
                        const thekogans_make &config,
                        const std::list<thekogans_make::FileList::Ptr> &fileList,
                        std::set<InstallPaths> &installPaths) {
                    for (std::list<thekogans_make::FileList::Ptr>::const_iterator
                            it = fileList.begin (),
                            end = fileList.end (); it != end; ++it) {
                        if ((*it)->install) {
                            std::string prefix = MakePath (config.project_root, (*it)->prefix);
                            for (std::list<thekogans_make::FileList::File::Ptr>::const_iterator
                                    jt = (*it)->files.begin (),
                                    end = (*it)->files.end (); jt != end; ++jt) {
                                if ((*jt)->customBuild.get () == 0) {
                                    installPaths.insert (
                                        InstallPaths (
                                            MakePath (prefix, (*jt)->name),
                                            MakePath ((*it)->destinationPrefix, (*jt)->name)));
                                }
                                else {
                                    std::string prefix =
                                        MakePath (
                                            MakePath (
                                                config.project_root,
                                                GetBuildDirectory (
                                                    config.generator,
                                                    config.config,
                                                    config.type,
                                                    config.runtime_type)),
                                            (*it)->prefix);
                                    for (std::vector<std::string>::const_iterator
                                            kt = (*jt)->customBuild->outputs.begin (),
                                            end = (*jt)->customBuild->outputs.end (); kt != end; ++kt) {
                                        installPaths.insert (
                                            InstallPaths (
                                                MakePath (prefix, *kt),
                                                MakePath ((*it)->destinationPrefix, *kt)));
                                    }
                                }
                            }
                        }
                    }
                }

                void GetInstallPaths (
                        const thekogans_make &config,
                        std::set<InstallPaths> &installPaths) {
                    GetInstallPaths (config, config.masm_headers, installPaths);
                    GetInstallPaths (config, config.masm_sources, installPaths);
                    GetInstallPaths (config, config.masm_tests, installPaths);
                    GetInstallPaths (config, config.nasm_headers, installPaths);
                    GetInstallPaths (config, config.nasm_sources, installPaths);
                    GetInstallPaths (config, config.nasm_tests, installPaths);
                    GetInstallPaths (config, config.c_headers, installPaths);
                    GetInstallPaths (config, config.c_sources, installPaths);
                    GetInstallPaths (config, config.c_tests, installPaths);
                    GetInstallPaths (config, config.cpp_headers, installPaths);
                    GetInstallPaths (config, config.cpp_sources, installPaths);
                    GetInstallPaths (config, config.cpp_tests, installPaths);
                    GetInstallPaths (config, config.objective_c_headers, installPaths);
                    GetInstallPaths (config, config.objective_c_sources, installPaths);
                    GetInstallPaths (config, config.objective_c_tests, installPaths);
                    GetInstallPaths (config, config.objective_cpp_headers, installPaths);
                    GetInstallPaths (config, config.objective_cpp_sources, installPaths);
                    GetInstallPaths (config, config.objective_cpp_tests, installPaths);
                    GetInstallPaths (config, config.resources, installPaths);
                    GetInstallPaths (config, config.rc_sources, installPaths);
                }
            }

            void Installer::InstallLibrary (const std::string &project_root) {
                if (installedProjects.find (project_root) == installedProjects.end ()) {
                    installedProjects.insert (project_root);
                    std::string install_config = config;
                    if (install_config.empty ()) {
                        install_config =
                            thekogans_make::GetBuildConfig (project_root, THEKOGANS_MAKE_XML);
                    }
                    std::string install_type = type;
                    if (install_type.empty ()) {
                        install_type =
                            thekogans_make::GetBuildType (project_root, THEKOGANS_MAKE_XML);
                    }
                    std::string install_runtime_type = runtime_type;
                    if (install_runtime_type.empty ()) {
                        install_runtime_type =
                            thekogans_make::GetBuildRuntimeType (project_root, THEKOGANS_MAKE_XML);
                    }
                    if (!install_config.empty () && !install_type.empty () && !install_runtime_type.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            install_type,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                install_runtime_type);
                        InstallLibrary (
                            config,
                            config,
                            config,
                            config,
                            config,
                            config,
                            config,
                            config);
                    }
                    else if (!install_config.empty () && !install_type.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            install_type,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            install_type,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type,
                                TYPE_STATIC);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else if (!install_config.empty () && !install_runtime_type.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_SHARED,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_STATIC,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                install_runtime_type);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else if (!install_type.empty () && !install_runtime_type.empty ()) {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            install_type,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            install_type,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                install_runtime_type);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else if (!install_config.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_SHARED,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_SHARED,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_STATIC,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_STATIC,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                TYPE_SHARED);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                TYPE_STATIC);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                TYPE_SHARED);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC,
                                TYPE_STATIC);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else if (!install_type.empty ()) {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            install_type,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            install_type,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            install_type,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            install_type,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type,
                                TYPE_STATIC);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else if (!install_runtime_type.empty ()) {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_SHARED,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_STATIC,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_SHARED,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_STATIC,
                            install_runtime_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_SHARED,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_STATIC,
                                install_runtime_type);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_STATIC,
                                install_runtime_type);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                    else {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_SHARED,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_SHARED,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_STATIC,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_STATIC,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_SHARED,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_SHARED,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_STATIC,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_STATIC,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_SHARED,
                                TYPE_SHARED);
                        const thekogans_make &DebugSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_SHARED,
                                TYPE_STATIC);
                        const thekogans_make &DebugStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_STATIC,
                                TYPE_SHARED);
                        const thekogans_make &DebugStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_STATIC,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseSharedShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_SHARED,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseSharedStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_SHARED,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseStaticShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_STATIC,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStaticStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_STATIC,
                                TYPE_STATIC);
                        InstallLibrary (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic);
                    }
                }
            }

            void Installer::InstallProgram (const std::string &project_root) {
                if (installedProjects.find (project_root) == installedProjects.end ()) {
                    installedProjects.insert (project_root);
                    std::string install_config = config;
                    if (install_config.empty ()) {
                        install_config =
                            thekogans_make::GetBuildConfig (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_config.empty ()) {
                            install_config = CONFIG_RELEASE;
                        }
                    }
                    std::string install_type = type;
                    if (install_type.empty ()) {
                        install_type =
                            thekogans_make::GetBuildType (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_type.empty ()) {
                            install_type = TYPE_STATIC;
                        }
                    }
                    std::string install_runtime_type = runtime_type;
                    if (install_runtime_type.empty ()) {
                        install_runtime_type =
                            thekogans_make::GetBuildRuntimeType (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_runtime_type.empty ()) {
                            install_runtime_type = TYPE_SHARED;
                        }
                    }
                    BuildProject (
                        project_root,
                        install_config,
                        install_type,
                        install_runtime_type,
                        MODE_INSTALL,
                        hide_commands,
                        parallel_build,
                        TARGET_ALL);
                    const thekogans_make &config =
                        thekogans_make::GetConfig (
                            project_root,
                            THEKOGANS_MAKE_XML,
                            MAKE,
                            install_config,
                            install_type,
                            install_runtime_type);
                    std::list<std::string> dependencies;
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = config.dependencies.begin (),
                            end = config.dependencies.end (); it != end; ++it) {
                        if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            const core::thekogans_make &dependency = thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ());
                            if (dependency.project_type == PROJECT_TYPE_PROGRAM ||
                                    dependency.project_type == PROJECT_TYPE_PLUGIN) {
                                InstallDependency (dependency);
                                dependencies.push_back ((*it)->ToString (2));
                            }
                        }
                    }
                    // Uninstall old version
                    UninstallProgram (
                        config.organization,
                        config.project,
                        config.GetVersion (),
                        false);
                    std::cout << "Installing " << project_root << std::endl;
                    std::cout.flush ();
                    // install = "yes"
                    std::set<InstallPaths> installPaths;
                    GetInstallPaths (config, installPaths);
                    if (config.HasGoal ()) {
                        installPaths.insert (
                            InstallPaths (
                                config.GetProjectGoal (),
                                config.GetToolchainGoal ()));
                    }
                    for (std::set<InstallPaths>::const_iterator
                            it = installPaths.begin (),
                            end = installPaths.end (); it != end; ++it) {
                        CopyFile ((*it).first, (*it).second);
                    }
                    CopyDependencies (
                        project_root,
                        install_config,
                        install_type,
                        install_runtime_type,
                        config.GetToolchainBinDirectory ());
                    std::string config_file =
                        MakePath (
                            MakePath (_TOOLCHAIN_DIR, CONFIG_DIR),
                            GetFileName (
                                config.organization,
                                config.project,
                                std::string (),
                                config.GetVersion (),
                                XML_EXT));
                    std::cout << "Creating " << config_file << "\n";
                    std::cout.flush ();
                    std::string configFilePath = ToSystemPath (config_file);
                    util::Directory::Create (util::Path (configFilePath).GetDirectory ());
                    std::fstream configFile (
                        configFilePath.c_str (),
                        std::fstream::out | std::fstream::trunc);
                    if (configFile.is_open ()) {
                        util::Attributes attributes;
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_ORGANIZATION,
                                config.organization));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_PROJECT,
                                config.project));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_PROJECT_TYPE,
                                config.project_type));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_MAJOR_VERSION,
                                config.major_version));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_MINOR_VERSION,
                                config.minor_version));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_PATCH_VERSION,
                                config.patch_version));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_NAMING_CONVENTION,
                                config.naming_convention));
                        // NOTE: We don't tag the config file with install_config, install_type
                        // and install_runtime_type because, unlike libraries (and plugins), program
                        // dependency is config, type and runtime_type independent.
//                         attributes.push_back (
//                             util::Attribute (
//                                 thekogans_make::ATTR_BUILD_CONFIG,
//                                 install_config));
//                         attributes.push_back (
//                             util::Attribute (
//                                 thekogans_make::ATTR_BUILD_TYPE,
//                                 install_type));
//                         attributes.push_back (
//                             util::Attribute (
//                                 thekogans_make::ATTR_BUILD_RUNTIME_TYPE,
//                                 install_runtime_type));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_GUID,
                                config.guid.ToString ()));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_SCHEMA_VERSION,
                                util::ui32Tostring (THEKOGANS_MAKE_XML_SCHEMA_VERSION)));
                        configFile << util::OpenTag (0, thekogans_make::TAG_THEKOGANS_MAKE,
                            attributes, false, true);
                        // features
                        if (!config.features.empty ()) {
                            configFile << util::OpenTag (1, thekogans_make::TAG_FEATURES,
                                util::Attributes (), false, true);
                            for (std::set<std::string>::const_iterator
                                    it = config.features.begin (),
                                    end = config.features.end (); it != end; ++it) {
                                configFile <<
                                    util::OpenTag (2, thekogans_make::TAG_FEATURE) <<
                                    *it <<
                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                            }
                            configFile << util::CloseTag (1, thekogans_make::TAG_FEATURES);
                        }
                        // dependencies
                        if (!dependencies.empty ()) {
                            configFile << util::OpenTag (1, thekogans_make::TAG_DEPENDENCIES,
                                util::Attributes (), false, true);
                            for (std::list<std::string>::const_iterator
                                    it = dependencies.begin (),
                                    end = dependencies.end (); it != end; ++it) {
                                configFile << *it;
                            }
                            configFile << util::CloseTag (1, thekogans_make::TAG_DEPENDENCIES);
                        }
                        // resources
                        for (std::list<thekogans_make::FileList::Ptr>::const_iterator
                                it = config.resources.begin (),
                                end = config.resources.end (); it != end; ++it) {
                            if ((*it)->install) {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_PREFIX,
                                        MakePath (
                                            RESOURCES_DIR,
                                            GetFileName (
                                                config.organization,
                                                config.project,
                                                std::string (),
                                                config.GetVersion (),
                                                std::string ()))));
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_INSTALL,
                                        VALUE_YES));
                                configFile << util::OpenTag (1, thekogans_make::TAG_RESOURCES,
                                    attributes, false, true);
                                for (std::list<thekogans_make::FileList::File::Ptr>::const_iterator
                                        jt = (*it)->files.begin (),
                                        end = (*it)->files.end (); jt != end; ++jt) {
                                    if ((*jt)->customBuild.get () == 0) {
                                        configFile <<
                                            util::OpenTag (2, thekogans_make::TAG_RESOURCE) <<
                                            (*jt)->name <<
                                            util::CloseTag (0, thekogans_make::TAG_RESOURCE);
                                    }
                                    else {
                                        for (std::vector<std::string>::const_iterator
                                                kt = (*jt)->customBuild->outputs.begin (),
                                                end = (*jt)->customBuild->outputs.end (); kt != end; ++kt) {
                                            configFile <<
                                                util::OpenTag (2, thekogans_make::TAG_RESOURCE) <<
                                                *kt <<
                                                util::CloseTag (0, thekogans_make::TAG_RESOURCE);
                                        }
                                    }
                                }
                                configFile << util::CloseTag (1, thekogans_make::TAG_RESOURCES);
                            }
                        }
                        configFile << util::CloseTag (0, thekogans_make::TAG_THEKOGANS_MAKE);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to open: %s",
                            configFilePath.c_str ());
                    }
                }
            }

            void Installer::InstallPlugin (const std::string &project_root) {
                if (installedProjects.find (project_root) == installedProjects.end ()) {
                    installedProjects.insert (project_root);
                    std::string install_config = config;
                    if (install_config.empty ()) {
                        install_config =
                            thekogans_make::GetBuildConfig (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_config.empty ()) {
                            install_config = CONFIG_RELEASE;
                        }
                    }
                    std::string install_type = type;
                    if (install_type.empty ()) {
                        install_type =
                            thekogans_make::GetBuildType (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_type.empty ()) {
                            install_type = TYPE_SHARED;
                        }
                    }
                    std::string install_runtime_type = runtime_type;
                    if (install_runtime_type.empty ()) {
                        install_runtime_type =
                            thekogans_make::GetBuildRuntimeType (
                                project_root,
                                THEKOGANS_MAKE_XML);
                        if (install_runtime_type.empty ()) {
                            install_runtime_type = TYPE_SHARED;
                        }
                    }
                    BuildProject (
                        project_root,
                        install_config,
                        install_type,
                        install_runtime_type,
                        MODE_INSTALL,
                        hide_commands,
                        parallel_build,
                        TARGET_ALL);
                    const thekogans_make &plugin_config =
                        thekogans_make::GetConfig (
                            project_root,
                            THEKOGANS_MAKE_XML,
                            MAKE,
                            install_config,
                            install_type,
                            install_runtime_type);
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = plugin_config.plugin_hosts.begin (),
                            end = plugin_config.plugin_hosts.end (); it != end; ++it) {
                        const thekogans_make &host_config =
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                install_config,
                                install_type,
                                install_runtime_type);
                        std::string toDirectory = host_config.project_type == PROJECT_TYPE_PROGRAM ?
                            host_config.GetToolchainBinDirectory () :
                            host_config.GetToolchainLibDirectory ();
                        if (util::Path (ToSystemPath (toDirectory)).Exists ()) {
                            std::string fromPlugin = plugin_config.GetProjectGoal ();
                            std::string pluginFileName = util::Path (fromPlugin).GetFullFileName ();
                            std::string toPlugin =
                                ToSystemPath (MakePath (toDirectory, pluginFileName));
                            util::Plugins plugins (
                                ToSystemPath (host_config.GetToolchainGoal () + EXT_SEPARATOR + PLUGINS_EXT));
                            {
                                CopyFile (fromPlugin, toPlugin);
                                Manifest manifest (
                                    ToSystemPath (
                                        MakePath (toDirectory, THEKOGANS_MANIFEST + EXT_SEPARATOR + XML_EXT)));
                                manifest.AddFile (pluginFileName, host_config.GetGoalFileName ());
                                util::Plugins::Plugin::Ptr plugin =
                                    plugins.GetPlugin (pluginFileName);
                                if (plugin.Get () != 0) {
                                    for (util::Plugins::Plugin::Dependencies::const_iterator
                                            jt = plugin->dependencies.begin (),
                                            end = plugin->dependencies.end (); jt != end; ++jt) {
                                        if (manifest.DeleteFile (*jt, pluginFileName)) {
                                            DeleteFile (MakePath (toDirectory, *jt));
                                        }
                                    }
                                }
                                manifest.Save ();
                            }
                            {
                                CopyDependencies (
                                    plugin_config.project_root,
                                    plugin_config.config,
                                    plugin_config.type,
                                    plugin_config.runtime_type,
                                    toDirectory);
                                util::Plugins::Plugin::Dependencies dependencies;
                                {
                                    std::set<std::string> sharedLibraries;
                                    plugin_config.GetSharedLibraries (sharedLibraries);
                                    for (std::set<std::string>::const_iterator
                                            jt = sharedLibraries.begin (),
                                            end = sharedLibraries.end (); jt != end; ++jt) {
                                        dependencies.insert (util::Path (*jt).GetFullFileName ());
                                    }
                                }
                                plugins.AddPlugin (
                                    pluginFileName,
                                    plugin_config.GetVersion (),
                                    GetFileHash (toPlugin),
                                    dependencies);
                                plugins.Save ();
                            }
                        }
                    }
                }
            }

            void Installer::InstallPluginHosts (const std::string &project_root) {
                const thekogans_make &plugin_config =
                    thekogans_make::GetConfig (
                        project_root,
                        THEKOGANS_MAKE_XML,
                        MAKE,
                        config,
                        type,
                        runtime_type);
                for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                        it = plugin_config.plugin_hosts.begin (),
                        end = plugin_config.plugin_hosts.end (); it != end; ++it) {
                    const thekogans_make &host_config =
                        thekogans_make::GetConfig (
                            (*it)->GetProjectRoot (),
                            (*it)->GetConfigFile (),
                            (*it)->GetGenerator (),
                            config,
                            type,
                            runtime_type);
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        if (host_config.project_type == PROJECT_TYPE_LIBRARY) {
                            InstallLibrary ((*it)->GetProjectRoot ());
                        }
                        else if (host_config.project_type == PROJECT_TYPE_PROGRAM) {
                            InstallProgram ((*it)->GetProjectRoot ());
                        }
                        else if (host_config.project_type == PROJECT_TYPE_PLUGIN) {
                            InstallPlugin ((*it)->GetProjectRoot ());
                        }
                    }
                }
            }

            namespace {
                inline std::string VariableTest (
                        const std::string &variable,
                        const std::string &value) {
                    return "$(" + variable + ") == '" + value + "'";
                }
            }

            void Installer::InstallLibrary (
                    const thekogans_make &DebugSharedShared,
                    const thekogans_make &DebugSharedStatic,
                    const thekogans_make &DebugStaticShared,
                    const thekogans_make &DebugStaticStatic,
                    const thekogans_make &ReleaseSharedShared,
                    const thekogans_make &ReleaseSharedStatic,
                    const thekogans_make &ReleaseStaticShared,
                    const thekogans_make &ReleaseStaticStatic) {
                std::list<thekogans_make::Dependency *> commonDependencies;
                GetCommonDependencies (
                    DebugSharedShared,
                    DebugSharedStatic,
                    DebugStaticShared,
                    DebugStaticStatic,
                    ReleaseSharedShared,
                    ReleaseSharedStatic,
                    ReleaseStaticShared,
                    ReleaseStaticStatic,
                    commonDependencies);
                std::list<thekogans_make::Dependency *> DebugSharedSharedDependencies;
                std::list<thekogans_make::Dependency *> DebugSharedStaticDependencies;
                std::list<thekogans_make::Dependency *> DebugStaticSharedDependencies;
                std::list<thekogans_make::Dependency *> DebugStaticStaticDependencies;
                std::list<thekogans_make::Dependency *> ReleaseSharedSharedDependencies;
                std::list<thekogans_make::Dependency *> ReleaseSharedStaticDependencies;
                std::list<thekogans_make::Dependency *> ReleaseStaticSharedDependencies;
                std::list<thekogans_make::Dependency *> ReleaseStaticStaticDependencies;
                GetUniqueDependencies (
                    DebugSharedShared,
                    DebugSharedStatic,
                    DebugStaticShared,
                    DebugStaticStatic,
                    ReleaseSharedShared,
                    ReleaseSharedStatic,
                    ReleaseStaticShared,
                    ReleaseStaticStatic,
                    commonDependencies,
                    DebugSharedSharedDependencies,
                    DebugSharedStaticDependencies,
                    DebugStaticSharedDependencies,
                    DebugStaticStaticDependencies,
                    ReleaseSharedSharedDependencies,
                    ReleaseSharedStaticDependencies,
                    ReleaseStaticSharedDependencies,
                    ReleaseStaticStaticDependencies);
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = commonDependencies.begin (),
                        end = commonDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = DebugSharedSharedDependencies.begin (),
                        end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = DebugSharedStaticDependencies.begin (),
                        end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = DebugStaticSharedDependencies.begin (),
                        end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = DebugStaticStaticDependencies.begin (),
                        end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = ReleaseSharedSharedDependencies.begin (),
                        end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = ReleaseSharedStaticDependencies.begin (),
                        end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = ReleaseStaticSharedDependencies.begin (),
                        end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency *>::const_iterator
                        it = ReleaseStaticStaticDependencies.begin (),
                        end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                (*it)->GetRuntimeType ()));
                    }
                }
                // Uninstall old version
                UninstallLibrary (
                    DebugSharedShared.organization,
                    DebugSharedShared.project,
                    DebugSharedShared.GetVersion (),
                    false);
                std::cout << "Installing " << DebugSharedShared.project_root << std::endl;
                std::cout.flush ();
                // install = "yes"
                std::set<InstallPaths> installPaths;
                GetInstallPaths (DebugSharedShared, installPaths);
                GetInstallPaths (DebugSharedStatic, installPaths);
                GetInstallPaths (DebugStaticShared, installPaths);
                GetInstallPaths (DebugStaticStatic, installPaths);
                GetInstallPaths (ReleaseSharedShared, installPaths);
                GetInstallPaths (ReleaseSharedStatic, installPaths);
                GetInstallPaths (ReleaseStaticShared, installPaths);
                GetInstallPaths (ReleaseStaticStatic, installPaths);
                if (DebugSharedShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugSharedShared.GetProjectGoal (),
                            DebugSharedShared.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            DebugSharedShared.GetProjectLinkLibrary (),
                            DebugSharedShared.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (DebugSharedStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugSharedStatic.GetProjectGoal (),
                            DebugSharedStatic.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            DebugSharedStatic.GetProjectLinkLibrary (),
                            DebugSharedStatic.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (DebugStaticShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugStaticShared.GetProjectGoal (),
                            DebugStaticShared.GetToolchainGoal ()));
                }
                if (DebugStaticStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugStaticStatic.GetProjectGoal (),
                            DebugStaticStatic.GetToolchainGoal ()));
                }
                if (ReleaseSharedShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseSharedShared.GetProjectGoal (),
                            ReleaseSharedShared.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            ReleaseSharedShared.GetProjectLinkLibrary (),
                            ReleaseSharedShared.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (ReleaseSharedStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseSharedStatic.GetProjectGoal (),
                            ReleaseSharedStatic.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            ReleaseSharedStatic.GetProjectLinkLibrary (),
                            ReleaseSharedStatic.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (ReleaseStaticShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseStaticShared.GetProjectGoal (),
                            ReleaseStaticShared.GetToolchainGoal ()));
                }
                if (ReleaseStaticStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseStaticStatic.GetProjectGoal (),
                            ReleaseStaticStatic.GetToolchainGoal ()));
                }
                for (std::set<InstallPaths>::const_iterator
                        it = installPaths.begin (),
                        end = installPaths.end (); it != end; ++it) {
                    CopyFile ((*it).first, (*it).second);
                }
                std::string config_file =
                    MakePath (
                        MakePath (_TOOLCHAIN_DIR, CONFIG_DIR),
                        GetFileName (
                            DebugSharedShared.organization,
                            DebugSharedShared.project,
                            std::string (),
                            DebugSharedShared.GetVersion (),
                            XML_EXT));
                std::cout << "Creating " << config_file << "\n";
                std::cout.flush ();
                std::string configFilePath = ToSystemPath (config_file);
                util::Directory::Create (util::Path (configFilePath).GetDirectory ());
                std::fstream configFile (
                    configFilePath.c_str (),
                    std::fstream::out | std::fstream::trunc);
                if (configFile.is_open ()) {
                    util::Attributes attributes;
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_ORGANIZATION,
                            DebugSharedShared.organization));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PROJECT,
                            DebugSharedShared.project));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PROJECT_TYPE,
                            DebugSharedShared.project_type));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_MAJOR_VERSION,
                            DebugSharedShared.major_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_MINOR_VERSION,
                            DebugSharedShared.minor_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PATCH_VERSION,
                            DebugSharedShared.patch_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_NAMING_CONVENTION,
                            DebugSharedShared.naming_convention));
                    std::string build_config = config;
                    if (build_config.empty ()) {
                        build_config = DebugSharedShared.build_config;
                    }
                    if (!build_config.empty ()) {
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_BUILD_CONFIG,
                                build_config));
                    }
                    std::string build_type = type;
                    if (build_type.empty ()) {
                        build_type = DebugSharedShared.build_type;
                    }
                    if (!build_type.empty ()) {
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_BUILD_TYPE,
                                build_type));
                    }
                    std::string build_runtime_type = runtime_type;
                    if (build_runtime_type.empty ()) {
                        build_runtime_type = DebugSharedShared.build_runtime_type;
                    }
                    if (!build_runtime_type.empty ()) {
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_BUILD_RUNTIME_TYPE,
                                build_runtime_type));
                    }
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_GUID,
                            DebugSharedShared.guid.ToString ()));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_SCHEMA_VERSION,
                            util::ui32Tostring (THEKOGANS_MAKE_XML_SCHEMA_VERSION)));
                    configFile << util::OpenTag (0, thekogans_make::TAG_THEKOGANS_MAKE,
                        attributes, false, true);
                    // features
                    if (!DebugSharedShared.features.empty () ||
                            !DebugSharedStatic.features.empty () ||
                            !DebugStaticShared.features.empty () ||
                            !DebugStaticStatic.features.empty () ||
                            !ReleaseSharedShared.features.empty () ||
                            !ReleaseSharedStatic.features.empty () ||
                            !ReleaseStaticShared.features.empty () ||
                            !ReleaseStaticStatic.features.empty ()) {
                        configFile << util::OpenTag (1, thekogans_make::TAG_FEATURES,
                            util::Attributes (), false, true);
                        std::set<std::string> commonFeatures;
                        GetCommonFeatures (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic,
                            commonFeatures);
                        for (std::set<std::string>::const_iterator
                                it = commonFeatures.begin (),
                                end = commonFeatures.end (); it != end; ++it) {
                            configFile <<
                                util::OpenTag (2, thekogans_make::TAG_FEATURE) <<
                                *it <<
                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                        }
                        std::set<std::string> DebugSharedSharedFeatures;
                        std::set<std::string> DebugSharedStaticFeatures;
                        std::set<std::string> DebugStaticSharedFeatures;
                        std::set<std::string> DebugStaticStaticFeatures;
                        std::set<std::string> ReleaseSharedSharedFeatures;
                        std::set<std::string> ReleaseSharedStaticFeatures;
                        std::set<std::string> ReleaseStaticSharedFeatures;
                        std::set<std::string> ReleaseStaticStaticFeatures;
                        GetUniqueFeatures (
                            DebugSharedShared,
                            DebugSharedStatic,
                            DebugStaticShared,
                            DebugStaticStatic,
                            ReleaseSharedShared,
                            ReleaseSharedStatic,
                            ReleaseStaticShared,
                            ReleaseStaticStatic,
                            commonFeatures,
                            DebugSharedSharedFeatures,
                            DebugSharedStaticFeatures,
                            DebugStaticSharedFeatures,
                            DebugStaticStaticFeatures,
                            ReleaseSharedSharedFeatures,
                            ReleaseSharedStaticFeatures,
                            ReleaseStaticSharedFeatures,
                            ReleaseStaticStaticFeatures);
                        // debug && release
                        if ((!DebugSharedSharedFeatures.empty () ||
                                !DebugSharedStaticFeatures.empty () ||
                                !DebugStaticSharedFeatures.empty () ||
                                !DebugStaticStaticFeatures.empty ()) &&
                            (!ReleaseSharedSharedFeatures.empty () ||
                                !ReleaseSharedStaticFeatures.empty () ||
                                !ReleaseStaticSharedFeatures.empty () ||
                                !ReleaseStaticStaticFeatures.empty ())) {
                            // choose
                            configFile << util::OpenTag (2, thekogans_make::TAG_CHOOSE,
                                util::Attributes (), false, true);
                            {
                                // when debug
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                // shared && static
                                if ((!DebugSharedSharedFeatures.empty () || !DebugSharedStaticFeatures.empty ()) &&
                                        (!DebugStaticSharedFeatures.empty () || !DebugStaticStaticFeatures.empty ())) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!DebugSharedSharedFeatures.empty () && !DebugSharedStaticFeatures.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = DebugSharedSharedFeatures.begin (),
                                                        end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = DebugSharedStaticFeatures.begin (),
                                                        end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!DebugSharedSharedFeatures.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugSharedSharedFeatures.begin (),
                                                    end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugSharedStaticFeatures.begin (),
                                                     end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!DebugStaticSharedFeatures.empty () && !DebugStaticStaticFeatures.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = DebugStaticSharedFeatures.begin (),
                                                        end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = DebugStaticStaticFeatures.begin (),
                                                        end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!DebugStaticSharedFeatures.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugStaticSharedFeatures.begin (),
                                                    end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugStaticStaticFeatures.begin (),
                                                     end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                // shared
                                else if (!DebugSharedSharedFeatures.empty () || !DebugSharedStaticFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugSharedSharedFeatures.empty () && !DebugSharedStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugSharedSharedFeatures.begin (),
                                                    end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugSharedStaticFeatures.begin (),
                                                    end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugSharedSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugSharedSharedFeatures.begin (),
                                                end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugSharedStaticFeatures.begin (),
                                                end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                // static
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugStaticSharedFeatures.empty () && !DebugStaticStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugStaticSharedFeatures.begin (),
                                                    end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = DebugStaticStaticFeatures.begin (),
                                                    end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugStaticSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticSharedFeatures.begin (),
                                                end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticStaticFeatures.begin (),
                                                end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            {
                                // when release
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                // shared && static
                                if ((!ReleaseSharedSharedFeatures.empty () || !ReleaseSharedStaticFeatures.empty ()) &&
                                        (!ReleaseStaticSharedFeatures.empty () || !ReleaseStaticStaticFeatures.empty ())) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!ReleaseSharedSharedFeatures.empty () && !ReleaseSharedStaticFeatures.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = ReleaseSharedSharedFeatures.begin (),
                                                        end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = ReleaseSharedStaticFeatures.begin (),
                                                        end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!ReleaseSharedSharedFeatures.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseSharedSharedFeatures.begin (),
                                                    end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseSharedStaticFeatures.begin (),
                                                     end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!ReleaseStaticSharedFeatures.empty () && !ReleaseStaticStaticFeatures.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = ReleaseStaticSharedFeatures.begin (),
                                                        end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::set<std::string>::const_iterator
                                                        it = ReleaseStaticStaticFeatures.begin (),
                                                        end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                                    configFile <<
                                                        util::OpenTag (8, thekogans_make::TAG_FEATURE) <<
                                                        *it <<
                                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!ReleaseStaticSharedFeatures.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseStaticSharedFeatures.begin (),
                                                    end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseStaticStaticFeatures.begin (),
                                                     end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                // shared
                                else if (!ReleaseSharedSharedFeatures.empty () || !ReleaseSharedStaticFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseSharedSharedFeatures.empty () && !ReleaseSharedStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseSharedSharedFeatures.begin (),
                                                    end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseSharedStaticFeatures.begin (),
                                                    end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseSharedSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseSharedSharedFeatures.begin (),
                                                end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseSharedStaticFeatures.begin (),
                                                end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                // static
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseStaticSharedFeatures.empty () && !ReleaseStaticStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseStaticSharedFeatures.begin (),
                                                    end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                    it = ReleaseStaticStaticFeatures.begin (),
                                                    end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseStaticSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticSharedFeatures.begin (),
                                                end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticStaticFeatures.begin (),
                                                end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_CHOOSE);
                        }
                        // debug
                        else if (!DebugSharedSharedFeatures.empty () ||
                                !DebugSharedStaticFeatures.empty () ||
                                !DebugStaticSharedFeatures.empty () ||
                                !DebugStaticStaticFeatures.empty ()) {
                            // if debug
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            // shared && static
                            if ((!DebugSharedSharedFeatures.empty () || !DebugSharedStaticFeatures.empty ()) &&
                                    (!DebugStaticSharedFeatures.empty () || !DebugStaticStaticFeatures.empty ())) {
                                // choose
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    // when shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugSharedSharedFeatures.empty () && !DebugSharedStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugSharedSharedFeatures.begin (),
                                                     end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugSharedStaticFeatures.begin (),
                                                     end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugSharedSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                 it = DebugSharedSharedFeatures.begin (),
                                                 end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                 it = DebugSharedStaticFeatures.begin (),
                                                 end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    // when static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugStaticSharedFeatures.empty () && !DebugStaticStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugStaticSharedFeatures.begin (),
                                                     end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = DebugStaticStaticFeatures.begin (),
                                                     end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugStaticSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticSharedFeatures.begin (),
                                                end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticStaticFeatures.begin (),
                                                end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            // shared
                            else if (!DebugSharedSharedFeatures.empty () || !DebugSharedStaticFeatures.empty ()) {
                                // if shared
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!DebugSharedSharedFeatures.empty () && !DebugSharedStaticFeatures.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugSharedSharedFeatures.begin (),
                                                end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugSharedStaticFeatures.begin (),
                                                end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!DebugSharedSharedFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugSharedSharedFeatures.begin (),
                                            end = DebugSharedSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugSharedStaticFeatures.begin (),
                                            end = DebugSharedStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            // static
                            else {
                                // if static
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!DebugStaticSharedFeatures.empty () && !DebugStaticStaticFeatures.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticSharedFeatures.begin (),
                                                end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticStaticFeatures.begin (),
                                                end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!DebugStaticSharedFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugStaticSharedFeatures.begin (),
                                            end = DebugStaticSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugStaticStaticFeatures.begin (),
                                            end = DebugStaticStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        // release
                        else if (!ReleaseSharedSharedFeatures.empty () ||
                                !ReleaseSharedStaticFeatures.empty () ||
                                !ReleaseStaticSharedFeatures.empty () ||
                                !ReleaseStaticStaticFeatures.empty ()) {
                            // if release
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            // shared && static
                            if ((!ReleaseSharedSharedFeatures.empty () || !ReleaseSharedStaticFeatures.empty ()) &&
                                    (!ReleaseStaticSharedFeatures.empty () || !ReleaseStaticStaticFeatures.empty ())) {
                                // choose
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    // when shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseSharedSharedFeatures.empty () && !ReleaseSharedStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseSharedSharedFeatures.begin (),
                                                     end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseSharedStaticFeatures.begin (),
                                                     end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseSharedSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                 it = ReleaseSharedSharedFeatures.begin (),
                                                 end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                 it = ReleaseSharedStaticFeatures.begin (),
                                                 end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    // when static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseStaticSharedFeatures.empty () && !ReleaseStaticStaticFeatures.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseStaticSharedFeatures.begin (),
                                                     end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::set<std::string>::const_iterator
                                                     it = ReleaseStaticStaticFeatures.begin (),
                                                     end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                                configFile <<
                                                    util::OpenTag (7, thekogans_make::TAG_FEATURE) <<
                                                    *it <<
                                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseStaticSharedFeatures.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticSharedFeatures.begin (),
                                                end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticStaticFeatures.begin (),
                                                end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            // shared
                            else if (!ReleaseSharedSharedFeatures.empty () || !ReleaseSharedStaticFeatures.empty ()) {
                                // if shared
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!ReleaseSharedSharedFeatures.empty () && !ReleaseSharedStaticFeatures.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseSharedSharedFeatures.begin (),
                                                end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseSharedStaticFeatures.begin (),
                                                end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!ReleaseSharedSharedFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseSharedSharedFeatures.begin (),
                                            end = ReleaseSharedSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseSharedStaticFeatures.begin (),
                                            end = ReleaseSharedStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            // static
                            else {
                                // if static
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!ReleaseStaticSharedFeatures.empty () && !ReleaseStaticStaticFeatures.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticSharedFeatures.begin (),
                                                end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticStaticFeatures.begin (),
                                                end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!ReleaseStaticSharedFeatures.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseStaticSharedFeatures.begin (),
                                            end = ReleaseStaticSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseStaticStaticFeatures.begin (),
                                            end = ReleaseStaticStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        configFile << util::CloseTag (1, thekogans_make::TAG_FEATURES);
                    }
                    // dependencies
                    if (!commonDependencies.empty () ||
                            !DebugSharedSharedDependencies.empty () ||
                            !DebugSharedStaticDependencies.empty () ||
                            !DebugStaticSharedDependencies.empty () ||
                            !DebugStaticStaticDependencies.empty () ||
                            !ReleaseSharedSharedDependencies.empty () ||
                            !ReleaseSharedStaticDependencies.empty () ||
                            !ReleaseStaticSharedDependencies.empty () ||
                            !ReleaseStaticStaticDependencies.empty ()) {
                        configFile << util::OpenTag (1, thekogans_make::TAG_DEPENDENCIES,
                            util::Attributes (), false, true);
                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                it = commonDependencies.begin (),
                                end = commonDependencies.end (); it != end; ++it) {
                            configFile << (*it)->ToString (2);
                        }
                        // debug && release
                        if ((!DebugSharedSharedDependencies.empty () ||
                                !DebugSharedStaticDependencies.empty () ||
                                !DebugStaticSharedDependencies.empty () ||
                                !DebugStaticStaticDependencies.empty ()) &&
                            (!ReleaseSharedSharedDependencies.empty () ||
                                !ReleaseSharedStaticDependencies.empty () ||
                                !ReleaseStaticSharedDependencies.empty () ||
                                !ReleaseStaticStaticDependencies.empty ())) {
                            // choose
                            configFile << util::OpenTag (2, thekogans_make::TAG_CHOOSE,
                                util::Attributes (), false, true);
                            {
                                // when debug
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                // shared && static
                                if ((!DebugSharedSharedDependencies.empty () || !DebugSharedStaticDependencies.empty ()) &&
                                        (!DebugStaticSharedDependencies.empty () || !DebugStaticStaticDependencies.empty ())) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!DebugSharedSharedDependencies.empty () && !DebugSharedStaticDependencies.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = DebugSharedSharedDependencies.begin (),
                                                        end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = DebugSharedStaticDependencies.begin (),
                                                        end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!DebugSharedSharedDependencies.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugSharedSharedDependencies.begin (),
                                                    end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugSharedStaticDependencies.begin (),
                                                     end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!DebugStaticSharedDependencies.empty () && !DebugStaticStaticDependencies.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = DebugStaticSharedDependencies.begin (),
                                                        end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = DebugStaticStaticDependencies.begin (),
                                                        end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!DebugStaticSharedDependencies.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugStaticSharedDependencies.begin (),
                                                    end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugStaticStaticDependencies.begin (),
                                                     end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                // shared
                                else if (!DebugSharedSharedDependencies.empty () || !DebugSharedStaticDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugSharedSharedDependencies.empty () && !DebugSharedStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugSharedSharedDependencies.begin (),
                                                    end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugSharedStaticDependencies.begin (),
                                                    end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugSharedSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugSharedSharedDependencies.begin (),
                                                end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugSharedStaticDependencies.begin (),
                                                end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                // static
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugStaticSharedDependencies.empty () && !DebugStaticStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugStaticSharedDependencies.begin (),
                                                    end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = DebugStaticStaticDependencies.begin (),
                                                    end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugStaticSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticSharedDependencies.begin (),
                                                end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticStaticDependencies.begin (),
                                                end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            {
                                // when release
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                // shared && static
                                if ((!ReleaseSharedSharedDependencies.empty () || !ReleaseSharedStaticDependencies.empty ()) &&
                                        (!ReleaseStaticSharedDependencies.empty () || !ReleaseStaticStaticDependencies.empty ())) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!ReleaseSharedSharedDependencies.empty () && !ReleaseSharedStaticDependencies.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = ReleaseSharedSharedDependencies.begin (),
                                                        end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = ReleaseSharedStaticDependencies.begin (),
                                                        end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!ReleaseSharedSharedDependencies.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseSharedSharedDependencies.begin (),
                                                    end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseSharedStaticDependencies.begin (),
                                                     end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        // shared && static
                                        if (!ReleaseStaticSharedDependencies.empty () && !ReleaseStaticStaticDependencies.empty ()) {
                                            // choose
                                            configFile << util::OpenTag (6, thekogans_make::TAG_CHOOSE,
                                                util::Attributes (), false, true);
                                            {
                                                // when shared
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = ReleaseStaticSharedDependencies.begin (),
                                                        end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            {
                                                // when static
                                                util::Attributes attributes;
                                                attributes.push_back (
                                                    util::Attribute (
                                                        thekogans_make::ATTR_CONDITION,
                                                        VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                                configFile << util::OpenTag (7, thekogans_make::TAG_WHEN,
                                                    attributes, false, true);
                                                for (std::list<thekogans_make::Dependency *>::const_iterator
                                                        it = ReleaseStaticStaticDependencies.begin (),
                                                        end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                                    configFile << (*it)->ToString (8);
                                                }
                                                configFile << util::CloseTag (7, thekogans_make::TAG_WHEN);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_CHOOSE);
                                        }
                                        // shared
                                        else if (!ReleaseStaticSharedDependencies.empty ()) {
                                            // if shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseStaticSharedDependencies.begin (),
                                                    end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        // static
                                        else {
                                            // if static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_IF,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseStaticStaticDependencies.begin (),
                                                     end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_IF);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                // shared
                                else if (!ReleaseSharedSharedDependencies.empty () || !ReleaseSharedStaticDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseSharedSharedDependencies.empty () && !ReleaseSharedStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseSharedSharedDependencies.begin (),
                                                    end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseSharedStaticDependencies.begin (),
                                                    end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseSharedSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseSharedSharedDependencies.begin (),
                                                end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseSharedStaticDependencies.begin (),
                                                end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                // static
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseStaticSharedDependencies.empty () && !ReleaseStaticStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseStaticSharedDependencies.begin (),
                                                    end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                    it = ReleaseStaticStaticDependencies.begin (),
                                                    end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseStaticSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticSharedDependencies.begin (),
                                                end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticStaticDependencies.begin (),
                                                end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_CHOOSE);
                        }
                        // debug
                        else if (!DebugSharedSharedDependencies.empty () ||
                                !DebugSharedStaticDependencies.empty () ||
                                !DebugStaticSharedDependencies.empty () ||
                                !DebugStaticStaticDependencies.empty ()) {
                            // if debug
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            // shared && static
                            if ((!DebugSharedSharedDependencies.empty () || !DebugSharedStaticDependencies.empty ()) &&
                                    (!DebugStaticSharedDependencies.empty () || !DebugStaticStaticDependencies.empty ())) {
                                // choose
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    // when shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugSharedSharedDependencies.empty () && !DebugSharedStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugSharedSharedDependencies.begin (),
                                                     end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugSharedStaticDependencies.begin (),
                                                     end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugSharedSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                 it = DebugSharedSharedDependencies.begin (),
                                                 end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                 it = DebugSharedStaticDependencies.begin (),
                                                 end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    // when static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!DebugStaticSharedDependencies.empty () && !DebugStaticStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugStaticSharedDependencies.begin (),
                                                     end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = DebugStaticStaticDependencies.begin (),
                                                     end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!DebugStaticSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticSharedDependencies.begin (),
                                                end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticStaticDependencies.begin (),
                                                end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            // shared
                            else if (!DebugSharedSharedDependencies.empty () || !DebugSharedStaticDependencies.empty ()) {
                                // if shared
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!DebugSharedSharedDependencies.empty () && !DebugSharedStaticDependencies.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugSharedSharedDependencies.begin (),
                                                end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugSharedStaticDependencies.begin (),
                                                end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!DebugSharedSharedDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = DebugSharedSharedDependencies.begin (),
                                            end = DebugSharedSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = DebugSharedStaticDependencies.begin (),
                                            end = DebugSharedStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            // static
                            else {
                                // if static
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!DebugStaticSharedDependencies.empty () && !DebugStaticStaticDependencies.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticSharedDependencies.begin (),
                                                end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = DebugStaticStaticDependencies.begin (),
                                                end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!DebugStaticSharedDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = DebugStaticSharedDependencies.begin (),
                                            end = DebugStaticSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = DebugStaticStaticDependencies.begin (),
                                            end = DebugStaticStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        // release
                        else if (!ReleaseSharedSharedDependencies.empty () ||
                                !ReleaseSharedStaticDependencies.empty () ||
                                !ReleaseStaticSharedDependencies.empty () ||
                                !ReleaseStaticStaticDependencies.empty ()) {
                            // if release
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            // shared && static
                            if ((!ReleaseSharedSharedDependencies.empty () || !ReleaseSharedStaticDependencies.empty ()) &&
                                    (!ReleaseStaticSharedDependencies.empty () || !ReleaseStaticStaticDependencies.empty ())) {
                                // choose
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    // when shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseSharedSharedDependencies.empty () && !ReleaseSharedStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseSharedSharedDependencies.begin (),
                                                     end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseSharedStaticDependencies.begin (),
                                                     end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseSharedSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                 it = ReleaseSharedSharedDependencies.begin (),
                                                 end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                 it = ReleaseSharedStaticDependencies.begin (),
                                                 end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    // when static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    // shared && static
                                    if (!ReleaseStaticSharedDependencies.empty () && !ReleaseStaticStaticDependencies.empty ()) {
                                        // choose
                                        configFile << util::OpenTag (5, thekogans_make::TAG_CHOOSE,
                                            util::Attributes (), false, true);
                                        {
                                            // when shared
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseStaticSharedDependencies.begin (),
                                                     end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        {
                                            // when static
                                            util::Attributes attributes;
                                            attributes.push_back (
                                                util::Attribute (
                                                    thekogans_make::ATTR_CONDITION,
                                                    VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                            configFile << util::OpenTag (6, thekogans_make::TAG_WHEN,
                                                attributes, false, true);
                                            for (std::list<thekogans_make::Dependency *>::const_iterator
                                                     it = ReleaseStaticStaticDependencies.begin (),
                                                     end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                                configFile << (*it)->ToString (7);
                                            }
                                            configFile << util::CloseTag (6, thekogans_make::TAG_WHEN);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_CHOOSE);
                                    }
                                    // shared
                                    else if (!ReleaseStaticSharedDependencies.empty ()) {
                                        // if shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticSharedDependencies.begin (),
                                                end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    // static
                                    else {
                                        // if static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_IF,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticStaticDependencies.begin (),
                                                end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_IF);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            // shared
                            else if (!ReleaseSharedSharedDependencies.empty () || !ReleaseSharedStaticDependencies.empty ()) {
                                // if shared
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!ReleaseSharedSharedDependencies.empty () && !ReleaseSharedStaticDependencies.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseSharedSharedDependencies.begin (),
                                                end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseSharedStaticDependencies.begin (),
                                                end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!ReleaseSharedSharedDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = ReleaseSharedSharedDependencies.begin (),
                                            end = ReleaseSharedSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = ReleaseSharedStaticDependencies.begin (),
                                            end = ReleaseSharedStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            // static
                            else {
                                // if static
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                // shared && static
                                if (!ReleaseStaticSharedDependencies.empty () && !ReleaseStaticStaticDependencies.empty ()) {
                                    // choose
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        // when shared
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticSharedDependencies.begin (),
                                                end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        // when static
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency *>::const_iterator
                                                it = ReleaseStaticStaticDependencies.begin (),
                                                end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                // shared
                                else if (!ReleaseStaticSharedDependencies.empty ()) {
                                    // if shared
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = ReleaseStaticSharedDependencies.begin (),
                                            end = ReleaseStaticSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    // if static
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_RUNTIME_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency *>::const_iterator
                                            it = ReleaseStaticStaticDependencies.begin (),
                                            end = ReleaseStaticStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        configFile << util::CloseTag (1, thekogans_make::TAG_DEPENDENCIES);
                    }
                    configFile << util::CloseTag (0, thekogans_make::TAG_THEKOGANS_MAKE);
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to open: %s",
                        configFilePath.c_str ());
                }
            }

            void Installer::InstallDependency (const thekogans_make &dependency) {
                if (dependency.config_file == THEKOGANS_MAKE_XML) {
                    if (dependency.project_type == PROJECT_TYPE_LIBRARY) {
                        InstallLibrary (dependency.project_root);
                    }
                    else if (dependency.project_type == PROJECT_TYPE_PROGRAM) {
                        InstallProgram (dependency.project_root);
                    }
                    else if (dependency.project_type == PROJECT_TYPE_PLUGIN) {
                        InstallPlugin (dependency.project_root);
                    }
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
