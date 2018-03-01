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

#if !defined (__thekogans_make_core_Utils_h)
#define __thekogans_make_core_Utils_h

#include <string>
#include <list>
#include <set>
#include <map>
#include "thekogans/util/Singleton.h"
#include "thekogans/util/SpinLock.h"
#include "thekogans/util/Buffer.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/make/core/Config.h"
#if defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/make/core/CygwinMountTable.h"
#endif // defined (TOOLCHAIN_OS_Windows)
#include "thekogans/make/core/Value.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct thekogans_make;

            #define THEKOGANS_MAKE_XML "thekogans_make.xml"
            #define THEKOGANS_MAKE_XML_SCHEMA_VERSION 2

            #define NAMING_CONVENTION_FLAT "Flat"
            #define NAMING_CONVENTION_HIERARCHICAL "Hierarchical"

            #define PROJECT_TYPE_LIBRARY "library"
            #define PROJECT_TYPE_PROGRAM "program"
            #define PROJECT_TYPE_PLUGIN "plugin"

            #define CONFIG_DEBUG "Debug"
            #define CONFIG_RELEASE "Release"

            #define TYPE_SHARED "Shared"
            #define TYPE_STATIC "Static"

            #define MODE_DEVELOPMENT "Development"
            #define MODE_INSTALL "Install"

            #define VALUE_YES "yes"
            #define VALUE_NO "no"

            #define TARGET_ALL "all"
            #define TARGET_TESTS "tests"
            #define TARGET_CLEAN "clean"
            #define TARGET_CLEAN_SELF "clean_self"
            #define TARGET_TAGS "tags"

            #define MAKE "make"
            #define MAKEFILE "Makefile"

            #define PATH_SEPARATOR "/"
            #define PATH_SEPARATOR_CHAR '/'
            #define ORGANIZATION_PROJECT_SEPARATOR "_"
            #define PROJECT_EXAMPLE_SEPARATOR "_"
            #define DECORATIONS_SEPARATOR "-"
            #define VERSION_SEPARATOR "."
            #define EXT_SEPARATOR "."
            #define EXT_SEPARATOR_CHAR '.'

            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string BIN_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string LIB_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string SRC_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string INCLUDE_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string RESOURCES_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string EXAMPLES_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string DOC_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TESTS_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string BUILD_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string CONFIG_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string COMMON_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string SOURCES_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string PROJECTS_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TOOLCHAIN_DIR;

            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string LIB_PREFIX;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string TAR_GZ_EXT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string XML_EXT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string PLUGINS_EXT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string THEKOGANS_MANIFEST;

            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _DEVELOPMENT_ROOT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ROOT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_OS;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ARCH;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_COMPILER;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_TRIPLET;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_ORGANIZATION;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_PROJECT;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_BRANCH;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DEFAULT_VERSION;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_NAMING_CONVENTION;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_NAME;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_SHELL;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_ENDIAN;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_DIR;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_BRANCH;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_PROGRAM_SUFFIX;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_SHARED_LIBRARY_SUFFIX;
            extern _LIB_THEKOGANS_MAKE_CORE_DECL const std::string _TOOLCHAIN_STATIC_LIBRARY_SUFFIX;

            typedef std::map<std::string, Value> SymbolTable;

            struct _LIB_THEKOGANS_MAKE_CORE_DECL EnvironmentSymbolTable :
                    public util::Singleton<EnvironmentSymbolTable, util::SpinLock>,
                    public SymbolTable {
                EnvironmentSymbolTable ();
            };

        #if defined (TOOLCHAIN_OS_Windows)
            #define ToSystemPath(path) thekogans::make::core::CygwinMountTable::Instance ().ToHostPath (path)
        #else // defined (TOOLCHAIN_OS_Windows)
            #define ToSystemPath(path) (path)
        #endif // defined (TOOLCHAIN_OS_Windows)
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API ParseQuotedString (
                const thekogans_make &config,
                thekogans::util::Buffer &buffer,
                char quoteCh);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetLinkLibrarySuffix (
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetFileName (
                const std::string &organization,
                const std::string &project,
                const std::string &branch,
                const std::string &version,
                const std::string &ext);
            _LIB_THEKOGANS_MAKE_CORE_DECL util::ui32 _LIB_THEKOGANS_MAKE_CORE_API ParseFileName (
                const std::string &fileName,
                std::string &organization,
                std::string &project,
                std::string &branch,
                std::string &version,
                std::string &ext);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetBuildDirectory (
                const std::string &generator,
                const std::string &config,
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetBuildRoot (
                const std::string &project_root,
                const std::string &generator,
                const std::string &config,
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API BuildRootExists (
                const std::string &project_root,
                const std::string &generator,
                const std::string &config,
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CreateBuildRoot (
                const std::string &project_root,
                const std::string &generator,
                const std::string &config,
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDirectoryFromName (
                const std::string &name);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetNameFromDirectory (
                const std::string &directory);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDefaultBranch (
                const std::string &organization,
                const std::string &project);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetDefaultVersion (
                const std::string &organization,
                const std::string &project);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetConfigKey (
                const std::string &project_root,
                const std::string &config_file,
                const std::string &generator,
                const std::string &config,
                const std::string &type);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API MakePath (
                const std::string &path1,
                const std::string &path2);
            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API MakePath (
                const std::list<std::string> &components,
                bool absolute);

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetFileHash (
                const std::string &path);
            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API CopyFile (
                const std::string &from,
                const std::string &to);
            _LIB_THEKOGANS_MAKE_CORE_DECL bool _LIB_THEKOGANS_MAKE_CORE_API DeleteFile (
                const std::string &file);

            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API Uninstall (
                const std::string &organization,
                const std::string &project,
                const std::string &version,
                bool dependencies,
                std::set<std::string> &visitedDependencies);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallLibrary (
                const std::string &organization,
                const std::string &project,
                const std::string &version,
                bool dependencies);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallProgram (
                const std::string &organization,
                const std::string &project,
                const std::string &version,
                bool dependencies);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API UninstallPlugin (
                const std::string &organization,
                const std::string &project,
                const std::string &version);


            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CopyDependencies (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                const std::string &destination = std::string ());
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CopyPlugin (
                const std::string &project_root,
                const std::string &config);

            _LIB_THEKOGANS_MAKE_CORE_DECL std::string _LIB_THEKOGANS_MAKE_CORE_API GetGeneratorList (
                const std::string &separator);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API CreateBuildSystem (
                const std::string &project_root,
                const std::string &generator,
                const std::string &config,
                const std::string &type,
                bool generateDependencies);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API DeleteBuildSystem (
                const std::string &project_root,
                const std::string &generator,
                const std::string &config,
                const std::string &type,
                bool deleteDependencies);
            _LIB_THEKOGANS_MAKE_CORE_DECL void _LIB_THEKOGANS_MAKE_CORE_API BuildProject (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                const std::string &mode,
                bool hide_commands,
                bool parallel_build,
                const std::string &target);

            inline bool IsEscapableCh (char ch) {
                return
                    ch == '\\' || ch == '$' || ch == '#' ||
                    ch == '?' || ch == '"' || ch == '\'' ||
                    ch == ' ';
            }

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Utils_h)
