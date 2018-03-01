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
#include "thekogans/make/core/Sources.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                std::ostream &operator << (
                        std::ostream &stream,
                        Source &source) {
                    stream << source.organization << " - " << source.url;
                    return stream;
                }
            }

            const char * const Sources::ATTR_SCHEMA_VERSION = "schema_version";
            const char * const Sources::TAG_SOURCES = "sources";

            Sources::Sources (const std::string &sourcesFilePath_) :
                    sourcesFilePath (sourcesFilePath_) {
                if (util::Path (sourcesFilePath).Exists ()) {
                    util::ReadOnlyFile sourcesFile (util::HostEndian, sourcesFilePath);
                    // Protect yourself.
                    const util::ui32 MAX_SOURCES_FILE_SIZE = 1024 * 1024;
                    util::ui32 sourcesFileSize = (util::ui32)sourcesFile.GetSize ();
                    if (sourcesFileSize > MAX_SOURCES_FILE_SIZE) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is bigger (%u) than expected. (%u)",
                            sourcesFilePath.c_str (),
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
                            sourcesFilePath.c_str ());
                    }
                    pugi::xml_document document;
                    pugi::xml_parse_result result =
                        document.load_buffer (
                            buffer.GetReadPtr (),
                            buffer.GetDataAvailableForReading ());
                    if (!result) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to parse: %s (%s)",
                            sourcesFilePath.c_str (),
                            result.description ());
                    }
                    pugi::xml_node node = document.document_element ();
                    if (std::string (node.name ()) == TAG_SOURCES) {
                        schema_version = node.attribute (ATTR_SCHEMA_VERSION).value ();
                        if (schema_version.empty ()) {
                            schema_version = util::ui32Tostring (SOURCES_XML_SCHEMA_VERSION);
                        }
                        if (atoi (schema_version.c_str ()) > SOURCES_XML_SCHEMA_VERSION) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "%s schema version (%s) is greater then we support (%u). "
                                "Please update your version (%s) of thekogans_make.",
                                sourcesFilePath.c_str (),
                                schema_version.c_str (),
                                SOURCES_XML_SCHEMA_VERSION,
                                GetVersion ().ToString ().c_str ());
                        }
                        for (pugi::xml_node child = node.first_child ();
                                !child.empty (); child = child.next_sibling ()) {
                            if (child.type () == pugi::node_element) {
                                std::string childName = child.name ();
                                if (childName == Source::TAG_SOURCE) {
                                    sources.push_back (Source::Ptr (new Source (child)));
                                }
                            }
                        }
                        if (atoi (schema_version.c_str ()) != SOURCES_XML_SCHEMA_VERSION) {
                            schema_version = util::ui32Tostring (SOURCES_XML_SCHEMA_VERSION);
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is not a valid sources file.",
                            sourcesFilePath.c_str ());
                    }
                }
            }

            void Sources::ListSources () const {
                if (!sources.empty ()) {
                    for (std::list<Source::Ptr>::const_iterator
                            it = sources.begin (),
                            end = sources.end (); it != end; ++it) {
                        (*it)->List ();
                    }
                }
                else {
                    std::cout << "No sources found in " <<
                        sourcesFilePath << std::endl;
                }
                std::cout.flush ();
            }

            void Sources::UpdateSources () {
                if (!sources.empty ()) {
                    for (std::list<Source::Ptr>::iterator
                            it = sources.begin (),
                            end = sources.end (); it != end; ++it) {
                        THEKOGANS_UTIL_TRY {
                            std::cout << "Updating " << **it << std::endl;
                            std::cout.flush ();
                            UpdateSource (**it);
                        }
                        THEKOGANS_UTIL_CATCH (util::Exception) {
                            std::cout << "Unable to update " << **it <<
                                "(" << exception.what () << "), skipping.\n";
                            std::cout.flush ();
                        }
                    }
                    Save ();
                }
                else {
                    std::cout << "No sources found in " <<
                        sourcesFilePath << std::endl;
                    std::cout.flush ();
                }
            }

            void Sources::GetSources (std::set<std::string> &sources_) const {
                for (std::list<Source::Ptr>::const_iterator
                        it = sources.begin (),
                        end = sources.end (); it != end; ++it) {
                    sources_.insert ((*it)->organization);
                }
            }

            void Sources::AddSource (
                    const std::string &organization,
                    const std::string &url) {
                Source *source = GetSource (organization);
                if (source != 0) {
                    std::cout << "Updating " << *source << " -> " << url << std::endl;
                    source->url = url;
                }
                else {
                    source = new Source (organization, url);
                    std::cout << "Adding " << *source << std::endl;
                    sources.push_back (Source::Ptr (source));
                }
                std::cout.flush ();
                UpdateSource (*source);
                Save ();
            }

            void Sources::DeleteSource (const std::string &organization) {
                for (std::list<Source::Ptr>::iterator
                        it = sources.begin (),
                        end = sources.end (); it != end; ++it) {
                    if ((*it)->organization == organization) {
                        std::cout << "Deleting " << **it << std::endl;
                        sources.erase (it);
                        Save ();
                        return;
                    }
                }
                std::cout << organization << " not found.\n";
                std::cout.flush ();
            }

            std::string Sources::GetSourceURL (
                    const std::string &organization) const {
                const Source *source = GetSource (organization);
                return source != 0 ? source->url : std::string ();
            }

            std::string Sources::GetSourceProjectLatestVersion (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch) const {
                const Source *source = GetSource (organization);
                return source != 0 ?
                    source->GetProjectLatestVersion (name, branch) :
                    std::string ();
            }

            void Sources::GetSourceProjectBranches (
                    const std::string &organization,
                    const std::string &name,
                    std::set<std::string> &branches) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    source->GetProjectBranches (name, branches);
                }
            }

            void Sources::GetSourceProjectVersions (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    std::set<std::string> &versions) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    source->GetProjectVersions (name, branch, versions);
                }
            }

            std::string Sources::GetSourceProjectSHA2_256 (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                return source != 0 ?
                    source->GetProjectSHA2_256 (name, branch, version) :
                    std::string ();
            }

            bool Sources::IsSourceProject (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    const Source::Project *project = source->GetProject (name, branch, version);
                    if (project != 0) {
                        return true;
                    }
                }
                return false;
            }

            void Sources::GetSourceProject (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &branch,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    const Source::Project *project = source->GetProject (name, branch, version);
                    if (project != 0) {
                        util::ChildProcess shellProcess (ToSystemPath (_TOOLCHAIN_SHELL));
                        std::list<std::string> components;
                        components.push_back (_TOOLCHAIN_ROOT);
                        components.push_back (COMMON_DIR);
                        components.push_back (BIN_DIR);
                        components.push_back ("gettoolchainsourceproject");
                        shellProcess.AddArgument (MakePath (components, false));
                        shellProcess.AddArgument ("-o:" + source->organization);
                        shellProcess.AddArgument ("-u:" + source->url);
                        shellProcess.AddArgument ("-p:" + project->name);
                        if (!project->branch.empty ()) {
                            shellProcess.AddArgument ("-b:" + project->branch);
                        }
                        shellProcess.AddArgument ("-v:" + project->version);
                        shellProcess.AddArgument ("-s:" + project->SHA2_256);
                        util::ChildProcess::ChildStatus childStatus = shellProcess.Exec ();
                        if (childStatus == util::ChildProcess::Failed ||
                                shellProcess.GetReturnCode () != 0) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Unable to execute: '%s'.",
                                shellProcess.BuildCommandLine ().c_str ());
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "No project entry for: '%s'.",
                            GetFileName (organization, name, branch, version, TAR_GZ_EXT).c_str ());
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "No source entry for project: '%s'.",
                        organization.c_str ());
                }
            }

            std::string Sources::GetSourceToolchainLatestVersion (
                    const std::string &organization,
                    const std::string &name) const {
                const Source *source = GetSource (organization);
                return source != 0 ?
                    source->GetToolchainLatestVersion (name) :
                    std::string ();
            }

            void Sources::GetSourceToolchainVersions (
                    const std::string &organization,
                    const std::string &name,
                    std::set<std::string> &versions) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    source->GetToolchainVersions (name, versions);
                }
            }

            std::string Sources::GetSourceToolchainFile (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                return source != 0 ? source->GetToolchainFile (name, version) : std::string ();
            }

            std::string Sources::GetSourceToolchainSHA2_256 (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                return source != 0 ? source->GetToolchainSHA2_256 (name, version) : std::string ();
            }

            namespace {
                struct DataSink {
                    virtual ~DataSink () {}

                    virtual std::size_t HandleData (
                        void *data,
                        std::size_t elementSize,
                        std::size_t elementCount) = 0;
                };

                struct CURLHandle {
                    CURL *curl;
                    DataSink &dataSink;

                    CURLHandle (
                            const std::string &url,
                            DataSink &dataSink_) :
                            curl (curl_easy_init ()),
                            dataSink (dataSink_) {
                        if (curl != 0) {
                            curl_easy_setopt (curl, CURLOPT_URL, url.c_str ());
                            curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
                            curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, Callback);
                            curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *)this);
                            curl_easy_setopt (curl, CURLOPT_USERAGENT, "thekogans_make-agent/1.0");
                            curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0L);
                            curl_easy_setopt (curl, CURLOPT_XFERINFOFUNCTION, ProgressBar);
                            curl_easy_setopt (curl, CURLOPT_FAILONERROR, 1L);
                            curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 0);
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                "curl_easy_init failed.");
                        }
                    }
                    ~CURLHandle () {
                        curl_easy_cleanup (curl);
                        std::cout << std::endl;
                    }

                    void GetURL () {
                        CURLcode code = curl_easy_perform (curl);
                        if (code != CURLE_OK) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                curl_easy_strerror (code));
                        }
                    }

                private:
                    static size_t Callback (
                            void *data,
                            size_t elementSize,
                            size_t elementCount,
                            void *userData) {
                        CURLHandle *curlHandle = (CURLHandle *)userData;
                        return curlHandle->dataSink.HandleData (data, elementSize, elementCount);
                    }

                    static int ProgressBar (
                            void * /*clientp*/,
                            curl_off_t dltotal,
                            curl_off_t dlnow,
                            curl_off_t /*ultotal*/,
                            curl_off_t /*ulnow*/) {
                        double fraction = (double)dlnow / (double)dltotal;
                        const int MAX_BARWIDTH =  79;
                        int count = (int)((MAX_BARWIDTH - 7) * fraction);
                        if (count > 0) {
                            std::cout << "\r";
                            std::cout.width (MAX_BARWIDTH);
                            std::cout << std::left << std::string (count, '#') << (int)(fraction * 100.0) << "%";
                            std::cout.flush ();
                        }
                        return 0;
                    }
                };
            }

            bool Sources::IsSourceToolchain (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    const Source::Toolchain *toolchain = source->GetToolchain (name, version);
                    if (toolchain != 0) {
                        return true;
                    }
                }
                return false;
            }

            void Sources::InstallSourceToolchain (
                    const std::string &organization,
                    const std::string &name,
                    const std::string &version,
                    const std::string &config,
                    const std::string &type) const {
                const Source *source = GetSource (organization);
                if (source != 0) {
                    const Source::Toolchain *toolchain = source->GetToolchain (name, version);
                    if (toolchain != 0) {
                        util::ChildProcess shellProcess (ToSystemPath (_TOOLCHAIN_SHELL));
                        std::list<std::string> components;
                        components.push_back (_TOOLCHAIN_ROOT);
                        components.push_back (COMMON_DIR);
                        components.push_back (BIN_DIR);
                        components.push_back ("installtoolchainsourcetoolchain");
                        shellProcess.AddArgument (MakePath (components, false));
                        shellProcess.AddArgument ("-o:" + source->organization);
                        shellProcess.AddArgument ("-u:" + source->url);
                        shellProcess.AddArgument ("-p:" + toolchain->name);
                        shellProcess.AddArgument ("-v:" + toolchain->version);
                        if (!toolchain->file.empty ()) {
                            shellProcess.AddArgument ("-f:" + toolchain->file);
                        }
                        shellProcess.AddArgument ("-c:" + config);
                        shellProcess.AddArgument ("-t:" + type);
                        util::ChildProcess::ChildStatus childStatus = shellProcess.Exec ();
                        if (childStatus == util::ChildProcess::Failed ||
                                shellProcess.GetReturnCode () != 0) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Unable to execute: '%s'.",
                                shellProcess.BuildCommandLine ().c_str ());
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "No toolchain entry for: '%s'.",
                            GetFileName (organization, name, std::string (), version, TAR_GZ_EXT).c_str ());
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "No source entry for: '%s'.",
                        organization.c_str ());
                }
            }

            Source *Sources::GetSource (const std::string &organization) const {
                for (std::list<Source::Ptr>::const_iterator
                        it = sources.begin (),
                        end = sources.end (); it != end; ++it) {
                    if ((*it)->organization == organization) {
                        return (*it).get ();
                    }
                }
                return 0;
            }

            void Sources::UpdateSource (Source &source) {
                struct BufferDataSink : public DataSink {
                    std::vector<util::ui8> buffer;

                    virtual std::size_t HandleData (
                            void *data,
                            std::size_t elementSize,
                            std::size_t elementCount) {
                        std::size_t size = elementSize * elementCount;
                        if (size != 0) {
                            std::size_t oldSize = buffer.size ();
                            buffer.resize (oldSize + size);
                            memcpy (&buffer[oldSize], data, size);
                        }
                        return size;
                    }
                } bufferDataSink;
                std::string sourceUrl =
                    MakePath (MakePath (source.url, source.organization), SOURCE_XML);
                CURLHandle curlHandle (sourceUrl, bufferDataSink);
                curlHandle.GetURL ();
                source.Clear ();
                if (!bufferDataSink.buffer.empty ()) {
                    pugi::xml_document document;
                    pugi::xml_parse_result result =
                        document.load_buffer (
                            &bufferDataSink.buffer[0],
                            bufferDataSink.buffer.size ());
                    if (!result) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to parse: %s (%s)",
                            sourceUrl.c_str (),
                            result.description ());
                    }
                    pugi::xml_node node = document.document_element ();
                    if (std::string (node.name ()) == Source::TAG_SOURCE) {
                        source.Parsesource (node);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "'%s' is not a valid source file.",
                            sourceUrl.c_str ());
                    }
                }
            }

            void Sources::Save () const {
                std::fstream sourcesFile (
                    sourcesFilePath.c_str (),
                    std::fstream::out | std::fstream::trunc);
                if (sourcesFile.is_open ()) {
                    util::Attributes attributes;
                    attributes.push_back (
                        util::Attribute (
                            ATTR_SCHEMA_VERSION,
                            util::ui32Tostring (SOURCES_XML_SCHEMA_VERSION)));
                    sourcesFile << util::OpenTag (0, TAG_SOURCES, attributes, false, true);
                    for (std::list<Source::Ptr>::const_iterator
                            it = sources.begin (),
                            end = sources.end (); it != end; ++it) {
                        (*it)->Save (sourcesFile, 1);
                    }
                    sourcesFile << util::CloseTag (0, TAG_SOURCES);
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to open: %s.",
                        sourcesFilePath.c_str ());
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
