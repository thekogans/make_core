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

#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/util/Version.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Directory.h"
#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
    #include "thekogans/make/core/Sources.h"
#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Project.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                bool InstallVersion (
                        const std::string &organization,
                        const std::string &project,
                        const std::string &branch,
                        const std::string &version,
                        const std::string &example) {
                    bool installed = Project::IsInstalled (
                        organization, project, branch, version, example);
                #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    if (!installed &&
                            ToolchainSources::Instance ().IsSourceProject (
                                organization, project, branch, version)) {
                        ToolchainSources::Instance ().GetSourceProject (
                            organization, project, branch, version);
                        installed = Project::IsInstalled (
                            organization, project, branch, version, example);
                        if (!installed) {
                            util::Path (
                                ToSystemPath (
                                    Project::GetRoot (
                                        organization,
                                        project,
                                        branch,
                                        version,
                                        std::string ()))).Delete ();
                        }
                    }
                #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    return installed;
                }
            }

            bool Project::Find (
                    const std::string &organization,
                    const std::string &project,
                    std::string &branch,
                    std::string &version,
                    const std::string &example) {
                bool installed = InstallVersion (
                    organization, project, branch, version, example);
                if (!installed) {
                    if (branch.empty ()) {
                        branch = GetDefaultBranch (organization, project);
                        if (!branch.empty ()) {
                            installed = InstallVersion (
                                organization, project, branch, version, example);
                        }
                    }
                    if (!installed && version.empty ()) {
                        version = GetDefaultVersion (organization, project);
                        if (!version.empty ()) {
                            installed = InstallVersion (
                                organization, project, branch, version, example);
                        }
                        if (!installed) {
                            std::vector<util::Version> versions;
                            versions.push_back (
                                util::Version (
                                    GetLatestVersion (organization, project, branch)));
                        #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                            versions.push_back (
                                util::Version (
                                    ToolchainSources::Instance ().GetSourceProjectLatestVersion (
                                        organization, project, branch)));
                            if (versions[0] < versions[1]) {
                                std::swap (versions[0], versions[1]);
                            }
                        #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                            for (std::size_t i = 0, count = versions.size (); !installed && i < count; ++i) {
                                if (versions[i] != util::Version::Empty) {
                                    version = versions[i].ToString ();
                                    installed = InstallVersion (
                                        organization, project, branch, version, example);
                                }
                            }
                        }
                    }
                }
                return installed;
            }

            bool Project::IsInstalled (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example) {
                return util::Path (
                    ToSystemPath (
                        GetConfig (
                            organization,
                            project,
                            branch,
                            version,
                            example))).Exists ();
            }

            // Versioned project directories can look like this:
            //
            // 1 - DEVELOPMENT_ROOT/organization/project-version
            // 2 - DEVELOPMENT_ROOT/organization/project/branch-version
            std::string Project::GetLatestVersion (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch) {
                util::Version latestVersion (0, 0, 0);
                std::string path;
                std::string fileTemplate;
                {
                    std::list<std::string> components;
                    components.push_back (_DEVELOPMENT_ROOT);
                    components.push_back (organization);
                    if (!branch.empty ()) {
                        // 2
                        components.push_back (GetDirectoryFromName (project));
                        util::Path branchDirectory (branch);
                        components.push_back (branchDirectory.GetDirectory ());
                        fileTemplate = branchDirectory.GetFullFileName ();
                    }
                    else {
                        // 1
                        util::Path projectDirectory (GetDirectoryFromName (project));
                        components.push_back (projectDirectory.GetDirectory ());
                        fileTemplate = projectDirectory.GetFullFileName ();
                    }
                    path = ToSystemPath (MakePath (components, false));
                    fileTemplate += "-%u.%u.%u";
                }
                if (util::Path (path).Exists ()) {
                    util::Directory directory (path);
                    util::Directory::Entry entry;
                    for (bool gotEntry = directory.GetFirstEntry (entry);
                            gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                        if (entry.type == util::Directory::Entry::Folder &&
                                !util::IsDotOrDotDot (entry.name.c_str ())) {
                            util::ui32 major_version = 0;
                            util::ui32 minor_version = 0;
                            util::ui32 patch_version = 0;
                            if (sscanf (entry.name.c_str (), fileTemplate.c_str (),
                                    &major_version, &minor_version, &patch_version) == 3) {
                                util::Version version (major_version, minor_version, patch_version);
                                if (latestVersion < version) {
                                    latestVersion = version;
                                }
                            }
                        }
                    }
                }
                return latestVersion != util::Version::Empty ?
                    latestVersion.ToString () : std::string ();
            }

            std::string Project::GetConfig (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example) {
                return MakePath (
                    GetRoot (organization, project, branch, version, example),
                    THEKOGANS_MAKE_XML);
            }

            std::string Project::GetRoot (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example) {
                std::list<std::string> components;
                components.push_back (_DEVELOPMENT_ROOT);
                components.push_back (organization);
                components.push_back (GetDirectoryFromName (project));
                if (!branch.empty ()) {
                    components.push_back (branch);
                }
                if (!version.empty ()) {
                    components.back () += DECORATIONS_SEPARATOR + version;
                }
                if (!example.empty ()) {
                    components.push_back (EXAMPLES_DIR);
                    components.push_back (GetDirectoryFromName (example));
                }
                return MakePath (components, false);
            }

            std::string Project::GetLibrary (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config,
                    const std::string &type) {
                std::string project_root =
                    GetRoot (organization, project, branch, version, example);
                std::list<std::string> components;
                components.push_back (project_root);
                components.push_back (LIB_DIR);
                std::string naming_convention =
                    thekogans_make::GetNamingConvention (project_root, THEKOGANS_MAKE_XML);
                if (naming_convention == NAMING_CONVENTION_HIERARCHICAL) {
                    components.push_back (_TOOLCHAIN_BRANCH);
                    components.push_back (config);
                    components.push_back (type);
                }
                std::string libraryName =
                    LIB_PREFIX + organization + ORGANIZATION_PROJECT_SEPARATOR + project;
                if (!example.empty ()) {
                    libraryName += PROJECT_EXAMPLE_SEPARATOR + example;
                }
                if (naming_convention == NAMING_CONVENTION_FLAT) {
                    libraryName +=
                        DECORATIONS_SEPARATOR + _TOOLCHAIN_TRIPLET +
                        DECORATIONS_SEPARATOR + config +
                        DECORATIONS_SEPARATOR + type;
                }
                libraryName += VERSION_SEPARATOR +
                    (!example.empty () ?
                        thekogans_make::GetVersion (project_root, THEKOGANS_MAKE_XML) :
                        version) + EXT_SEPARATOR + GetLinkLibrarySuffix (type);
                components.push_back (libraryName);
                return MakePath (components, true);
            }

            std::string Project::GetProgram (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config,
                    const std::string &type) {
                std::string project_root =
                    GetRoot (organization, project, branch, version, example);
                std::list<std::string> components;
                components.push_back (project_root);
                components.push_back (BIN_DIR);
                std::string naming_convention =
                    thekogans_make::GetNamingConvention (project_root, THEKOGANS_MAKE_XML);
                if (naming_convention == NAMING_CONVENTION_HIERARCHICAL) {
                    components.push_back (_TOOLCHAIN_BRANCH);
                    components.push_back (config);
                    components.push_back (type);
                }
                std::string programName =
                    organization + ORGANIZATION_PROJECT_SEPARATOR + project;
                if (!example.empty ()) {
                    programName += PROJECT_EXAMPLE_SEPARATOR + example;
                }
                if (naming_convention == NAMING_CONVENTION_FLAT) {
                    programName +=
                        DECORATIONS_SEPARATOR + _TOOLCHAIN_TRIPLET +
                        DECORATIONS_SEPARATOR + config +
                        DECORATIONS_SEPARATOR + type;
                }
                programName += VERSION_SEPARATOR +
                    (!example.empty () ?
                        thekogans_make::GetVersion (project_root, THEKOGANS_MAKE_XML) :
                        version) + _TOOLCHAIN_PROGRAM_SUFFIX;
                components.push_back (programName);
                return MakePath (components, true);
            }

            std::string Project::GetPlugin (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &example,
                    const std::string &config) {
                const std::string type = TYPE_SHARED;
                std::string project_root =
                    GetRoot (organization, project, branch, version, example);
                std::list<std::string> components;
                components.push_back (project_root);
                components.push_back (LIB_DIR);
                std::string naming_convention =
                    thekogans_make::GetNamingConvention (project_root, THEKOGANS_MAKE_XML);
                if (naming_convention == NAMING_CONVENTION_HIERARCHICAL) {
                    components.push_back (_TOOLCHAIN_BRANCH);
                    components.push_back (config);
                    components.push_back (type);
                }
                std::string pluginName =
                    organization + ORGANIZATION_PROJECT_SEPARATOR + project;
                if (!example.empty ()) {
                    pluginName += PROJECT_EXAMPLE_SEPARATOR + example;
                }
                if (naming_convention == NAMING_CONVENTION_FLAT) {
                    pluginName +=
                        DECORATIONS_SEPARATOR + _TOOLCHAIN_TRIPLET +
                        DECORATIONS_SEPARATOR + config +
                        DECORATIONS_SEPARATOR + type;
                }
                pluginName += VERSION_SEPARATOR +
                    (!example.empty () ?
                        thekogans_make::GetVersion (project_root, THEKOGANS_MAKE_XML) :
                        version) + EXT_SEPARATOR + _TOOLCHAIN_SHARED_LIBRARY_SUFFIX;
                components.push_back (pluginName);
                return MakePath (components, true);
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
