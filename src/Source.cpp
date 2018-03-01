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

#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)

#if !defined (TOOLCHAIN_OS_Windows)
    #include <sys/stat.h>
    #include <fcntl.h>
#endif // !defined (TOOLCHAIN_OS_Windows)
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "thekogans/util/ByteSwap.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/util/ChildProcess.h"
#include "thekogans/util/SHA2.h"
#include "thekogans/util/XMLUtils.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Version.h"
#include "thekogans/make/core/Source.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                std::ostream &operator << (
                        std::ostream &stream,
                        Source::Project &project) {
                    stream << "name: " << project.name << ",";
                    if (!project.branch.empty ()) {
                        stream << " branch: " << project.branch << ",";
                    }
                    stream <<
                        " version: " << project.version << "," <<
                        " SHA2_256: " << project.SHA2_256;
                    return stream;
                }

                std::ostream &operator << (
                        std::ostream &stream,
                        Source::Toolchain &toolchain) {
                    stream <<
                        "name: " << toolchain.name << "," <<
                        " version: " << toolchain.version << ",";
                    if (!toolchain.file.empty ()) {
                        stream << " file: " << toolchain.file << EXT_SEPARATOR << TAR_GZ_EXT << ",";
                    }
                    stream << " SHA2_256: " << toolchain.SHA2_256;
                    return stream;
                }
            }

            const char * const Source::ATTR_SCHEMA_VERSION = "schema_version";
            const char * const Source::ATTR_ORGANIZATION = "organization";
            const char * const Source::ATTR_URL = "url";
            const char * const Source::ATTR_NAME = "name";
            const char * const Source::ATTR_BRANCH = "branch";
            const char * const Source::ATTR_VERSION = "version";
            const char * const Source::ATTR_FILE = "file";
            const char * const Source::ATTR_SHA2_256 = "SHA2-256";

            const char * const Source::TAG_SOURCE = "source";
            const char * const Source::TAG_PROJECT = "project";
            const char * const Source::TAG_TOOLCHAIN = "toolchain";

            THEKOGANS_UTIL_IMPLEMENT_HEAP (Source)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (Source::Project)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (Source::Toolchain)

            Source::Source (const std::string &organization) {
                std::list<std::string> components;
                components.push_back (_DEVELOPMENT_ROOT);
                components.push_back (SOURCES_DIR);
                components.push_back (organization);
                components.push_back (SOURCE_XML);
                std::string sourceFilePath = ToSystemPath (MakePath (components, false));
                if (util::Path (sourceFilePath).Exists ()) {
                    util::ReadOnlyFile sourcesFile (util::HostEndian, sourceFilePath);
                    // Protect yourself.
                    const util::ui32 MAX_SOURCES_FILE_SIZE = 1024 * 1024;
                    util::ui32 sourcesFileSize = (util::ui32)sourcesFile.GetSize ();
                    if (sourcesFileSize > MAX_SOURCES_FILE_SIZE) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is bigger (%u) than expected. (%u)",
                            sourceFilePath.c_str (),
                            sourcesFileSize,
                            MAX_SOURCES_FILE_SIZE);
                    }
                    util::Buffer buffer (util::HostEndian, sourcesFileSize);
                    if (buffer.AdvanceWriteOffset (
                            sourcesFile.Read (
                                buffer.GetWritePtr (),
                                sourcesFileSize)) != sourcesFileSize) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to read %u bytes from '%s'.",
                            sourcesFileSize,
                            sourceFilePath.c_str ());
                    }
                    pugi::xml_document document;
                    pugi::xml_parse_result result =
                        document.load_buffer (
                            buffer.GetReadPtr (),
                            buffer.GetDataAvailableForReading ());
                    if (!result) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to parse: %s (%s)",
                            sourceFilePath.c_str (),
                            result.description ());
                    }
                    pugi::xml_node node = document.document_element ();
                    if (std::string (node.name ()) == TAG_SOURCE) {
                        Parsesource (node);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is not a valid source file.",
                            sourceFilePath.c_str ());
                    }
                }
            }

            void Source::Create (
                    const std::string &organization,
                    const std::string &url) {
                std::list<std::string> components;
                components.push_back (_DEVELOPMENT_ROOT);
                components.push_back (SOURCES_DIR);
                components.push_back (organization);
                components.push_back (SOURCE_XML);
                std::string sourceFilePath = ToSystemPath (MakePath (components, false));
                if (!util::Path (sourceFilePath).Exists ()) {
                    std::cout << "Adding " << organization << " - " << url << std::endl;
                    std::string directory = util::Path (sourceFilePath).GetDirectory ();
                    if (!util::Path (directory).Exists ()) {
                        util::Directory::Create (directory);
                    }
                    std::string projectsDirectory = util::MakePath (directory, PROJECTS_DIR);
                    if (!util::Path (projectsDirectory).Exists ()) {
                        util::Directory::Create (projectsDirectory);
                    }
                    std::string toolchainDirectory = util::MakePath (directory, TOOLCHAIN_DIR);
                    if (!util::Path (toolchainDirectory).Exists ()) {
                        util::Directory::Create (toolchainDirectory);
                    }
                    std::fstream sourceFile (
                        sourceFilePath.c_str (),
                        std::fstream::out | std::fstream::trunc);
                    if (sourceFile.is_open ()) {
                        util::Attributes attributes;
                        attributes.push_back (util::Attribute (ATTR_ORGANIZATION, organization));
                        attributes.push_back (util::Attribute (ATTR_URL, url));
                        attributes.push_back (
                            util::Attribute (
                                ATTR_SCHEMA_VERSION,
                                util::ui32Tostring (SOURCE_XML_SCHEMA_VERSION)));
                        sourceFile <<
                            util::OpenTag (1, TAG_SOURCE, attributes, false, true) <<
                            util::CloseTag (1, TAG_SOURCE);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to open: '%s'.",
                            sourceFilePath.c_str ());
                    }
                }
            }

            void Source::GetSources (std::set<std::string> &sources) {
                std::string sourcePath = ToSystemPath (MakePath (_DEVELOPMENT_ROOT, SOURCES_DIR));
                if (util::Path (sourcePath).Exists ()) {
                    util::Directory directory (sourcePath);
                    util::Directory::Entry entry;
                    for (bool gotEntry = directory.GetFirstEntry (entry);
                            gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                        if (entry.type == util::Directory::Entry::Folder &&
                                !util::IsDotOrDotDot (entry.name.c_str ())) {
                            std::list<std::string> components;
                            components.push_back (_DEVELOPMENT_ROOT);
                            components.push_back (SOURCES_DIR);
                            components.push_back (entry.name);
                            components.push_back (SOURCE_XML);
                            if (util::Path (ToSystemPath (MakePath (components, false))).Exists ()) {
                                sources.insert (entry.name);
                            }
                        }
                    }
                }
            }

            Source::Project *Source::GetProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const {
                for (std::list<Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    if ((*it)->name == name &&
                            (*it)->branch == branch &&
                            (*it)->version == version) {
                        return (*it).get ();
                    }
                }
                return 0;
            }

            void Source::GetProjectNames (
                    std::set<std::string> &names) const {
                for (std::list<Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    names.insert ((*it)->name);
                }
            }

            void Source::GetProjectBranches (
                    const std::string &name,
                    std::set<std::string> &branches) const {
                for (std::list<Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    if ((*it)->name == name) {
                        branches.insert ((*it)->branch);
                    }
                }
            }

            void Source::GetProjectVersions (
                    const std::string &name,
                    const std::string &branch,
                    std::set<std::string> &versions) const {
                for (std::list<Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    if ((*it)->name == name && (*it)->branch == branch) {
                        versions.insert ((*it)->version);
                    }
                }
            }

            std::string Source::GetProjectLatestVersion (
                    const std::string &name,
                    const std::string &branch) const {
                util::Version latestVersion = util::Version::Empty;
                for (std::list<Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    if ((*it)->name == name && (*it)->branch == branch) {
                        util::Version version ((*it)->version);
                        if (latestVersion < version) {
                            latestVersion = version;
                        }
                    }
                }
                return latestVersion.ToString ();
            }

            std::string Source::GetProjectSHA2_256 (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const {
                const Project *project = GetProject (name, branch, version);
                return project != 0 ? project->SHA2_256 : std::string ();
            }

            void Source::AddProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version,
                    const std::string &SHA2_256) {
                bool updated = false;
                for (std::list<Project::Ptr>::iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    if ((*it)->name == name && (*it)->branch == branch) {
                        if ((*it)->version == version) {
                            (*it)->SHA2_256 = SHA2_256;
                            std::cout << "Updating " << **it << std::endl;
                            updated = true;
                            break;
                        }
                        else if (util::Version (version) > util::Version ((*it)->version)) {
                            Project::Ptr project (new Project (name, branch, version, SHA2_256));
                            std::cout << "Adding " << *project << std::endl;
                            projects.insert (it, std::move (project));
                            updated = true;
                            break;
                        }
                    }
                }
                if (!updated) {
                    Project::Ptr project (new Project (name, branch, version, SHA2_256));
                    std::cout << "Adding " << *project << std::endl;
                    projects.push_back (std::move (project));
                }
            }

            bool Source::DeleteProject (
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) {
                for (std::list<Project::Ptr>::iterator
                        it = projects.begin (),
                        end = projects.end (); it != end;) {
                    if ((*it)->name == name &&
                            (*it)->branch == branch &&
                            (*it)->version == version) {
                        std::cout << "Deleting " << **it << std::endl;
                        projects.erase (it);
                        return true;
                    }
                }
                return false;
            }

            bool Source::CleanupProject (
                    const std::string &name,
                    const std::string &branch) {
                bool deleted = false;
                std::string latestVersion = GetProjectLatestVersion (name, branch);
                std::set<std::string> versions;
                GetProjectVersions (name, branch, versions);
                for (std::set<std::string>::const_iterator
                        it = versions.begin (),
                        end = versions.end (); it != end; ++it) {
                    if (latestVersion != *it) {
                        DeleteProject (name, branch, *it);
                        deleted = true;
                    }
                }
                return deleted;
            }

            Source::Toolchain *Source::GetToolchain (
                    const std::string &name,
                    const std::string &version) const {
                for (std::list<Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    if ((*it)->name == name && (*it)->version == version) {
                        return (*it).get ();
                    }
                }
                return 0;
            }

            void Source::GetToolchainNames (
                    std::set<std::string> &names) const {
                for (std::list<Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    names.insert ((*it)->name);
                }
            }

            void Source::GetToolchainVersions (
                    const std::string &name,
                    std::set<std::string> &versions) const {
                for (std::list<Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    if ((*it)->name == name) {
                        versions.insert ((*it)->version);
                    }
                }
            }

            std::string Source::GetToolchainLatestVersion (
                    const std::string &name) const {
                util::Version latestVersion = util::Version::Empty;
                for (std::list<Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    if ((*it)->name == name) {
                        util::Version version ((*it)->version);
                        if (latestVersion < version) {
                            latestVersion = version;
                        }
                    }
                }
                return latestVersion.ToString ();
            }

            std::string Source::GetToolchainFile (
                    const std::string &name,
                    const std::string &version) const {
                const Toolchain *toolchain = GetToolchain (name, version);
                return toolchain != 0 ? toolchain->file : std::string ();
            }

            std::string Source::GetToolchainSHA2_256 (
                    const std::string &name,
                    const std::string &version) const {
                const Toolchain *toolchain = GetToolchain (name, version);
                return toolchain != 0 ? toolchain->SHA2_256 : std::string ();
            }

            void Source::AddToolchain (
                    const std::string &name,
                    const std::string &version,
                    const std::string &file,
                    const std::string &SHA2_256) {
                bool updated = false;
                for (std::list<Toolchain::Ptr>::iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    if ((*it)->name == name) {
                        if ((*it)->version == version) {
                            (*it)->file = file;
                            (*it)->SHA2_256 = SHA2_256;
                            std::cout << "Updating " << **it << std::endl;
                            updated = true;
                            break;
                        }
                        else if (util::Version (version) > util::Version ((*it)->version)) {
                            Toolchain::Ptr toolchain_ (new Toolchain (name, version, file, SHA2_256));
                            std::cout << "Adding " << *toolchain_ << std::endl;
                            toolchain.insert (it, std::move (toolchain_));
                            updated = true;
                            break;
                        }
                    }
                }
                if (!updated) {
                    Toolchain::Ptr toolchain_ (new Toolchain (name, version, file, SHA2_256));
                    std::cout << "Adding " << *toolchain_ << std::endl;
                    toolchain.push_back (std::move (toolchain_));
                }
            }

            bool Source::DeleteToolchain (
                    const std::string &name,
                    const std::string &version) {
                for (std::list<Toolchain::Ptr>::iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end;) {
                    if ((*it)->name == name && (*it)->version == version) {
                        std::cout << "Deleting " << **it << std::endl;
                        toolchain.erase (it);
                        return true;
                    }
                }
                return false;
            }

            bool Source::CleanupToolchain (const std::string &name) {
                bool deleted = false;
                std::string latestVersion = GetToolchainLatestVersion (name);
                std::set<std::string> versions;
                GetToolchainVersions (name, versions);
                for (std::set<std::string>::const_iterator
                        it = versions.begin (),
                        end = versions.end (); it != end; ++it) {
                    if (latestVersion != *it) {
                        DeleteToolchain (name, *it);
                        deleted = true;
                    }
                }
                return deleted;
            }

            void Source::List () const {
                std::cout << organization << ": " << url << std::endl;
                std::cout << "projects:\n";
                for (std::list<Source::Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    std::cout << "  " << **it << std::endl;
                }
                std::cout << "toolchain:\n";
                for (std::list<Source::Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    std::cout << "  " << **it << std::endl;
                }
            }

            void Source::Clear () {
                projects.clear ();
                toolchain.clear ();
            }

            void Source::Save () const {
                std::list<std::string> components;
                components.push_back (_DEVELOPMENT_ROOT);
                components.push_back (SOURCES_DIR);
                components.push_back (organization);
                components.push_back (SOURCE_XML);
                std::string sourceFilePath = ToSystemPath (MakePath (components, false));
                std::fstream sourceFile (
                    sourceFilePath.c_str (),
                    std::fstream::out | std::fstream::trunc);
                if (sourceFile.is_open ()) {
                    Save (sourceFile, 0);
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to open: %s.",
                        sourceFilePath.c_str ());
                }
            }

            void Source::Parsesource (const pugi::xml_node &node) {
                organization = node.attribute (ATTR_ORGANIZATION).value ();
                if (organization.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                        "Empty organization in source.");
                }
                url = node.attribute (ATTR_URL).value ();
                if (url.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Empty url in source %s",
                        organization.c_str ());
                }
                schema_version = node.attribute (ATTR_SCHEMA_VERSION).value ();
                if (schema_version.empty ()) {
                    schema_version = util::ui32Tostring (SOURCE_XML_SCHEMA_VERSION);
                }
                if (atoi (schema_version.c_str ()) <= SOURCE_XML_SCHEMA_VERSION) {
                    for (pugi::xml_node child = node.first_child ();
                            !child.empty (); child = child.next_sibling ()) {
                        if (child.type () == pugi::node_element) {
                            std::string childName = child.name ();
                            if (childName == TAG_PROJECT) {
                                Parseproject (child);
                            }
                            else if (childName == TAG_TOOLCHAIN) {
                                Parsetoolchain (child);
                            }
                            else {
                                THEKOGANS_UTIL_LOG_WARNING (
                                    "Unrecognized tag '%s', skipping.\n",
                                    childName.c_str ());
                            }
                        }
                    }
                }
                else {
                    THEKOGANS_UTIL_LOG_WARNING (
                        "%s schema version (%s) is greater then we support (%u).\n"
                        "Please update your version (%s) of thekogans_make. (skipping)\n",
                        organization.c_str (),
                        schema_version.c_str (),
                        SOURCE_XML_SCHEMA_VERSION,
                        GetVersion ().ToString ().c_str ());
                }
            }

            void Source::Parseproject (const pugi::xml_node &node) {
                std::string name = node.attribute (ATTR_NAME).value ();
                std::string branch = node.attribute (ATTR_BRANCH).value ();
                std::string version = node.attribute (ATTR_VERSION).value ();
                std::string SHA2_256 = node.attribute (ATTR_SHA2_256).value ();
                if (!name.empty () && !version.empty () && !SHA2_256.empty ()) {
                    projects.push_back (
                        Source::Project::Ptr (
                            new Source::Project (name, branch, version, SHA2_256)));
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Malformed project entry:\n"
                        "  <source organization = \"%s\"\n"
                        "          url = \"%s\">\n"
                        "    <project name = \"%s\"\n"
                        "             branch = \"%s\"\n"
                        "             version = \"%s\"\n"
                        "             SHA2-256 = \"%s\"/>\n"
                        "  </source>\n",
                        organization.c_str (),
                        url.c_str (),
                        name.c_str (),
                        branch.c_str (),
                        version.c_str (),
                        SHA2_256.c_str ());
                }
            }

            void Source::Parsetoolchain (const pugi::xml_node &node) {
                std::string name = node.attribute (ATTR_NAME).value ();
                std::string version = node.attribute (ATTR_VERSION).value ();
                std::string file = node.attribute (ATTR_FILE).value ();
                std::string SHA2_256 = node.attribute (ATTR_SHA2_256).value ();
                if (!name.empty () && !version.empty () && !SHA2_256.empty ()) {
                    toolchain.push_back (
                        Source::Toolchain::Ptr (
                            new Source::Toolchain (name, version, file, SHA2_256)));
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Malformed toolchain entry:\n"
                        "  <source organization = \"%s\"\n"
                        "          url = \"%s\">\n"
                        "    <toolchain name = \"%s\"\n"
                        "               version = \"%s\"\n"
                        "               file = \"%s\"\n"
                        "               SHA2-256 = \"%s\"/>\n"
                        "  </source>\n",
                        organization.c_str (),
                        url.c_str (),
                        name.c_str (),
                        version.c_str (),
                        file.c_str (),
                        SHA2_256.c_str ());
                }
            }

            void Source::Save (
                    std::fstream &sourceFile,
                    util::ui32 indentationLevel) const {
                util::Attributes attributes;
                attributes.push_back (
                    util::Attribute (ATTR_ORGANIZATION, organization));
                attributes.push_back (
                    util::Attribute (ATTR_URL, url));
                attributes.push_back (
                    util::Attribute (ATTR_SCHEMA_VERSION, schema_version));
                sourceFile << util::OpenTag (indentationLevel, TAG_SOURCE, attributes, false, true);
                for (std::list<Source::Project::Ptr>::const_iterator
                        it = projects.begin (),
                        end = projects.end (); it != end; ++it) {
                    util::Attributes attributes;
                    attributes.push_back (util::Attribute (ATTR_NAME, (*it)->name));
                    if (!(*it)->branch.empty ()) {
                        attributes.push_back (util::Attribute (ATTR_BRANCH, (*it)->branch));
                    }
                    attributes.push_back (util::Attribute (ATTR_VERSION, (*it)->version));
                    attributes.push_back (util::Attribute (ATTR_SHA2_256, (*it)->SHA2_256));
                    sourceFile << util::OpenTag (indentationLevel + 1, TAG_PROJECT, attributes, true, true);
                }
                for (std::list<Source::Toolchain::Ptr>::const_iterator
                        it = toolchain.begin (),
                        end = toolchain.end (); it != end; ++it) {
                    util::Attributes attributes;
                    attributes.push_back (util::Attribute (ATTR_NAME, (*it)->name));
                    attributes.push_back (util::Attribute (ATTR_VERSION, (*it)->version));
                    if (!(*it)->file.empty ()) {
                        attributes.push_back (util::Attribute (ATTR_FILE, (*it)->file));
                    }
                    attributes.push_back (util::Attribute (ATTR_SHA2_256, (*it)->SHA2_256));
                    sourceFile << util::OpenTag (indentationLevel + 1, TAG_TOOLCHAIN, attributes, true, true);
                }
                sourceFile << util::CloseTag (indentationLevel, TAG_SOURCE);
            }

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
