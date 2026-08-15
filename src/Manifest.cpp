// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_make_core.
//
// libthekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#include <fstream>
#include "thekogans/util/Path.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Buffer.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/util/XMLUtils.h"
#include "thekogans/util/Exception.h"
#include "thekogans/make/core/Manifest.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                const char * const TAG_MANIFEST = "manifest";
                const char * const ATTR_SCHEMA_VERSION = "schema_version";

                const char * const TAG_FILE = "file";
                const char * const ATTR_NAME = "name";
                const char * const TAG_DEPENDENT = "dependent";

                const util::ui32 MANIFEST_XML_SCHEMA_VERSION = 1;
            }

            Manifest::Manifest (
                    const std::string &path_,
                    util::ui64 maxManifestFileSize) :
                    path (path_),
                    modified (false) {
                if (util::Path (path).Exists ()) {
                    util::ReadOnlyFile file (util::HostEndian, path);
                    // Protect yourself.
                    util::ui64 fileSize = file.GetSize ();
                    if (fileSize > maxManifestFileSize) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is bigger (%u) than expected. (" THEKOGANS_UTIL_UI64_FORMAT ")",
                            path.c_str (),
                            fileSize,
                            maxManifestFileSize);
                    }
                    util::Buffer buffer (util::HostEndian, (util::ui32)fileSize);
                    if (buffer.AdvanceWriteOffset (
                            file.Read (
                                buffer.GetWritePtr (),
                                (util::ui32)fileSize)) != (util::ui32)fileSize) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to read %u bytes from '%s'.",
                            fileSize,
                            path.c_str ());
                    }
                    pugi::xml_document document;
                    pugi::xml_parse_result result =
                        document.load_buffer (
                            buffer.GetReadPtr (),
                            buffer.GetDataAvailableForReading ());
                    if (!result) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to parse %s (%s)",
                            path.c_str (),
                            result.description ());
                    }
                    pugi::xml_node node = document.document_element ();
                    if (std::string (node.name ()) == TAG_MANIFEST) {
                        ParseManifest (node);
                    }
                }
            }

            bool Manifest::AddFile (
                    const std::string &file,
                    const std::string &dependent) {
                Files::iterator it = files.find (file);
                if (it == files.end ()) {
                    std::pair<Files::iterator, bool> result =
                        files.insert (Files::value_type (file, Dependents ()));
                    if (result.second) {
                        it = result.first;
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to insert %s (%s) in to %s.",
                            file.c_str (),
                            dependent.c_str (),
                            path.c_str ());
                    }
                }
                Dependents::iterator jt = it->second.find (dependent);
                if (jt == it->second.end ()) {
                    it->second.insert (dependent);
                    modified = true;
                    return true;
                }
                return false;
            }

            bool Manifest::DeleteFile (
                    const std::string &file,
                    const std::string &dependent) {
                bool returnCode = false;
                Files::iterator it = files.find (file);
                if (it != files.end ()) {
                    Dependents::iterator jt = it->second.find (dependent);
                    if (jt != it->second.end ()) {
                        it->second.erase (jt);
                        if (it->second.empty ()) {
                            files.erase (it);
                            returnCode = true;
                        }
                        modified = true;
                    }
                }
                return returnCode;
            }

            void Manifest::Save () {
                if (modified) {
                    std::fstream manifestFile (
                        path.c_str (),
                        std::fstream::out | std::fstream::trunc);
                    if (manifestFile.is_open ()) {
                        util::Attributes attributes;
                        attributes.push_back (
                            util::Attribute (
                                ATTR_SCHEMA_VERSION,
                                util::ui32Tostring (MANIFEST_XML_SCHEMA_VERSION)));
                        manifestFile << util::OpenTag (0, TAG_MANIFEST, attributes, false, true);
                        for (Files::const_iterator
                                 it = files.begin (),
                                 end = files.end (); it != end; ++it) {
                            util::Attributes attributes;
                            attributes.push_back (
                                util::Attribute (
                                    ATTR_NAME,
                                    util::EncodeXMLCharEntities (it->first)));
                            manifestFile << util::OpenTag (1, TAG_FILE, attributes, false, true);
                            for (Dependents::const_iterator
                                     jt = it->second.begin (),
                                     end = it->second.end (); jt != end; ++jt) {
                                manifestFile <<
                                    util::OpenTag (2, TAG_DEPENDENT,
                                        util::Attributes (), false, false) <<
                                    util::EncodeXMLCharEntities (*jt) <<
                                    util::CloseTag (0, TAG_DEPENDENT);
                            }
                            manifestFile << util::CloseTag (1, TAG_FILE);
                        }
                        manifestFile << util::CloseTag (0, TAG_MANIFEST);
                        modified = false;
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to open: %s.",
                            path.c_str ());
                    }
                }
            }

            void Manifest::ParseManifest (pugi::xml_node &node) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_FILE) {
                            ParseFile (child);
                        }
                    }
                }
            }

            void Manifest::ParseFile (pugi::xml_node &node) {
                std::string name = util::Decodestring (node.attribute (ATTR_NAME).value ());
                if (!name.empty ()) {
                    std::pair<Files::iterator, bool> result =
                        files.insert (Files::value_type (name, Dependents ()));
                    if (result.second) {
                        for (pugi::xml_node child = node.first_child ();
                                !child.empty (); child = child.next_sibling ()) {
                            if (child.type () == pugi::node_element) {
                                std::string childName = child.name ();
                                if (childName == TAG_DEPENDENT) {
                                    std::string dependent =
                                        util::Decodestring (util::TrimSpaces (child.text ().get ()));
                                    if (!dependent.empty ()) {
                                        result.first->second.insert (dependent);
                                    }
                                }
                            }
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to insert %s in to %s.",
                            name.c_str (),
                            path.c_str ());
                    }
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
