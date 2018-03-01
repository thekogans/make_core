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

#if !defined (__thekogans_make_core_Sources_h)
#define __thekogans_make_core_Sources_h

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
#include "thekogans/make/core/Source.h"
#include "thekogans/make/core/Utils.h"

namespace thekogans {
    namespace make {
        namespace core {

            #define SOURCES_XML "Sources.xml"
            #define SOURCES_XML_SCHEMA_VERSION 1

            /// \struct Sources Sources.h thekogans/make/Sources.h
            ///
            /// \brief
            /// Used to retrieve various info from the $TOOLCHAIN_ROOT/Sources.xml file.

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Sources {
                static const char * const ATTR_SCHEMA_VERSION;
                static const char * const TAG_SOURCES;

                std::string sourcesFilePath;
                std::string schema_version;
                std::list<Source::Ptr> sources;

                Sources (const std::string &sourcesFilePath =
                    ToSystemPath (MakePath (_TOOLCHAIN_ROOT, SOURCES_XML)));

                void ListSources () const;
                void UpdateSources ();

                void GetSources (std::set<std::string> &sources) const;
                void AddSource (
                    const std::string &organization,
                    const std::string &url);
                void DeleteSource (const std::string &organization);
                std::string GetSourceURL (const std::string &organization) const;

                std::string GetSourceProjectLatestVersion (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch) const;
                void GetSourceProjectBranches (
                    const std::string &organization,
                    const std::string &name,
                    std::set<std::string> &branches) const;
                void GetSourceProjectVersions (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    std::set<std::string> &versions) const;
                std::string GetSourceProjectSHA2_256 (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const;
                bool IsSourceProject (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const;
                void GetSourceProject (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const;

                std::string GetSourceToolchainLatestVersion (
                    const std::string &organization,
                    const std::string &name) const;
                void GetSourceToolchainVersions (
                    const std::string &organization,
                    const std::string &name,
                    std::set<std::string> &versions) const;
                std::string GetSourceToolchainFile (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const;
                std::string GetSourceToolchainSHA2_256 (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const;
                bool IsSourceToolchain (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const;
                void InstallSourceToolchain (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version,
                    const std::string &config = std::string (),
                    const std::string &type = std::string ()) const;

            private:
                Source *GetSource (const std::string &organization) const;
                void UpdateSource (Source &source);
                void Save () const;

                THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN (Sources)
            };

            typedef util::Singleton<Sources> ToolchainSources;

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)

#endif // !defined (__thekogans_make_core_Sources_h)
