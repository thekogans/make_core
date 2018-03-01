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

#include "thekogans/util/Types.h"
#include "thekogans/util/Array.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Directory.h"
#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
    #include "thekogans/make/core/Sources.h"
#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Toolchain.h"

namespace thekogans {
    namespace make {
        namespace core {

            bool Toolchain::Find (
                    const std::string &organization,
                    const std::string &project,
                    std::string &version) {
                bool installed = IsInstalled (organization, project, version);
                if (!installed) {
                    std::vector<util::Version> versions;
                    versions.push_back (
                        util::Version (
                            GetLatestVersion (organization, project)));
                #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    versions.push_back (
                        util::Version (
                            ToolchainSources::Instance ().GetSourceToolchainLatestVersion (
                                organization, project)));
                    if (versions[0] < versions[1]) {
                        std::swap (versions[0], versions[1]);
                    }
                #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    for (std::size_t i = 0, count = versions.size (); !installed && i < count; ++i) {
                        if (versions[i] != util::Version::Empty) {
                            version = versions[i].ToString ();
                            installed = IsInstalled (organization, project, version);
                        #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                            if (!installed &&
                                    ToolchainSources::Instance ().IsSourceToolchain (
                                        organization, project, version)) {
                                ToolchainSources::Instance ().InstallSourceToolchain (
                                    organization, project, version);
                                installed = IsInstalled (organization, project, version);
                            }
                        #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                        }
                    }
                }
                return installed;
            }

            bool Toolchain::IsInstalled (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version) {
                return util::Path (ToSystemPath (GetConfig (organization, project, version))).Exists ();
            }

            void Toolchain::GetVersions (
                    const std::string &organization,
                    const std::string &project,
                    std::list<std::string> &versions) {
                util::Version latestVersion (0, 0, 0);
                std::string path = ToSystemPath (MakePath (_TOOLCHAIN_DIR, CONFIG_DIR));
                if (util::Path (path).Exists ()) {
                    util::Directory directory (path);
                    util::Directory::Entry entry;
                    std::string fileTemplate =
                        GetFileName (organization, project, std::string (), "%u.%u.%u", XML_EXT);
                    for (bool gotEntry = directory.GetFirstEntry (entry);
                            gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                        if (entry.type == util::Directory::Entry::File) {
                            util::ui32 major_version = 0;
                            util::ui32 minor_version = 0;
                            util::ui32 patch_version = 0;
                            if (sscanf (entry.name.c_str (), fileTemplate.c_str (),
                                    &major_version, &minor_version, &patch_version) == 3) {
                                versions.push_back (
                                    util::Version (
                                        major_version,
                                        minor_version,
                                        patch_version).ToString ());
                            }
                        }
                    }
                }
            }

            std::string Toolchain::GetLatestVersion (
                    const std::string &organization,
                    const std::string &project) {
                util::Version latestVersion (0, 0, 0);
                std::string path = ToSystemPath (MakePath (_TOOLCHAIN_DIR, CONFIG_DIR));
                if (util::Path (path).Exists ()) {
                    util::Directory directory (path);
                    util::Directory::Entry entry;
                    std::string fileTemplate =
                        GetFileName (organization, project, std::string (), "%u.%u.%u", XML_EXT);
                    for (bool gotEntry = directory.GetFirstEntry (entry);
                            gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                        if (entry.type == util::Directory::Entry::File) {
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
                return latestVersion.ToString ();
            }

            std::string Toolchain::GetConfig (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version) {
                std::list<std::string> components;
                components.push_back (_TOOLCHAIN_DIR);
                components.push_back (CONFIG_DIR);
                components.push_back (
                    GetFileName (organization, project, std::string (), version, XML_EXT));
                return MakePath (components, false);
            }

            std::string Toolchain::GetLibrary (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version,
                    const std::string &config,
                    const std::string &type) {
                std::list<std::string> components;
                components.push_back (_TOOLCHAIN_DIR);
                components.push_back (LIB_DIR);
                components.push_back (
                    GetFileName (organization, project, std::string (), version, std::string ()));
                std::string namingConvention =
                    thekogans_make::GetNamingConvention (
                        _TOOLCHAIN_DIR,
                        MakePath (
                            CONFIG_DIR,
                            GetFileName (organization, project, std::string (), version, XML_EXT)));
                if (namingConvention == NAMING_CONVENTION_HIERARCHICAL) {
                    components.push_back (config);
                    components.push_back (type);
                }
                std::string libraryName = LIB_PREFIX + organization + ORGANIZATION_PROJECT_SEPARATOR + project;
                if (namingConvention == NAMING_CONVENTION_FLAT) {
                    libraryName += DECORATIONS_SEPARATOR + _TOOLCHAIN_TRIPLET +
                        DECORATIONS_SEPARATOR + config + DECORATIONS_SEPARATOR + type;
                }
                libraryName += VERSION_SEPARATOR + version + EXT_SEPARATOR;
                if (type == TYPE_SHARED) {
                    libraryName += _TOOLCHAIN_SHARED_LIBRARY_SUFFIX;
                }
                else {
                    libraryName += _TOOLCHAIN_STATIC_LIBRARY_SUFFIX;
                }
                components.push_back (libraryName);
                return MakePath (components, true);
            }

            std::string Toolchain::GetProgram (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version) {
                std::list<std::string> components;
                components.push_back (_TOOLCHAIN_DIR);
                components.push_back (BIN_DIR);
                components.push_back (
                    GetFileName (organization, project, std::string (), version, std::string ()));
                components.push_back (
                    GetFileName (organization, project, std::string (), std::string (), _TOOLCHAIN_PROGRAM_SUFFIX));
                return MakePath (components, false);
            }

            void Toolchain::Cleanup (
                    const std::string &organization,
                    const std::string &project) {
                if (!organization.empty () && !project.empty ()) {
                    std::list<std::string> versions;
                    GetVersions (organization, project, versions);
                    if (versions.size () > 1) {
                        util::Version latestVersion (0, 0, 0);
                        for (std::list<std::string>::const_iterator
                                it = versions.begin (),
                                end = versions.end (); it != end; ++it) {
                            util::Version version (*it);
                            if (latestVersion < version) {
                                latestVersion = version;
                            }
                        }
                        std::set<std::string> visitedDependencies;
                        for (std::list<std::string>::const_iterator
                                it = versions.begin (),
                                end = versions.end (); it != end; ++it) {
                            if (util::Version (*it) != latestVersion) {
                                Uninstall (organization, project, *it, true, visitedDependencies);
                            }
                        }
                    }
                }
                else {
                    typedef std::pair<std::string, std::string> OrganizationProject;
                    typedef std::set<OrganizationProject> OrganizationProjectSet;
                    OrganizationProjectSet organizationProjectSet;
                    std::string path = ToSystemPath (MakePath (_TOOLCHAIN_DIR, CONFIG_DIR));
                    if (util::Path (path).Exists ()) {
                        util::Directory directory (path);
                        util::Directory::Entry entry;
                        std::string fileTemplate = GetFileName (
                            !organization.empty () ? organization : "%s",
                            !project.empty () ? project : "%s",
                            std::string (),
                            "%u.%u.%u",
                            XML_EXT);
                        for (bool gotEntry = directory.GetFirstEntry (entry);
                                gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                            if (entry.type == util::Directory::Entry::File) {
                                util::Array<char> organization (entry.name.size ());
                                util::Array<char> project (entry.name.size ());
                                util::ui32 major_version = 0;
                                util::ui32 minor_version = 0;
                                util::ui32 patch_version = 0;
                                if (sscanf (entry.name.c_str (), fileTemplate.c_str (),
                                        organization.array, project.array,
                                        &major_version, &minor_version, &patch_version) == 3) {
                                    organizationProjectSet.insert (
                                        OrganizationProject (organization.array, project.array));
                                }
                            }
                        }
                    }
                    for (OrganizationProjectSet::const_iterator
                            it = organizationProjectSet.begin (),
                            end = organizationProjectSet.end (); it != end; ++it) {
                        Toolchain::Cleanup ((*it).first, (*it).second);
                    }
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
