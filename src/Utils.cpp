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

#if !defined (TOOLCHAIN_OS_Windows)
    #include <fcntl.h>
#endif // !defined (TOOLCHAIN_OS_Windows)
#include <cstring>
#include <cstdio>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "thekogans/util/FixedArray.h"
#include "thekogans/util/Array.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/util/Version.h"
#include "thekogans/util/Plugins.h"
#include "thekogans/util/SHA2.h"
#include "thekogans/util/ChildProcess.h"
#if defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/util/WindowsUtils.h"
#endif // defined (TOOLCHAIN_OS_Windows)
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Function.h"
#if defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/make/core/CygwinMountTable.h"
#endif // defined (TOOLCHAIN_OS_Windows)
#include "thekogans/make/core/Manifest.h"
#include "thekogans/make/core/Generator.h"
#include "thekogans/make/core/Project.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/core/Utils.h"

namespace thekogans {
    namespace make {
        namespace core {

            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string BIN_DIR = "bin";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string LIB_DIR = "lib";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string SRC_DIR = "src";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string INCLUDE_DIR = "include";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string RESOURCES_DIR = "resources";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string EXAMPLES_DIR = "examples";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string DOC_DIR = "doc";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TESTS_DIR = "tests";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string BUILD_DIR = "build";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string CONFIG_DIR = "config";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string COMMON_DIR = "common";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string SOURCES_DIR = "sources";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string PROJECTS_DIR = "projects";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TOOLCHAIN_DIR = "toolchain";

            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string LIB_PREFIX = "lib";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TAR_GZ_EXT = "tar.gz";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string XML_EXT = "xml";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string PLUGINS_EXT = "plugins";
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string THEKOGANS_MANIFEST = "thekogans_manifest";

            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _DEVELOPMENT_ROOT =
                util::GetEnvironmentVariable ("DEVELOPMENT_ROOT");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ROOT =
                util::GetEnvironmentVariable ("TOOLCHAIN_ROOT");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_OS =
                util::GetEnvironmentVariable ("TOOLCHAIN_OS");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ARCH =
                util::GetEnvironmentVariable ("TOOLCHAIN_ARCH");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_COMPILER =
                util::GetEnvironmentVariable ("TOOLCHAIN_COMPILER");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_TRIPLET =
                util::GetEnvironmentVariable ("TOOLCHAIN_TRIPLET");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_ORGANIZATION =
                util::GetEnvironmentVariable ("TOOLCHAIN_DEFAULT_ORGANIZATION");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_PROJECT =
                util::GetEnvironmentVariable ("TOOLCHAIN_DEFAULT_PROJECT");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_BRANCH =
                util::GetEnvironmentVariable ("TOOLCHAIN_DEFAULT_BRANCH");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_VERSION =
                util::GetEnvironmentVariable ("TOOLCHAIN_DEFAULT_VERSION");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_NAMING_CONVENTION =
                util::GetEnvironmentVariable ("TOOLCHAIN_NAMING_CONVENTION");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_NAME =
                util::GetEnvironmentVariable ("TOOLCHAIN_NAME");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_SHELL =
                util::GetEnvironmentVariable ("TOOLCHAIN_SHELL");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ENDIAN =
                util::GetEnvironmentVariable ("TOOLCHAIN_ENDIAN");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DIR =
                util::GetEnvironmentVariable ("TOOLCHAIN_DIR");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_BRANCH =
                util::GetEnvironmentVariable ("TOOLCHAIN_BRANCH");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_PROGRAM_SUFFIX =
                util::GetEnvironmentVariable ("TOOLCHAIN_PROGRAM_SUFFIX");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_SHARED_LIBRARY_SUFFIX =
                util::GetEnvironmentVariable ("TOOLCHAIN_SHARED_LIBRARY_SUFFIX");
            _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_STATIC_LIBRARY_SUFFIX =
                util::GetEnvironmentVariable ("TOOLCHAIN_STATIC_LIBRARY_SUFFIX");

            EnvironmentSymbolTable::EnvironmentSymbolTable () {
                insert (value_type ("BIN_DIR", Value (BIN_DIR)));
                insert (value_type ("LIB_DIR", Value (LIB_DIR)));
                insert (value_type ("SRC_DIR", Value (SRC_DIR)));
                insert (value_type ("INCLUDE_DIR", Value (INCLUDE_DIR)));
                insert (value_type ("RESOURCES_DIR", Value (RESOURCES_DIR)));
                insert (value_type ("EXAMPLES_DIR", Value (EXAMPLES_DIR)));
                insert (value_type ("DOC_DIR", Value (DOC_DIR)));
                insert (value_type ("TESTS_DIR", Value (TESTS_DIR)));
                insert (value_type ("BUILD_DIR", Value (BUILD_DIR)));
                insert (value_type ("CONFIG_DIR", Value (CONFIG_DIR)));
                insert (value_type ("COMMON_DIR", Value (COMMON_DIR)));
                insert (value_type ("SOURCES_DIR", Value (SOURCES_DIR)));
                insert (value_type ("LIB_PREFIX", Value (LIB_PREFIX)));
                insert (value_type ("XML_EXT", Value (XML_EXT)));
                insert (value_type ("PLUGINS_EXT", Value (PLUGINS_EXT)));
                insert (value_type ("THEKOGANS_MANIFEST", Value (THEKOGANS_MANIFEST)));
                insert (value_type ("DEVELOPMENT_ROOT", Value (_DEVELOPMENT_ROOT)));
                insert (value_type ("TOOLCHAIN_ROOT", Value (_TOOLCHAIN_ROOT)));
                insert (value_type ("TOOLCHAIN_OS", Value (_TOOLCHAIN_OS)));
                insert (value_type ("TOOLCHAIN_ARCH", Value (_TOOLCHAIN_ARCH)));
                insert (value_type ("TOOLCHAIN_COMPILER", Value (_TOOLCHAIN_COMPILER)));
                insert (value_type ("TOOLCHAIN_TRIPLET", Value (_TOOLCHAIN_TRIPLET)));
                insert (value_type ("TOOLCHAIN_DEFAULT_ORGANIZATION", Value (_TOOLCHAIN_DEFAULT_ORGANIZATION)));
                insert (value_type ("TOOLCHAIN_DEFAULT_PROJECT", Value (_TOOLCHAIN_DEFAULT_PROJECT)));
                insert (value_type ("TOOLCHAIN_DEFAULT_BRANCH", Value (_TOOLCHAIN_DEFAULT_BRANCH)));
                insert (value_type ("TOOLCHAIN_DEFAULT_VERSION", Value (Value::TYPE_Version, _TOOLCHAIN_DEFAULT_VERSION)));
                insert (value_type ("TOOLCHAIN_NAMING_CONVENTION", Value (_TOOLCHAIN_NAMING_CONVENTION)));
                insert (value_type ("TOOLCHAIN_NAME", Value (_TOOLCHAIN_NAME)));
                insert (value_type ("TOOLCHAIN_SHELL", Value (_TOOLCHAIN_SHELL)));
                insert (value_type ("TOOLCHAIN_ENDIAN", Value (_TOOLCHAIN_ENDIAN)));
                insert (value_type ("TOOLCHAIN_DIR", Value (_TOOLCHAIN_DIR)));
                insert (value_type ("TOOLCHAIN_BRANCH", Value (_TOOLCHAIN_BRANCH)));
                insert (value_type ("TOOLCHAIN_PROGRAM_SUFFIX", Value (_TOOLCHAIN_PROGRAM_SUFFIX)));
                insert (value_type ("TOOLCHAIN_SHARED_LIBRARY_SUFFIX", Value (_TOOLCHAIN_SHARED_LIBRARY_SUFFIX)));
                insert (value_type ("TOOLCHAIN_STATIC_LIBRARY_SUFFIX", Value (_TOOLCHAIN_STATIC_LIBRARY_SUFFIX)));
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API ParseQuotedString (
                    const thekogans_make &config,
                    util::Buffer &buffer,
                    char quoteCh) {
                std::string quotedString;
                while (buffer.GetDataAvailableForReading () > 0) {
                    util::i8 ch;
                    buffer >> ch;
                    if (ch == quoteCh) {
                        return quotedString;
                    }
                    switch (ch) {
                        case '\\':
                            buffer >> ch;
                            if (IsEscapableCh (ch)) {
                                quotedString += ch;
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid escape sequence in: %s", buffer.data);
                            }
                            break;
                        case '$':
                            quotedString += Function::ParseAndExec (config, buffer).ToString ();
                            break;
                        default:
                            quotedString += ch;
                            break;
                    }
                }
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Missing closing '%c' in: %s", quoteCh, buffer.data);
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetLinkLibrarySuffix (
                    const std::string &type) {
            #if defined (TOOLCHAIN_OS_Windows)
                return _TOOLCHAIN_STATIC_LIBRARY_SUFFIX;
            #else // defined (TOOLCHAIN_OS_Windows)
                return type == TYPE_SHARED ?
                    _TOOLCHAIN_SHARED_LIBRARY_SUFFIX :
                    _TOOLCHAIN_STATIC_LIBRARY_SUFFIX;
            #endif // defined (TOOLCHAIN_OS_Windows)
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetFileName (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &ext) {
                std::string fileName = organization + ORGANIZATION_PROJECT_SEPARATOR + project;
                if (!branch.empty ()) {
                    fileName += DECORATIONS_SEPARATOR + GetNameFromDirectory (branch);
                }
                if (!version.empty ()) {
                    fileName += DECORATIONS_SEPARATOR + version;
                }
                if (!ext.empty ()) {
                    if (ext[0] != EXT_SEPARATOR_CHAR) {
                        fileName += EXT_SEPARATOR;
                    }
                    fileName += ext;
                }
                return fileName;
            }

            namespace {
                std::string::size_type SkipVersion (
                        const std::string &fileName,
                        std::string::size_type start) {
                    std::string::size_type end =
                        fileName.find_first_of (VERSION_SEPARATOR, start);
                    if (end != std::string::npos) {
                        start = end + 1;
                        end = fileName.find_first_of (VERSION_SEPARATOR, start);
                        if (end != std::string::npos) {
                            start = end + 1;
                            end = fileName.find_first_of (EXT_SEPARATOR, start);
                        }
                    }
                    return end;
                }
            }

            // A valid file name will have one of the following forms:
            //
            // 1 - organization_ptoject
            // 2 - organization_ptoject-version
            // 3 - organization_ptoject-branch-version
            // 4 - organization_ptoject.ext
            // 5 - organization_ptoject-version.ext
            // 6 - organization_ptoject-branch-version.ext
            _LIB_THEKOGANS_MAKE_CORE_DECL util::ui32 _LIB_THEKOGANS_MAKE_CORE_API ParseFileName (
                    const std::string &fileName,
                    std::string &organization,
                    std::string &project,
                    std::string &branch,
                    std::string &version,
                    std::string &ext) {
                util::ui32 form = 0;
                std::string::size_type start = 0;
                std::string::size_type end =
                    fileName.find_first_of (ORGANIZATION_PROJECT_SEPARATOR, start);
                if (end != std::string::npos) {
                    organization = fileName.substr (start, end - start);
                    start = end + 1;
                    end = fileName.find_first_of (DECORATIONS_SEPARATOR, start);
                    if (end != std::string::npos) {
                        project = fileName.substr (start, end - start);
                        start = end + 1;
                        end = fileName.find_first_of (DECORATIONS_SEPARATOR, start);
                        if (end != std::string::npos) {
                            branch = fileName.substr (start, end - start);
                            start = end + 1;
                            end = SkipVersion (fileName, start);
                            if (end != std::string::npos) {
                                // 6
                                version = fileName.substr (start, end - start);
                                start = end + 1;
                                ext = fileName.substr (start, std::string::npos);
                                form = 6;
                            }
                            else {
                                // 3
                                version = fileName.substr (start, std::string::npos);
                                form = 3;
                            }
                        }
                        else {
                            end = SkipVersion (fileName, start);
                            if (end != std::string::npos) {
                                // 5
                                version = fileName.substr (start, end - start);
                                start = end + 1;
                                ext = fileName.substr (start, std::string::npos);
                                form = 5;
                            }
                            else {
                                // 2
                                version = fileName.substr (start, std::string::npos);
                                form = 2;
                            }
                        }
                    }
                    else {
                        end = fileName.find_first_of (EXT_SEPARATOR, start);
                        if (end != std::string::npos) {
                            // 4
                            project = fileName.substr (start, end - start);
                            start = end + 1;
                            ext = fileName.substr (start, std::string::npos);
                            form = 4;
                        }
                        else {
                            // 1
                            project = fileName.substr (start, std::string::npos);
                            form = 1;
                        }
                    }
                }
                return form;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetBuildDirectory (
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                std::list<std::string> components;
                components.push_back (BUILD_DIR);
                components.push_back (_TOOLCHAIN_BRANCH);
                components.push_back (generator);
                components.push_back (config);
                components.push_back (type);
                return MakePath (components, false);
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetBuildRoot (
                    const std::string &project_root,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                return MakePath (project_root, GetBuildDirectory (generator, config, type));
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API BuildRootExists (
                    const std::string &project_root,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                return util::Path (
                    ToSystemPath (
                        GetBuildRoot (project_root, generator, config, type))).Exists ();
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CreateBuildRoot (
                    const std::string &project_root,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                std::string buildRoot =
                    ToSystemPath (GetBuildRoot (project_root, generator, config, type));
                if (!util::Path (buildRoot).Exists ()) {
                    util::Directory::Create (buildRoot);
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDirectoryFromName (
                    const std::string &name) {
                std::string directory = name;
                std::replace (directory.begin (), directory.end (), '_', PATH_SEPARATOR_CHAR);
                return directory;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetNameFromDirectory (
                    const std::string &directory) {
                std::string name = directory;
                std::replace (name.begin (), name.end (), PATH_SEPARATOR_CHAR, '_');
                return name;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDefaultBranch (
                    const std::string &organization,
                    const std::string &project) {
                if (!organization.empty () && !project.empty ()) {
                    std::string ORGANIZATION_PROJECT_DEFAULT_BRANCH =
                        util::StringToUpper (organization.c_str ()) + ORGANIZATION_PROJECT_SEPARATOR +
                        util::StringToUpper (project.c_str ()) + "_DEFAULT_BRANCH";
                    std::string branch =
                        util::GetEnvironmentVariable (ORGANIZATION_PROJECT_DEFAULT_BRANCH.c_str ());
                    if (!branch.empty ()) {
                        return branch;
                    }
                }
                if (!organization.empty ()) {
                    std::string ORGANIZATION_DEFAULT_BRANCH =
                        util::StringToUpper (organization.c_str ()) + "_DEFAULT_BRANCH";
                    std::string branch =
                        util::GetEnvironmentVariable (ORGANIZATION_DEFAULT_BRANCH.c_str ());
                    if (!branch.empty ()) {
                        return branch;
                    }
                }
                return _TOOLCHAIN_DEFAULT_BRANCH;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDefaultVersion (
                    const std::string &organization,
                    const std::string &project) {
                if (!organization.empty () && !project.empty ()) {
                    std::string ORGANIZATION_PROJECT_DEFAULT_VERSION =
                        util::StringToUpper (organization.c_str ()) + ORGANIZATION_PROJECT_SEPARATOR +
                        util::StringToUpper (project.c_str ()) + "_DEFAULT_VERSION";
                    std::string version =
                        util::GetEnvironmentVariable (ORGANIZATION_PROJECT_DEFAULT_VERSION.c_str ());
                    if (!version.empty ()) {
                        return version;
                    }
                }
                if (!organization.empty ()) {
                    std::string ORGANIZATION_DEFAULT_VERSION =
                        util::StringToUpper (organization.c_str ()) + "_DEFAULT_VERSION";
                    std::string version =
                        util::GetEnvironmentVariable (ORGANIZATION_DEFAULT_VERSION.c_str ());
                    if (!version.empty ()) {
                        return version;
                    }
                }
                return _TOOLCHAIN_DEFAULT_VERSION;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetConfigKey (
                    const std::string &project_root,
                    const std::string &config_file,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                std::string configKey = MakePath (project_root, config_file);
                if (!generator.empty ()) {
                    configKey += DECORATIONS_SEPARATOR + generator;
                }
                if (!config.empty () && !type.empty ()) {
                    configKey += DECORATIONS_SEPARATOR + config + DECORATIONS_SEPARATOR + type;
                }
                return configKey;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API MakePath (
                    const std::string &path1,
                    const std::string &path2) {
                std::string path;
                if (!path1.empty () && !path2.empty ()) {
                    path = path1;
                    if (path.back () != PATH_SEPARATOR_CHAR) {
                        path += PATH_SEPARATOR;
                    }
                    path += path2[0] == PATH_SEPARATOR_CHAR ? &path2[1] : &path2[0];
                }
                else if (!path1.empty ()) {
                    path = path1;
                }
                else if (!path2.empty ()) {
                    path = path2;
                }
                return path;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API MakePath (
                    const std::list<std::string> &components,
                    bool absolute) {
                std::string path;
                if (absolute) {
                    path = PATH_SEPARATOR;
                }
                for (std::list<std::string>::const_iterator
                        it = components.begin (),
                        end = components.end (); it != end; ++it) {
                    path = MakePath (path, *it);
                }
                return path;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetFileHash (
                    const std::string &path) {
                util::Hash::Digest digest;
                util::SHA2 hasher;
                hasher.FromFile (ToSystemPath (path), util::SHA2::DIGEST_SIZE_256, digest);
                return util::Hash::DigestToString (digest);
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API CopyFile (
                    const std::string &from,
                    const std::string &to) {
                std::string fromPath = ToSystemPath (from);
                std::string toPath = ToSystemPath (to);
                if (!util::Path (toPath).Exists () ||
                        util::Directory::Entry (toPath).lastModifiedDate <
                        util::Directory::Entry (fromPath).lastModifiedDate) {
                    std::cout << "Copying " << from << " -> " << to << std::endl;
                    std::cout.flush ();
                    util::Directory::Create (util::Path (toPath).GetDirectory ());
                    util::ReadOnlyFile fromFile (util::HostEndian, fromPath);
                #if defined (TOOLCHAIN_OS_Windows)
                    util::File toFile (
                        util::HostEndian,
                        toPath,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        CREATE_ALWAYS);
                #else // defined (TOOLCHAIN_OS_Windows)
                    util::File toFile (
                        util::HostEndian,
                        toPath,
                        O_RDWR | O_CREAT | O_TRUNC,
                        util::Directory::Entry (fromPath).mode);
                #endif // defined (TOOLCHAIN_OS_Windows)
                    util::FixedArray<util::ui8, 4096> buffer;
                    for (util::ui32 count = fromFile.Read (buffer.array, 4096);
                            count != 0;
                            count = fromFile.Read (buffer.array, 4096)) {
                        toFile.Write (buffer.array, count);
                    }
                    return true;
                }
                return false;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API DeleteFile (const std::string &file) {
                util::Path path (ToSystemPath (file));
                if (path.Exists ()) {
                    std::cout << "Deleting " << file << std::endl;
                    std::cout.flush ();
                    path.Delete ();
                    return true;
                }
                return false;
            }

            namespace {
                void DeleteFolders (
                        const std::string &path,
                        const std::string &folderName) {
                    util::Directory directory (ToSystemPath (path));
                    util::Directory::Entry entry;
                    for (bool gotEntry = directory.GetFirstEntry (entry);
                            gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                        if (entry.type == util::Directory::Entry::Folder &&
                                !util::IsDotOrDotDot (entry.name.c_str ())) {
                            if (entry.name == folderName) {
                                std::string folder = MakePath (path, entry.name);
                                std::cout << "Deleting " << folder << std::endl;
                                std::cout.flush ();
                                util::Path (ToSystemPath (folder)).Delete ();
                            }
                            else {
                                DeleteFolders (MakePath (path, entry.name), folderName);
                            }
                        }
                    }
                }

                void UninstallDependencies (
                        const std::string &project_root,
                        const std::string &config_file,
                        const std::string &config_,
                        const std::string &type,
                        std::set<std::string> &visitedDependencies) {
                    const thekogans_make &config = thekogans_make::GetConfig (
                        project_root,
                        config_file,
                        MAKE,
                        config_,
                        type);
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = config.dependencies.begin (),
                            end = config.dependencies.end (); it != end; ++it) {
                        if ((*it)->GetProjectRoot () == _TOOLCHAIN_DIR) {
                            const core::thekogans_make &dependency = thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ());
                            Uninstall (
                                dependency.organization,
                                dependency.project,
                                dependency.GetVersion (),
                                true,
                                visitedDependencies);
                        }
                    }
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API Uninstall (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version,
                    bool dependencies,
                    std::set<std::string> &visitedDependencies) {
                std::string project_root = _TOOLCHAIN_DIR;
                std::string config_file =
                    MakePath (
                        CONFIG_DIR,
                        GetFileName (organization, project, std::string (), version, XML_EXT));
                if (visitedDependencies.find (config_file) == visitedDependencies.end ()) {
                    visitedDependencies.insert (config_file);
                    if (dependencies) {
                        std::string install_config =
                            thekogans_make::GetBuildConfig (project_root, config_file);
                        std::string install_type =
                            thekogans_make::GetBuildType (project_root, config_file);
                        if (!install_config.empty () && !install_type.empty ()) {
                            UninstallDependencies (
                                project_root,
                                config_file,
                                install_config,
                                install_type,
                                visitedDependencies);
                        }
                        else if (!install_config.empty ()) {
                            UninstallDependencies (
                                project_root,
                                config_file,
                                install_config,
                                TYPE_SHARED,
                                visitedDependencies);
                            UninstallDependencies (
                                project_root,
                                config_file,
                                install_config,
                                TYPE_STATIC,
                                visitedDependencies);
                        }
                        else if (!install_type.empty ()) {
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_DEBUG,
                                install_type,
                                visitedDependencies);
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_RELEASE,
                                install_type,
                                visitedDependencies);
                        }
                        else {
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_DEBUG,
                                TYPE_SHARED,
                                visitedDependencies);
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_DEBUG,
                                TYPE_STATIC,
                                visitedDependencies);
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_RELEASE,
                                TYPE_SHARED,
                                visitedDependencies);
                            UninstallDependencies (
                                project_root,
                                config_file,
                                CONFIG_RELEASE,
                                TYPE_STATIC,
                                visitedDependencies);
                        }
                    }
                    std::string configFilePath = MakePath (project_root, config_file);
                    std::cout << "Uninstalling " << configFilePath << std::endl;
                    std::cout.flush ();
                    DeleteFolders (
                        project_root,
                        GetFileName (organization, project, std::string (), version, std::string ()));
                    DeleteFile (configFilePath);
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallLibrary (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version,
                    bool dependencies) {
                if (!version.empty ()) {
                    std::set<std::string> visitedDependencies;
                    Uninstall (organization, project, version, dependencies, visitedDependencies);
                }
                else {
                    std::set<std::string> visitedDependencies;
                    std::list<std::string> versions;
                    Toolchain::GetVersions (organization, project, versions);
                    for (std::list<std::string>::const_iterator
                            it = versions.begin (),
                            end = versions.end (); it != end; ++it) {
                        Uninstall (organization, project, *it, dependencies, visitedDependencies);
                    }
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallProgram (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version,
                    bool dependencies) {
                if (!version.empty ()) {
                    std::set<std::string> visitedDependencies;
                    Uninstall (organization, project, version, dependencies, visitedDependencies);
                }
                else {
                    std::set<std::string> visitedDependencies;
                    std::list<std::string> versions;
                    Toolchain::GetVersions (organization, project, versions);
                    for (std::list<std::string>::const_iterator
                            it = versions.begin (),
                            end = versions.end (); it != end; ++it) {
                        Uninstall (organization, project, *it, dependencies, visitedDependencies);
                    }
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallPlugin (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &version) {
                // FIXME: implement
                assert (0);
            }

            namespace {
                bool operator < (
                        const util::Plugins::Plugin &plugin1,
                        const util::Plugins::Plugin &plugin2) {
                    return plugin1.path < plugin2.path;
                }

                bool operator != (
                        const util::Plugins::Plugin &plugin1,
                        const util::Plugins::Plugin &plugin2) {
                    return util::Version (plugin1.version) != util::Version (plugin2.version) ||
                        plugin1.SHA2_256 != plugin2.SHA2_256 ||
                        plugin1.dependencies != plugin2.dependencies;
                }

                typedef std::list<util::Plugins::Plugin::Ptr> PluginList;
                typedef std::pair<util::Plugins::Plugin::Ptr, util::Plugins::Plugin::Ptr> ModifiedPlugin;
                typedef std::list<ModifiedPlugin> ModifiedPluginList;

                void DiffPluginMaps (
                        const util::Plugins::PluginMap &fromPlugins,
                        const util::Plugins::PluginMap &toPlugins,
                        PluginList &added,
                        PluginList &deleted,
                        ModifiedPluginList &modified) {
                    util::Plugins::PluginMap::const_iterator fromBegin = fromPlugins.begin ();
                    util::Plugins::PluginMap::const_iterator fromEnd = fromPlugins.end ();
                    util::Plugins::PluginMap::const_iterator toBegin = toPlugins.begin ();
                    util::Plugins::PluginMap::const_iterator toEnd = toPlugins.end ();
                    while (fromBegin != fromEnd && toBegin != toEnd) {
                        if (*fromBegin->second < *toBegin->second) {
                            deleted.push_back ((fromBegin++)->second);
                        }
                        else if (*toBegin->second < *fromBegin->second) {
                            added.push_back ((toBegin++)->second);
                        }
                        else if (*fromBegin->second != *toBegin->second) {
                            modified.push_back (ModifiedPlugin ((fromBegin++)->second, (toBegin++)->second));
                        }
                        else {
                            ++fromBegin;
                            ++toBegin;
                        }
                    }
                    assert (fromBegin == fromEnd || toBegin == toEnd);
                    while (fromBegin != fromEnd) {
                        deleted.push_back ((fromBegin++)->second);
                    }
                    while (toBegin != toEnd) {
                        added.push_back ((toBegin++)->second);
                    }
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CopyDependencies (
                    const std::string &project_root,
                    const std::string &config_,
                    const std::string &type,
                    const std::string &destination) {
                const thekogans_make &config = thekogans_make::GetConfig (
                    project_root,
                    THEKOGANS_MAKE_XML,
                    MAKE,
                    config_,
                    type);
                std::string goalFileName = config.GetGoalFileName ();
                std::string toDirectory = destination.empty () ?
                    config.GetProjectBinDirectory () : destination;
                Manifest manifest (
                    ToSystemPath (
                        MakePath (toDirectory, THEKOGANS_MANIFEST + EXT_SEPARATOR + XML_EXT)));
                std::set<std::string> sharedLibraries;
                config.GetSharedLibraries (sharedLibraries);
                for (std::set<std::string>::const_iterator
                        it = sharedLibraries.begin (),
                        end = sharedLibraries.end (); it != end; ++it) {
                    std::string fromDirectory = util::Path (*it).GetDirectory ();
                    std::string fromFileName = util::Path (*it).GetFullFileName ();
                    std::string sharedLibrary = MakePath (toDirectory, fromFileName);
                    CopyFile (*it, sharedLibrary);
                    manifest.AddFile (fromFileName, goalFileName);
                    std::string fromPluginsPath = ToSystemPath (*it + EXT_SEPARATOR + PLUGINS_EXT);
                    std::string toPluginsPath = ToSystemPath (sharedLibrary + EXT_SEPARATOR + PLUGINS_EXT);
                    if (util::Path (fromPluginsPath).Exists ()) {
                        if (util::Path (toPluginsPath).Exists ()) {
                            // Merge fromPluginsPath and toPluginsPath.
                            std::cout << "Merging " << *it + EXT_SEPARATOR + PLUGINS_EXT << " and " <<
                                sharedLibrary + EXT_SEPARATOR + PLUGINS_EXT << std::endl;
                            std::cout.flush ();
                            if (util::Directory::Entry (toPluginsPath).lastModifiedDate <
                                    util::Directory::Entry (fromPluginsPath).lastModifiedDate) {
                                util::Plugins fromPlugins (fromPluginsPath);
                                util::Plugins toPlugins (toPluginsPath);
                                PluginList added;
                                PluginList deleted;
                                ModifiedPluginList modified;
                                DiffPluginMaps (
                                    fromPlugins.GetPluginMap (),
                                    toPlugins.GetPluginMap (),
                                    added,
                                    deleted,
                                    modified);
                                for (PluginList::const_iterator
                                        jt = added.begin (),
                                        end = added.end (); jt != end; ++jt) {
                                    CopyFile (
                                        MakePath (fromDirectory, (*jt)->path),
                                        MakePath (toDirectory, (*jt)->path));
                                    manifest.AddFile ((*jt)->path, goalFileName);
                                    for (util::Plugins::Plugin::Dependencies::const_iterator
                                            kt = (*jt)->dependencies.begin (),
                                            end = (*jt)->dependencies.end (); kt != end; ++kt) {
                                        CopyFile (
                                            MakePath (fromDirectory, *kt),
                                            MakePath (toDirectory, *kt));
                                        manifest.AddFile (*kt, (*jt)->path);
                                    }
                                    toPlugins.AddPlugin (
                                        (*jt)->path,
                                        (*jt)->version,
                                        (*jt)->SHA2_256,
                                        (*jt)->dependencies);
                                }
                                for (PluginList::const_iterator
                                        jt = deleted.begin (),
                                        end = deleted.end (); jt != end; ++jt) {
                                    if (manifest.DeleteFile ((*jt)->path, fromFileName)) {
                                        DeleteFile (MakePath (toDirectory, (*jt)->path));
                                        for (util::Plugins::Plugin::Dependencies::const_iterator
                                                kt = (*jt)->dependencies.begin (),
                                                end = (*jt)->dependencies.end (); kt != end; ++kt) {
                                            if (manifest.DeleteFile (*kt, (*jt)->path)) {
                                                DeleteFile (MakePath (toDirectory, *kt));
                                            }
                                        }
                                    }
                                    toPlugins.DeletePlugin ((*jt)->path);
                                }
                                for (ModifiedPluginList::const_iterator
                                        jt = modified.begin (),
                                        end = modified.end (); jt != end; ++jt) {
                                    CopyFile (
                                        MakePath (fromDirectory, (*jt).first->path),
                                        MakePath (toDirectory, (*jt).first->path));
                                    for (util::Plugins::Plugin::Dependencies::const_iterator
                                            kt = (*jt).first->dependencies.begin (),
                                            end = (*jt).first->dependencies.end (); kt != end; ++kt) {
                                        if (manifest.DeleteFile (*kt, (*jt).first->path)) {
                                            DeleteFile (MakePath (toDirectory, *kt));
                                        }
                                    }
                                    for (util::Plugins::Plugin::Dependencies::const_iterator
                                            kt = (*jt).second->dependencies.begin (),
                                            end = (*jt).second->dependencies.end (); kt != end; ++kt) {
                                        CopyFile (
                                            MakePath (fromDirectory, *kt),
                                            MakePath (toDirectory, *kt));
                                        manifest.AddFile (*kt, (*jt).second->path);
                                    }
                                }
                                toPlugins.Save ();
                            }
                        }
                        else {
                            // Copy fromPluginsPath to toPluginsPath.
                            util::Plugins plugins (fromPluginsPath);
                            const util::Plugins::PluginMap &pluginMap = plugins.GetPluginMap ();
                            for (util::Plugins::PluginMap::const_iterator
                                    jt = pluginMap.begin (),
                                    end = pluginMap.end (); jt != end; ++jt) {
                                CopyFile (
                                    MakePath (fromDirectory, jt->first),
                                    MakePath (toDirectory, jt->first));
                                manifest.AddFile (jt->first, goalFileName);
                                for (util::Plugins::Plugin::Dependencies::const_iterator
                                        kt = jt->second->dependencies.begin (),
                                        end = jt->second->dependencies.end (); kt != end; ++kt) {
                                    CopyFile (
                                        MakePath (fromDirectory, *kt),
                                        MakePath (toDirectory, *kt));
                                    manifest.AddFile (*kt, jt->first);
                                }
                            }
                            CopyFile (fromPluginsPath, toPluginsPath);
                        }
                    }
                    else if (util::Path (toPluginsPath).Exists ()) {
                        // Delete toPluginsPath.
                        util::Plugins plugins (toPluginsPath);
                        const util::Plugins::PluginMap &pluginMap = plugins.GetPluginMap ();
                        for (util::Plugins::PluginMap::const_iterator
                                jt = pluginMap.begin (),
                                end = pluginMap.end (); jt != end; ++jt) {
                            if (manifest.DeleteFile (jt->first, fromFileName)) {
                                DeleteFile (MakePath (toDirectory, jt->first));
                                for (util::Plugins::Plugin::Dependencies::const_iterator
                                        kt = jt->second->dependencies.begin (),
                                        end = jt->second->dependencies.end (); kt != end; ++kt) {
                                    if (manifest.DeleteFile (*kt, jt->first)) {
                                        DeleteFile (MakePath (toDirectory, *kt));
                                    }
                                }
                            }
                        }
                        DeleteFile (sharedLibrary + EXT_SEPARATOR + PLUGINS_EXT);
                    }
                }
                // FIXME: Collect installed resources and copy them too.
                // Make sure the manifest reflects which library depends on them.
                manifest.Save ();
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CopyPlugin (
                    const std::string &project_root,
                    const std::string &config) {
                const thekogans_make &plugin_config =
                    thekogans_make::GetConfig (
                        project_root,
                        THEKOGANS_MAKE_XML,
                        MAKE,
                        config,
                        TYPE_SHARED);
                if (plugin_config.project_type == PROJECT_TYPE_PLUGIN) {
                    std::string fromPlugin = plugin_config.GetProjectGoal ();
                    for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                            it = plugin_config.plugin_hosts.begin (),
                            end = plugin_config.plugin_hosts.end (); it != end; ++it) {
                        if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            const thekogans_make &host_config =
                                thekogans_make::GetConfig (
                                    (*it)->GetProjectRoot (),
                                    (*it)->GetConfigFile (),
                                    (*it)->GetGenerator (),
                                    (*it)->GetConfig (),
                                    (*it)->GetType ());
                            std::string toDirectory = host_config.project_type == PROJECT_TYPE_PROGRAM ?
                                host_config.GetProjectBinDirectory () :
                                host_config.GetProjectLibDirectory ();
                            std::string pluginFileName = util::Path (fromPlugin).GetFullFileName ();
                            std::string toPlugin = MakePath (toDirectory, pluginFileName);
                            util::Plugins plugins (ToSystemPath (host_config.GetProjectGoal () + EXT_SEPARATOR + PLUGINS_EXT));
                            {
                                CopyFile (fromPlugin, toPlugin);
                                Manifest manifest (
                                    ToSystemPath (
                                        MakePath (toDirectory, THEKOGANS_MANIFEST + EXT_SEPARATOR + XML_EXT)));
                                manifest.AddFile (pluginFileName, host_config.GetGoalFileName ());
                                util::Plugins::Plugin::Ptr plugin = plugins.GetPlugin (pluginFileName);
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
                                CopyDependencies (project_root, config, TYPE_SHARED, toDirectory);
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
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "%s is not a plugin project.",
                        MakePath (project_root, THEKOGANS_MAKE_XML).c_str ());
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetGeneratorList (
                    const std::string &separator) {
                std::string generatorList;
                {
                    std::list<std::string> generators;
                    Generator::GetGenerators (generators);
                    if (!generators.empty ()) {
                        std::list<std::string>::const_iterator it = generators.begin ();
                        std::list<std::string>::const_iterator end = generators.end ();
                        generatorList = *it++;
                        while (it != end) {
                            generatorList += separator + *it++;
                        }
                    }
                    else {
                        generatorList = "No generators defined.";
                    }
                }
                return generatorList;
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CreateBuildSystem (
                    const std::string &project_root,
                    const std::string &generator_,
                    const std::string &config,
                    const std::string &type,
                    bool generateDependencies) {
                Generator::UniquePtr generator = Generator::Get (generator_);
                if (generator.get () != 0) {
                    if (config == CONFIG_DEBUG || config == CONFIG_RELEASE) {
                        if (type == TYPE_SHARED || type == TYPE_STATIC) {
                            generator->Generate (project_root, config, type, generateDependencies);
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Invlalid type: %s (" TYPE_STATIC " | " TYPE_SHARED ")",
                                type.c_str ());
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Invlalid config: %s (" CONFIG_DEBUG " | " CONFIG_RELEASE ")",
                            config.c_str ());
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invlalid generator: %s (%s)",
                        generator_.c_str (),
                        GetGeneratorList (", ").c_str ());
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API DeleteBuildSystem (
                    const std::string &project_root,
                    const std::string &generator_,
                    const std::string &config,
                    const std::string &type,
                    bool deleteDependencies) {
                Generator::UniquePtr generator = Generator::Get (generator_);
                if (generator.get () != 0) {
                    if (config == CONFIG_DEBUG || config == CONFIG_RELEASE) {
                        if (type == TYPE_SHARED || type == TYPE_STATIC) {
                            generator->Delete (project_root, config, type, deleteDependencies);
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Invlalid type: %s (" TYPE_STATIC " | " TYPE_SHARED ")",
                                type.c_str ());
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Invlalid config: %s (" CONFIG_DEBUG " | " CONFIG_RELEASE ")",
                            config.c_str ());
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invlalid generator: %s (%s)",
                        generator_.c_str (),
                        GetGeneratorList (", ").c_str ());
                }
            }

            namespace {
                void Execgnu_make (
                        const std::string &build_root,
                        const std::string &gnu_make,
                        const std::list<std::string> &arguments) {
                    util::ChildProcess gnu_makeProcess (gnu_make);
                    gnu_makeProcess.AddArgument ("-f");
                    gnu_makeProcess.AddArgument (MakePath (build_root, MAKEFILE));
                    for (std::list<std::string>::const_iterator
                            it = arguments.begin (),
                            end = arguments.end (); it != end; ++it) {
                        gnu_makeProcess.AddArgument (*it);
                    }
                    util::ChildProcess::ChildStatus childStatus = gnu_makeProcess.Exec ();
                    if (childStatus == util::ChildProcess::Failed ||
                            gnu_makeProcess.GetReturnCode () != 0) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to execute '%s'.",
                            gnu_makeProcess.BuildCommandLine ().c_str ());
                    }
                }

                void BuildProjectHelper (
                        const std::string &project_root,
                        const std::string &config_,
                        const std::string &type,
                        const std::string &gnu_make,
                        const std::list<std::string> &arguments,
                        const std::string &target,
                        std::set<std::string> &builtProjects) {
                    if (builtProjects.find (project_root) == builtProjects.end ()) {
                        builtProjects.insert (project_root);
                        const thekogans_make &config = thekogans_make::GetConfig (
                            project_root,
                            THEKOGANS_MAKE_XML,
                            MAKE,
                            config_,
                            type);
                        if (config.project_type == PROJECT_TYPE_PLUGIN) {
                            for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                                    it = config.plugin_hosts.begin (),
                                    end = config.plugin_hosts.end (); it != end; ++it) {
                                if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                                    BuildProjectHelper (
                                        (*it)->GetProjectRoot (),
                                        (*it)->GetConfig (),
                                        (*it)->GetType (),
                                        gnu_make,
                                        arguments,
                                        target,
                                        builtProjects);
                                    if (target == TARGET_ALL || target == TARGET_TESTS) {
                                        const core::thekogans_make &plugin_host = thekogans_make::GetConfig (
                                            (*it)->GetProjectRoot (),
                                            (*it)->GetConfigFile (),
                                            (*it)->GetGenerator (),
                                            (*it)->GetConfig (),
                                            (*it)->GetType ());
                                        if (plugin_host.project_type == PROJECT_TYPE_PROGRAM) {
                                            CopyDependencies (
                                                (*it)->GetProjectRoot (),
                                                (*it)->GetConfig (),
                                                (*it)->GetType ());
                                        }
                                        else if (plugin_host.project_type == PROJECT_TYPE_PLUGIN) {
                                            CopyPlugin (
                                                (*it)->GetProjectRoot (),
                                                (*it)->GetConfig ());
                                        }
                                    }
                                }
                            }
                        }
                        for (std::list<thekogans_make::Dependency::Ptr>::const_iterator
                                it = config.dependencies.begin (),
                                end = config.dependencies.end (); it != end; ++it) {
                            if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                                BuildProjectHelper (
                                    (*it)->GetProjectRoot (),
                                    (*it)->GetConfig (),
                                    (*it)->GetType (),
                                    gnu_make,
                                    arguments,
                                    target,
                                    builtProjects);
                            }
                        }
                        std::string build_root = GetBuildRoot (project_root, "make", config_, type);
                        Execgnu_make (build_root, gnu_make, arguments);
                        if (target == TARGET_CLEAN) {
                            DeleteFile (MakePath (build_root, MAKEFILE));
                        }
                    }
                }
            }

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API BuildProject (
                    const std::string &project_root,
                    const std::string &config_,
                    const std::string &type,
                    const std::string &mode,
                    bool hide_commands,
                    bool parallel_build,
                    const std::string &target) {
                CreateBuildSystem (project_root, "make", config_, type, true);
                std::string gnu_make =
                    ToSystemPath (
                        Toolchain::GetProgram ("gnu", "make",
                            Toolchain::GetLatestVersion ("gnu", "make")));
                std::list<std::string> arguments;
                if (hide_commands) {
                    arguments.push_back ("--quiet");
                }
                if (parallel_build) {
                    arguments.push_back ("--output-sync");
                    arguments.push_back ("-j");
                }
                arguments.push_back ("mode=" + mode);
                arguments.push_back ("hide_commands=" + std::string (hide_commands ? VALUE_YES : VALUE_NO));
                arguments.push_back (target);
                if (target != TARGET_CLEAN_SELF) {
                    std::set<std::string> builtProjects;
                    BuildProjectHelper (
                        project_root,
                        config_,
                        type,
                        gnu_make,
                        arguments,
                        target,
                        builtProjects);
                    if (target == TARGET_ALL || target == TARGET_TESTS) {
                        const thekogans_make &config = thekogans_make::GetConfig (
                            project_root,
                            THEKOGANS_MAKE_XML,
                            MAKE,
                            config_,
                            type);
                        if (config.project_type == PROJECT_TYPE_PROGRAM) {
                            CopyDependencies (project_root, config_, type);
                        }
                        else if (config.project_type == PROJECT_TYPE_PLUGIN) {
                            CopyPlugin (project_root, config_);
                        }
                    }
                }
                else {
                    std::string build_root = GetBuildRoot (project_root, "make", config_, type);
                    Execgnu_make (build_root, gnu_make, arguments);
                    DeleteFile (MakePath (build_root, MAKEFILE));
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
