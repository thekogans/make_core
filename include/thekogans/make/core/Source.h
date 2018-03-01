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

#if !defined (__thekogans_make_core_Source_h)
#define __thekogans_make_core_Source_h

#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)

#include <memory>
#include <string>
#include <list>
#include <set>
#include <pugixml.hpp>
#include "thekogans/util/Heap.h"
#include "thekogans/util/Singleton.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            #define SOURCE_XML "Source.xml"
            #define SOURCE_XML_SCHEMA_VERSION 1

            /// \struct Source Source.h thekogans/make/Source.h
            ///
            /// \brief
            /// Used to retrieve various info from the $DEVELOPMENT_ROOT/sources/$organization/Source.xml file.

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Source {
                typedef std::unique_ptr<Source> Ptr;

                THEKOGANS_UTIL_DECLARE_HEAP (Source)

                static const char * const ATTR_SCHEMA_VERSION;
                static const char * const ATTR_ORGANIZATION;
                static const char * const ATTR_URL;
                static const char * const ATTR_NAME;
                static const char * const ATTR_BRANCH;
                static const char * const ATTR_VERSION;
                static const char * const ATTR_FILE;
                static const char * const ATTR_SHA2_256;

                static const char * const TAG_SOURCE;
                static const char * const TAG_PROJECT;
                static const char * const TAG_TOOLCHAIN;

                std::string organization;
                std::string url;
                std::string schema_version;
                struct Project {
                    typedef std::unique_ptr<Project> Ptr;

                    THEKOGANS_UTIL_DECLARE_HEAP (Project)

                    std::string name;
                    std::string branch;
                    std::string version;
                    std::string SHA2_256;

                    Project (
                        const std::string &name_,
                        const std::string &branch_,
                        const std::string &version_,
                        const std::string &SHA2_256_) :
                        name (name_),
                        branch (branch_),
                        version (version_),
                        SHA2_256 (SHA2_256_) {}

                    THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN (Project)
                };
                std::list<Project::Ptr> projects;
                struct Toolchain {
                    typedef std::unique_ptr<Toolchain> Ptr;

                    THEKOGANS_UTIL_DECLARE_HEAP (Toolchain)

                    std::string name;
                    std::string version;
                    std::string file;
                    std::string SHA2_256;

                    Toolchain (
                        const std::string &name_,
                        const std::string &version_,
                        const std::string &file_,
                        const std::string &SHA2_256_) :
                        name (name_),
                        version (version_),
                        file (file_),
                        SHA2_256 (SHA2_256_) {}

                    THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN (Toolchain)
                };
                std::list<Toolchain::Ptr> toolchain;

                explicit Source (const std::string &organization);
                explicit Source (const pugi::xml_node &node) {
                    Parsesource (node);
                }
                Source (
                    const std::string &organization_,
                    const std::string &url_) :
                    organization (organization_),
                    url (url_),
                    schema_version (util::ui32Tostring (SOURCE_XML_SCHEMA_VERSION)) {}

                static void Create (
                    const std::string &organization,
                    const std::string &url);
                static void GetSources (std::set<std::string> &sources);

                Project *GetProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const;
                void GetProjectNames (std::set<std::string> &names) const;
                void GetProjectBranches (
                    const std::string &name,
                    std::set<std::string> &branches) const;
                void GetProjectVersions (
                    const std::string &name,
                    const std::string &branch,
                    std::set<std::string> &versions) const;
                std::string GetProjectLatestVersion (
                    const std::string &name,
                    const std::string &branch) const;
                std::string GetProjectSHA2_256 (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const;
                void AddProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &SHA2_256);
                bool DeleteProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version);
                bool CleanupProject (
                    const std::string &name,
                    const std::string &branch);

                Toolchain *GetToolchain (
                    const std::string &name,
                    const std::string &version) const;
                void GetToolchainNames (std::set<std::string> &names) const;
                void GetToolchainVersions (
                    const std::string &name,
                    std::set<std::string> &versions) const;
                std::string GetToolchainLatestVersion (
                    const std::string &name) const;
                std::string GetToolchainFile (
                    const std::string &name,
                    const std::string &version) const;
                std::string GetToolchainSHA2_256 (
                    const std::string &name,
                    const std::string &version) const;
                void AddToolchain (
                    const std::string &name,
                    const std::string &version,
                    const std::string &file,
                    const std::string &SHA2_256);
                bool DeleteToolchain (
                    const std::string &name,
                    const std::string &version);
                bool CleanupToolchain (const std::string &name);

                void List () const;

                void Clear ();
                void Save () const;

            private:
                void Parsesource (const pugi::xml_node &node);
                void Parseproject (const pugi::xml_node &node);
                void Parsetoolchain (const pugi::xml_node &node);
                void Save (
                    std::fstream &sourceFile,
                    util::ui32 indentationLevel) const;

                friend struct Sources;

                THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN (Source)
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)

#endif // !defined (__thekogans_make_core_Source_h)
