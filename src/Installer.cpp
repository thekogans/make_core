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
#include "thekogans/util/Environment.h"
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
                        const thekogans_make &DebugShared,
                        const thekogans_make &DebugStatic,
                        const thekogans_make &ReleaseShared,
                        const thekogans_make &ReleaseStatic,
                        std::set<std::string> &commonFeatures) {
                    for (auto feature : DebugShared.features) {
                        if (DebugStatic.features.find (feature) != DebugStatic.features.end () &&
                                ReleaseShared.features.find (feature) != ReleaseShared.features.end () &&
                                ReleaseStatic.features.find (feature) != ReleaseStatic.features.end ()) {
                            commonFeatures.insert (feature);
                        }
                    }
                }

                void GetUniqueFeatures (
                        const thekogans_make &DebugShared,
                        const thekogans_make &DebugStatic,
                        const thekogans_make &ReleaseShared,
                        const thekogans_make &ReleaseStatic,
                        const std::set<std::string> &commonFeatures,
                        std::set<std::string> &DebugSharedFeatures,
                        std::set<std::string> &DebugStaticFeatures,
                        std::set<std::string> &ReleaseSharedFeatures,
                        std::set<std::string> &ReleaseStaticFeatures) {
                    for (auto feature :DebugShared.features) {
                        if (commonFeatures.find (feature) == commonFeatures.end ()) {
                            DebugSharedFeatures.insert (feature);
                        }
                    }
                    for (auto feature : DebugStatic.features) {
                        if (commonFeatures.find (feature) == commonFeatures.end ()) {
                            DebugStaticFeatures.insert (feature);
                        }
                    }
                    for (auto feature : ReleaseShared.features) {
                        if (commonFeatures.find (feature) == commonFeatures.end ()) {
                            ReleaseSharedFeatures.insert (feature);
                        }
                    }
                    for (auto feature : ReleaseStatic.features) {
                        if (commonFeatures.find (feature) == commonFeatures.end ()) {
                            ReleaseStaticFeatures.insert (feature);
                        }
                    }
                }

                bool ContainsDependency (
                        const thekogans_make &config,
                        const thekogans_make::Dependency &dependency) {
                    for (auto dependency_ : config.dependencies) {
                        if (dependency_->EquivalentTo (dependency)) {
                            return true;
                        }
                    }
                    return false;
                }

                void GetCommonDependencies (
                        const thekogans_make &DebugShared,
                        const thekogans_make &DebugStatic,
                        const thekogans_make &ReleaseShared,
                        const thekogans_make &ReleaseStatic,
                        std::list<thekogans_make::Dependency::SharedPtr> &commonDependencies) {
                    for (auto dependency : DebugShared.dependencies) {
                        if (ContainsDependency (DebugStatic, *dependency) &&
                                ContainsDependency (ReleaseShared, *dependency) &&
                                ContainsDependency (ReleaseStatic, *dependency)) {
                            commonDependencies.push_back (dependency);
                        }
                    }
                }

                bool DoesNotContainDependency (
                        const std::list<thekogans_make::Dependency::SharedPtr> &commonDependencies,
                        const thekogans_make::Dependency &dependency) {
                    for (auto dependency_ : commonDependencies) {
                        if (dependency_->EquivalentTo (dependency)) {
                            return false;
                        }
                    }
                    return true;
                }

                void GetUniqueDependencies (
                        const thekogans_make &DebugShared,
                        const thekogans_make &DebugStatic,
                        const thekogans_make &ReleaseShared,
                        const thekogans_make &ReleaseStatic,
                        const std::list<thekogans_make::Dependency::SharedPtr> &commonDependencies,
                        std::list<thekogans_make::Dependency::SharedPtr> &DebugSharedDependencies,
                        std::list<thekogans_make::Dependency::SharedPtr> &DebugStaticDependencies,
                        std::list<thekogans_make::Dependency::SharedPtr> &ReleaseSharedDependencies,
                        std::list<thekogans_make::Dependency::SharedPtr> &ReleaseStaticDependencies) {
                    for (auto dependency : DebugShared.dependencies) {
                        if (DoesNotContainDependency (commonDependencies, *dependency)) {
                            DebugSharedDependencies.push_back (dependency);
                        }
                    }
                    for (auto dependency : DebugStatic.dependencies) {
                        if (DoesNotContainDependency (commonDependencies, *dependency)) {
                            DebugStaticDependencies.push_back (dependency);
                        }
                    }
                    for (auto dependency : ReleaseShared.dependencies) {
                        if (DoesNotContainDependency (commonDependencies, *dependency)) {
                            ReleaseSharedDependencies.push_back (dependency);
                        }
                    }
                    for (auto dependency : ReleaseStatic.dependencies) {
                        if (DoesNotContainDependency (commonDependencies, *dependency)) {
                            ReleaseStaticDependencies.push_back (dependency);
                        }
                    }
                }

                using InstallPaths = std::pair<std::string, std::string>;

                void GetInstallPaths (
                        const thekogans_make &config,
                        const std::list<thekogans_make::FileList::SharedPtr> &fileLists,
                        std::set<InstallPaths> &installPaths) {
                    for (auto fileList : fileLists) {
                        if (fileList->install) {
                            std::string prefix = MakePath (config.project_root, fileList->prefix);
                            for (auto file : fileList->files) {
                                if (file->customBuild == nullptr) {
                                    installPaths.insert (
                                        InstallPaths (
                                            MakePath (prefix, file->name),
                                            MakePath (fileList->destinationPrefix, file->name)));
                                }
                                else {
                                    std::string prefix =
                                        MakePath (
                                            MakePath (
                                                config.project_root,
                                                GetBuildDirectory (config.generator, config.config, config.type)),
                                            fileList->prefix);
                                    for (auto output : file->customBuild->outputs) {
                                        installPaths.insert (
                                            InstallPaths (
                                                MakePath (prefix, output),
                                                MakePath (fileList->destinationPrefix, output)));
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
                    if (!install_config.empty () && !install_type.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            install_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type);
                        const thekogans_make &DebugStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type);
                        const thekogans_make &ReleaseShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type);
                        const thekogans_make &ReleaseStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                install_type);
                        InstallLibrary (
                            DebugShared,
                            DebugStatic,
                            ReleaseShared,
                            ReleaseStatic);
                    }
                    else if (!install_config.empty ()) {
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            install_config,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED);
                        const thekogans_make &DebugStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                install_config,
                                TYPE_STATIC);
                        InstallLibrary (DebugShared, DebugStatic, ReleaseShared, ReleaseStatic);
                    }
                    else if (!install_type.empty ()) {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            install_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            install_type,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type);
                        const thekogans_make &DebugStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                install_type);
                        const thekogans_make &ReleaseShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type);
                        const thekogans_make &ReleaseStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                install_type);
                        InstallLibrary (
                            DebugShared,
                            DebugStatic,
                            ReleaseShared,
                            ReleaseStatic);
                    }
                    else {
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_DEBUG,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_SHARED,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        BuildProject (
                            project_root,
                            CONFIG_RELEASE,
                            TYPE_STATIC,
                            MODE_INSTALL,
                            hide_commands,
                            parallel_build,
                            TARGET_ALL);
                        const thekogans_make &DebugShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_SHARED);
                        const thekogans_make &DebugStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_DEBUG,
                                TYPE_STATIC);
                        const thekogans_make &ReleaseShared =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_SHARED);
                        const thekogans_make &ReleaseStatic =
                            thekogans_make::GetConfig (
                                project_root,
                                THEKOGANS_MAKE_XML,
                                MAKE,
                                CONFIG_RELEASE,
                                TYPE_STATIC);
                        InstallLibrary (
                            DebugShared,
                            DebugStatic,
                            ReleaseShared,
                            ReleaseStatic);
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
                    BuildProject (
                        project_root,
                        install_config,
                        install_type,
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
                            install_type);
                    std::list<std::string> dependencies;
                    for (auto dependency : config.dependencies) {
                        if (dependency->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            const core::thekogans_make &dependency_ = thekogans_make::GetConfig (
                                dependency->GetProjectRoot (),
                                dependency->GetConfigFile (),
                                dependency->GetGenerator (),
                                dependency->GetConfig (),
                                dependency->GetType ());
                            if (dependency_.project_type == PROJECT_TYPE_PROGRAM ||
                                    dependency_.project_type == PROJECT_TYPE_PLUGIN) {
                                InstallDependency (dependency_);
                                dependencies.push_back (dependency->ToString (2));
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
                    for (auto installPath :  installPaths) {
                        CopyFile (installPath.first, installPath.second);
                    }
                    CopyDependencies (
                        project_root,
                        install_config,
                        install_type,
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
                        // NOTE: We don't tag the config file with install_config and
                        // install_type because, unlike libraries (and plugins), program
                        // dependency is config and type independent.
//                         attributes.push_back (
//                             util::Attribute (
//                                 thekogans_make::ATTR_BUILD_CONFIG,
//                                 install_config));
//                         attributes.push_back (
//                             util::Attribute (
//                                 thekogans_make::ATTR_BUILD_TYPE,
//                                 install_type));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_GUID,
                                config.guid.ToHexString ()));
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
                            for (const auto &feature : config.features) {
                                configFile <<
                                    util::OpenTag (2, thekogans_make::TAG_FEATURE) <<
                                    feature <<
                                    util::CloseTag (0, thekogans_make::TAG_FEATURE);
                            }
                            configFile << util::CloseTag (1, thekogans_make::TAG_FEATURES);
                        }
                        // dependencies
                        if (!dependencies.empty ()) {
                            configFile << util::OpenTag (1, thekogans_make::TAG_DEPENDENCIES,
                                util::Attributes (), false, true);
                            for (const auto &dependency : dependencies) {
                                configFile << dependency;
                            }
                            configFile << util::CloseTag (1, thekogans_make::TAG_DEPENDENCIES);
                        }
                        // resources
                        for (auto resource : config.resources) {
                            if (resource->install) {
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
                                for (auto file : resource->files) {
                                    if (file->customBuild == nullptr) {
                                        configFile <<
                                            util::OpenTag (2, thekogans_make::TAG_RESOURCE) <<
                                            file->name <<
                                            util::CloseTag (0, thekogans_make::TAG_RESOURCE);
                                    }
                                    else {
                                        for (auto output : file->customBuild->outputs) {
                                            configFile <<
                                                util::OpenTag (2, thekogans_make::TAG_RESOURCE) <<
                                                output <<
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
                    BuildProject (
                        project_root,
                        install_config,
                        install_type,
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
                            install_type);
                    for (auto plugin_host : plugin_config.plugin_hosts) {
                        const thekogans_make &host_config =
                            thekogans_make::GetConfig (
                                plugin_host->GetProjectRoot (),
                                plugin_host->GetConfigFile (),
                                plugin_host->GetGenerator (),
                                install_config,
                                install_type);
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
                                util::Plugins::Plugin::SharedPtr plugin = plugins.GetPlugin (pluginFileName);
                                if (plugin != nullptr) {
                                    for (const auto &dependency : plugin->dependencies) {
                                        if (manifest.DeleteFile (dependency, pluginFileName)) {
                                            DeleteFile (MakePath (toDirectory, dependency));
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
                                    toDirectory);
                                util::Plugins::Plugin::Dependencies dependencies;
                                {
                                    std::set<std::string> sharedLibraries;
                                    plugin_config.GetSharedLibraries (sharedLibraries);
                                    for (const auto &sharedLibrary : sharedLibraries) {
                                        dependencies.insert (util::Path (sharedLibrary).GetFullFileName ());
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
                        type);
                for (auto plugin_host : plugin_config.plugin_hosts) {
                    const thekogans_make &host_config =
                        thekogans_make::GetConfig (
                            plugin_host->GetProjectRoot (),
                            plugin_host->GetConfigFile (),
                            plugin_host->GetGenerator (),
                            config,
                            type);
                    if (plugin_host->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        if (host_config.project_type == PROJECT_TYPE_PROGRAM) {
                            InstallProgram (plugin_host->GetProjectRoot ());
                        }
                        else if (host_config.project_type == PROJECT_TYPE_LIBRARY) {
                            InstallLibrary (plugin_host->GetProjectRoot ());
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
                    const thekogans_make &DebugShared,
                    const thekogans_make &DebugStatic,
                    const thekogans_make &ReleaseShared,
                    const thekogans_make &ReleaseStatic) {
                std::list<thekogans_make::Dependency::SharedPtr> commonDependencies;
                GetCommonDependencies (
                    DebugShared,
                    DebugStatic,
                    ReleaseShared,
                    ReleaseStatic,
                    commonDependencies);
                std::list<thekogans_make::Dependency::SharedPtr> DebugSharedDependencies;
                std::list<thekogans_make::Dependency::SharedPtr> DebugStaticDependencies;
                std::list<thekogans_make::Dependency::SharedPtr> ReleaseSharedDependencies;
                std::list<thekogans_make::Dependency::SharedPtr> ReleaseStaticDependencies;
                GetUniqueDependencies (
                    DebugShared,
                    DebugStatic,
                    ReleaseShared,
                    ReleaseStatic,
                    commonDependencies,
                    DebugSharedDependencies,
                    DebugStaticDependencies,
                    ReleaseSharedDependencies,
                    ReleaseStaticDependencies);
                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                        it = commonDependencies.begin (),
                        end = commonDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                        it = DebugSharedDependencies.begin (),
                        end = DebugSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                        it = DebugStaticDependencies.begin (),
                        end = DebugStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                        it = ReleaseSharedDependencies.begin (),
                        end = ReleaseSharedDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ()));
                    }
                }
                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                        it = ReleaseStaticDependencies.begin (),
                        end = ReleaseStaticDependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        InstallDependency (
                            thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ()));
                    }
                }
                // Uninstall old version
                UninstallLibrary (
                    DebugShared.organization,
                    DebugShared.project,
                    DebugShared.GetVersion (),
                    false);
                std::cout << "Installing " << DebugShared.project_root << std::endl;
                std::cout.flush ();
                // install = "yes"
                std::set<InstallPaths> installPaths;
                GetInstallPaths (DebugShared, installPaths);
                GetInstallPaths (DebugStatic, installPaths);
                GetInstallPaths (ReleaseShared, installPaths);
                GetInstallPaths (ReleaseStatic, installPaths);
                if (DebugShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugShared.GetProjectGoal (),
                            DebugShared.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            DebugShared.GetProjectLinkLibrary (),
                            DebugShared.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (DebugStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            DebugStatic.GetProjectGoal (),
                            DebugStatic.GetToolchainGoal ()));
                }
                if (ReleaseShared.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseShared.GetProjectGoal (),
                            ReleaseShared.GetToolchainGoal ()));
                #if defined (TOOLCHAIN_OS_Windows)
                    installPaths.insert (
                        InstallPaths (
                            ReleaseShared.GetProjectLinkLibrary (),
                            ReleaseShared.GetToolchainLinkLibrary ()));
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                if (ReleaseStatic.HasGoal ()) {
                    installPaths.insert (
                        InstallPaths (
                            ReleaseStatic.GetProjectGoal (),
                            ReleaseStatic.GetToolchainGoal ()));
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
                            DebugShared.organization,
                            DebugShared.project,
                            std::string (),
                            DebugShared.GetVersion (),
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
                            DebugShared.organization));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PROJECT,
                            DebugShared.project));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PROJECT_TYPE,
                            DebugShared.project_type));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_MAJOR_VERSION,
                            DebugShared.major_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_MINOR_VERSION,
                            DebugShared.minor_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_PATCH_VERSION,
                            DebugShared.patch_version));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_NAMING_CONVENTION,
                            DebugShared.naming_convention));
                    std::string build_config = config;
                    if (build_config.empty ()) {
                        build_config = DebugShared.build_config;
                    }
                    if (!build_config.empty ()) {
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_BUILD_CONFIG,
                                build_config));
                    }
                    std::string build_type = type;
                    if (build_type.empty ()) {
                        build_type = DebugShared.build_type;
                    }
                    if (!build_type.empty ()) {
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_BUILD_TYPE,
                                build_type));
                    }
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_GUID,
                            DebugShared.guid.ToHexString ()));
                    attributes.push_back (
                        util::Attribute (
                            thekogans_make::ATTR_SCHEMA_VERSION,
                            util::ui32Tostring (THEKOGANS_MAKE_XML_SCHEMA_VERSION)));
                    configFile << util::OpenTag (0, thekogans_make::TAG_THEKOGANS_MAKE,
                        attributes, false, true);
                    // features
                    if (!DebugShared.features.empty () ||
                            !DebugStatic.features.empty () ||
                            !ReleaseShared.features.empty () ||
                            !ReleaseStatic.features.empty ()) {
                        configFile << util::OpenTag (1, thekogans_make::TAG_FEATURES,
                            util::Attributes (), false, true);
                        std::set<std::string> commonFeatures;
                        GetCommonFeatures (DebugShared, DebugStatic,
                            ReleaseShared, ReleaseStatic, commonFeatures);
                        for (std::set<std::string>::const_iterator
                                it = commonFeatures.begin (),
                                end = commonFeatures.end (); it != end; ++it) {
                            configFile <<
                                util::OpenTag (2, thekogans_make::TAG_FEATURE) <<
                                *it <<
                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                        }
                        std::set<std::string> DebugSharedFeatures;
                        std::set<std::string> DebugStaticFeatures;
                        std::set<std::string> ReleaseSharedFeatures;
                        std::set<std::string> ReleaseStaticFeatures;
                        GetUniqueFeatures (
                            DebugShared,
                            DebugStatic,
                            ReleaseShared,
                            ReleaseStatic,
                            commonFeatures,
                            DebugSharedFeatures,
                            DebugStaticFeatures,
                            ReleaseSharedFeatures,
                            ReleaseStaticFeatures);
                        if ((!DebugSharedFeatures.empty () || !DebugStaticFeatures.empty ()) &&
                                (!ReleaseSharedFeatures.empty () || !ReleaseStaticFeatures.empty ())) {
                            configFile << util::OpenTag (2, thekogans_make::TAG_CHOOSE,
                                util::Attributes (), false, true);
                            {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                if (!DebugSharedFeatures.empty () && !DebugStaticFeatures.empty ()) {
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugSharedFeatures.begin (),
                                                end = DebugSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = DebugStaticFeatures.begin (),
                                                end = DebugStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                else if (!DebugSharedFeatures.empty ()) {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugSharedFeatures.begin (),
                                            end = DebugSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugStaticFeatures.begin (),
                                            end = DebugStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                if (!ReleaseSharedFeatures.empty () && !ReleaseStaticFeatures.empty ()) {
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseSharedFeatures.begin (),
                                                end = ReleaseSharedFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::set<std::string>::const_iterator
                                                it = ReleaseStaticFeatures.begin (),
                                                end = ReleaseStaticFeatures.end (); it != end; ++it) {
                                            configFile <<
                                                util::OpenTag (6, thekogans_make::TAG_FEATURE) <<
                                                *it <<
                                                util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                else if (!ReleaseSharedFeatures.empty ()) {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseSharedFeatures.begin (),
                                            end = ReleaseSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseStaticFeatures.begin (),
                                            end = ReleaseStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_CHOOSE);
                        }
                        else if (!DebugSharedFeatures.empty () || !DebugStaticFeatures.empty ()) {
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            if (!DebugSharedFeatures.empty () && !DebugStaticFeatures.empty ()) {
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    attributes, false, true);
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugSharedFeatures.begin (),
                                            end = DebugSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = DebugStaticFeatures.begin (),
                                            end = DebugStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_CHOOSE);
                            }
                            else if (!DebugSharedFeatures.empty ()) {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::set<std::string>::const_iterator
                                        it = DebugSharedFeatures.begin (),
                                        end = DebugSharedFeatures.end (); it != end; ++it) {
                                    configFile <<
                                        util::OpenTag (4, thekogans_make::TAG_FEATURE) <<
                                        *it <<
                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            else {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::set<std::string>::const_iterator
                                        it = DebugStaticFeatures.begin (),
                                        end = DebugStaticFeatures.end (); it != end; ++it) {
                                    configFile <<
                                        util::OpenTag (4, thekogans_make::TAG_FEATURE) <<
                                        *it <<
                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        else if (!ReleaseSharedFeatures.empty () || !ReleaseStaticFeatures.empty ()) {
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            if (!ReleaseSharedFeatures.empty () && !ReleaseStaticFeatures.empty ()) {
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseSharedFeatures.begin (),
                                            end = ReleaseSharedFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::set<std::string>::const_iterator
                                            it = ReleaseStaticFeatures.begin (),
                                            end = ReleaseStaticFeatures.end (); it != end; ++it) {
                                        configFile <<
                                            util::OpenTag (5, thekogans_make::TAG_FEATURE) <<
                                            *it <<
                                            util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_CHOOSE);
                            }
                            else if (!ReleaseSharedFeatures.empty ()) {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::set<std::string>::const_iterator
                                        it = ReleaseSharedFeatures.begin (),
                                        end = ReleaseSharedFeatures.end (); it != end; ++it) {
                                    configFile <<
                                        util::OpenTag (4, thekogans_make::TAG_FEATURE) <<
                                        *it <<
                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            else {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::set<std::string>::const_iterator
                                        it = ReleaseStaticFeatures.begin (),
                                        end = ReleaseStaticFeatures.end (); it != end; ++it) {
                                    configFile <<
                                        util::OpenTag (4, thekogans_make::TAG_FEATURE) <<
                                        *it <<
                                        util::CloseTag (0, thekogans_make::TAG_FEATURE);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        configFile << util::CloseTag (1, thekogans_make::TAG_FEATURES);
                    }
                    // dependencies
                    if (!commonDependencies.empty () ||
                            !DebugSharedDependencies.empty () ||
                            !DebugStaticDependencies.empty () ||
                            !ReleaseSharedDependencies.empty () ||
                            !ReleaseStaticDependencies.empty ()) {
                        configFile << util::OpenTag (1, thekogans_make::TAG_DEPENDENCIES,
                            util::Attributes (), false, true);
                        for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                it = commonDependencies.begin (),
                                end = commonDependencies.end (); it != end; ++it) {
                            configFile << (*it)->ToString (2);
                        }
                        if ((!DebugSharedDependencies.empty () || !DebugStaticDependencies.empty ()) &&
                                (!ReleaseSharedDependencies.empty () || !ReleaseStaticDependencies.empty ())) {
                            configFile << util::OpenTag (2, thekogans_make::TAG_CHOOSE,
                                util::Attributes (), false, true);
                            {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                if (!DebugSharedDependencies.empty () && !DebugStaticDependencies.empty ()) {
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                                it = DebugSharedDependencies.begin (),
                                                end = DebugSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                                it = DebugStaticDependencies.begin (),
                                                end = DebugStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                else if (!DebugSharedDependencies.empty ()) {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = DebugSharedDependencies.begin (),
                                            end = DebugSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = DebugStaticDependencies.begin (),
                                            end = DebugStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_WHEN,
                                    attributes, false, true);
                                if (!ReleaseSharedDependencies.empty () && !ReleaseStaticDependencies.empty ()) {
                                    configFile << util::OpenTag (4, thekogans_make::TAG_CHOOSE,
                                        util::Attributes (), false, true);
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                                it = ReleaseSharedDependencies.begin (),
                                                end = ReleaseSharedDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    {
                                        util::Attributes attributes;
                                        attributes.push_back (
                                            util::Attribute (
                                                thekogans_make::ATTR_CONDITION,
                                                VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                        configFile << util::OpenTag (5, thekogans_make::TAG_WHEN,
                                            attributes, false, true);
                                        for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                                it = ReleaseStaticDependencies.begin (),
                                                end = ReleaseStaticDependencies.end (); it != end; ++it) {
                                            configFile << (*it)->ToString (6);
                                        }
                                        configFile << util::CloseTag (5, thekogans_make::TAG_WHEN);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_CHOOSE);
                                }
                                else if (!ReleaseSharedDependencies.empty ()) {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = ReleaseSharedDependencies.begin (),
                                            end = ReleaseSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                else {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_IF,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = ReleaseStaticDependencies.begin (),
                                            end = ReleaseStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_IF);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_WHEN);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_CHOOSE);
                        }
                        else if (!DebugSharedDependencies.empty () || !DebugStaticDependencies.empty ()) {
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_DEBUG)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            if (!DebugSharedDependencies.empty () && !DebugStaticDependencies.empty ()) {
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = DebugSharedDependencies.begin (),
                                            end = DebugSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = DebugStaticDependencies.begin (),
                                            end = DebugStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_CHOOSE);
                            }
                            else if (!DebugSharedDependencies.empty ()) {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                        it = DebugSharedDependencies.begin (),
                                        end = DebugSharedDependencies.end (); it != end; ++it) {
                                    configFile << (*it)->ToString (4);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            else {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                        it = DebugStaticDependencies.begin (),
                                        end = DebugStaticDependencies.end (); it != end; ++it) {
                                    configFile << (*it)->ToString (4);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            configFile << util::CloseTag (2, thekogans_make::TAG_IF);
                        }
                        else if (!ReleaseSharedDependencies.empty () || !ReleaseStaticDependencies.empty ()) {
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONDITION,
                                    VariableTest (thekogans_make::VAR_CONFIG, CONFIG_RELEASE)));
                            configFile << util::OpenTag (2, thekogans_make::TAG_IF,
                                attributes, false, true);
                            if (!ReleaseSharedDependencies.empty () && !ReleaseStaticDependencies.empty ()) {
                                configFile << util::OpenTag (3, thekogans_make::TAG_CHOOSE,
                                    util::Attributes (), false, true);
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = ReleaseSharedDependencies.begin (),
                                            end = ReleaseSharedDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                {
                                    util::Attributes attributes;
                                    attributes.push_back (
                                        util::Attribute (
                                            thekogans_make::ATTR_CONDITION,
                                            VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                    configFile << util::OpenTag (4, thekogans_make::TAG_WHEN,
                                        attributes, false, true);
                                    for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                            it = ReleaseStaticDependencies.begin (),
                                            end = ReleaseStaticDependencies.end (); it != end; ++it) {
                                        configFile << (*it)->ToString (5);
                                    }
                                    configFile << util::CloseTag (4, thekogans_make::TAG_WHEN);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_CHOOSE);
                            }
                            else if (!ReleaseSharedDependencies.empty ()) {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_SHARED)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                        it = ReleaseSharedDependencies.begin (),
                                        end = ReleaseSharedDependencies.end (); it != end; ++it) {
                                    configFile << (*it)->ToString (4);
                                }
                                configFile << util::CloseTag (3, thekogans_make::TAG_IF);
                            }
                            else {
                                util::Attributes attributes;
                                attributes.push_back (
                                    util::Attribute (
                                        thekogans_make::ATTR_CONDITION,
                                        VariableTest (thekogans_make::VAR_TYPE, TYPE_STATIC)));
                                configFile << util::OpenTag (3, thekogans_make::TAG_IF,
                                    attributes, false, true);
                                for (std::list<thekogans_make::Dependency::SharedPtr>::const_iterator
                                        it = ReleaseStaticDependencies.begin (),
                                        end = ReleaseStaticDependencies.end (); it != end; ++it) {
                                    configFile << (*it)->ToString (4);
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
