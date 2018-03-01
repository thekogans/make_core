// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_stream.
//
// libthekogans_stream is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_stream is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_stream. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_core_Manifest_h)
#define __thekogans_make_core_Manifest_h

#include <string>
#include <set>
#include <map>
#include <pugixml.hpp>
#include "thekogans/util/Types.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Manifest {
            private:
                std::string path;
                typedef std::set<std::string> Dependents;
                typedef std::map<std::string, Dependents> Files;
                Files files;
                bool modified;

            public:
                enum {
                    /// \brief
                    /// Default max manifest file size.
                    DEFAULT_MAX_MANIFEST_FILE_SIZE = 1024 * 1024
                };

                /// \brief
                /// ctor.
                /// \param[in] path_ Path to the manifest xml file.
                /// \param[in] maxManifestFileSize
                Manifest (
                    const std::string &path_,
                    util::ui64 maxManifestFileSize = DEFAULT_MAX_MANIFEST_FILE_SIZE);

                /// \brief
                /// \param[in] file
                /// \return
                bool AddFile (
                    const std::string &file,
                    const std::string &dependent);
                /// \brief
                /// \param[in] file
                /// \return
                bool DeleteFile (
                    const std::string &file,
                    const std::string &dependent);

                /// \brief
                /// Save the manifest to the file.
                void Save ();

            private:
                /// \brief
                /// Parse the manifest tag.
                /// \param[in] node Root node.
                void ParseManifest (pugi::xml_node &node);
                void ParseFile (pugi::xml_node &node);

                /// \brief
                /// Manifest is neither copy constructable, nor assignable.
                THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (Manifest)
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Manifest_h)
