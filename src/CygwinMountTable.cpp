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

#if defined (TOOLCHAIN_OS_Windows)

#include <algorithm>
#include <iostream>
#include <fstream>
#include "thekogans/util/Exception.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/util/WindowsUtils.h"
#include "thekogans/make/core/CygwinMountTable.h"

namespace thekogans {
    namespace make {
        namespace core {

            // This mapper was inspired by:
            // https://android.googlesource.com/platform/ndk/+/master/build/awk/gen-cygwin-deps-converter.awk

            namespace {
                // cygwin_mount_table.txt entries have the following format:
                // host ' ' 'on' ' ' cygwin ' ' 'type' ' ' type ' ' flags
                // FIXME: what happens if there are spaces in fields?
                CygwinMountTable::Entry ParseEntry (const std::string &line) {
                    CygwinMountTable::Entry entry;
                    {
                        std::string::size_type host = line.find_first_of (' ', 0);
                        if (host == std::string::npos) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Malformed %s entry: %s",
                                util::GetEnvironmentVariable (
                                    "TOOLCHAIN_CYGWIN_MOUNT_TABLE").c_str (),
                                line.c_str ());
                        }
                        std::string::size_type onString = line.find_first_of (' ', host + 1);
                        if (onString == std::string::npos) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Malformed %s entry: %s",
                                util::GetEnvironmentVariable (
                                    "TOOLCHAIN_CYGWIN_MOUNT_TABLE").c_str (),
                                line.c_str ());
                        }
                        std::string::size_type cygwin = line.find_first_of (' ', onString + 1);
                        if (cygwin == std::string::npos) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Malformed %s entry: %s",
                                util::GetEnvironmentVariable (
                                    "TOOLCHAIN_CYGWIN_MOUNT_TABLE").c_str (),
                                line.c_str ());
                        }
                        std::string::size_type typeString = line.find_first_of (' ', cygwin + 1);
                        if (typeString == std::string::npos) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Malformed %s entry: %s",
                                util::GetEnvironmentVariable (
                                    "TOOLCHAIN_CYGWIN_MOUNT_TABLE").c_str (),
                                line.c_str ());
                        }
                        std::string::size_type type = line.find_first_of (' ', typeString + 1);
                        if (type == std::string::npos) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Malformed %s entry: %s",
                                util::GetEnvironmentVariable (
                                    "TOOLCHAIN_CYGWIN_MOUNT_TABLE").c_str (),
                                line.c_str ());
                        }
                        entry.host =
                            util::TrimSpaces (line.substr (0, host).c_str ());
                        entry.cygwin =
                            util::TrimSpaces (line.substr (onString, cygwin - onString).c_str ());
                        entry.type =
                            util::TrimSpaces (line.substr (typeString, type - typeString).c_str ());
                        entry.flags =
                            util::TrimSpaces (line.substr (type).c_str ());
                        // The root directory is a special case, because we need
                        // to add a slash at the end of the corresponding host path
                        // otherwise c:/cygwin/foo will be translated into //foo
                        // instead of /foo.
                        if (entry.cygwin == "/") {
                            entry.host += "/";
                        }
                    }
                    return entry;
                }
            }

            CygwinMountTable::CygwinMountTable () {
                std::string mountTablePath =
                    util::GetEnvironmentVariable ("TOOLCHAIN_CYGWIN_MOUNT_TABLE");
                std::ifstream mountTableFile (mountTablePath.c_str ());
                if (mountTableFile.is_open ()) {
                    while (mountTableFile.good ()) {
                        std::string line;
                        std::getline (mountTableFile, line);
                        if (!line.empty ()) {
                            Entry entry = ParseEntry (line);
                            if (!entry.host.empty () && !entry.cygwin.empty ()) {
                                hostEntries.push_back (entry);
                                cygwinEntries.push_back (entry);
                            }
                        }
                    }
                    struct Comparecygwin {
                        inline bool operator () (
                                const Entry &entry1,
                                const Entry &entry2) const {
                            return entry1.cygwin.size () > entry2.cygwin.size ();
                        }
                    };
                    std::sort (hostEntries.begin (), hostEntries.end (), Comparecygwin ());
                    struct Comparehost {
                        inline bool operator () (
                                const Entry &entry1,
                                const Entry &entry2) const {
                            return entry1.host.size () > entry2.host.size ();
                        }
                    };
                    std::sort (cygwinEntries.begin (), cygwinEntries.end (), Comparehost ());
                    mountTableFile.close ();
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to open: %s",
                        mountTablePath.c_str ());
                }
            }

            void CygwinMountTable::DumpEntries () const {
                std::cout << "cygwin -> host:\n";
                for (std::vector<Entry>::const_iterator
                        it = hostEntries.begin (),
                        end = hostEntries.end (); it != end; ++it) {
                    std::cout << "  " << (*it).cygwin << " -> " << (*it).host << "\n";
                }
                std::cout << "host -> cygwin:\n";
                for (std::vector<Entry>::const_iterator
                        it = cygwinEntries.begin (),
                        end = cygwinEntries.end (); it != end; ++it) {
                    std::cout << "  " << (*it).host << " -> " << (*it).cygwin << "\n";
                }
                std::cout.flush ();
            }

            std::string CygwinMountTable::ToHostPath (const std::string &cygwinPath) const {
                std::string hostPath;
                if (!cygwinPath.empty ()) {
                    for (std::vector<Entry>::const_iterator
                            it = hostEntries.begin (),
                            end = hostEntries.end (); it != end; ++it) {
                        if (cygwinPath.size () >= (*it).cygwin.size () &&
                                strncasecmp (cygwinPath.c_str (), (*it).cygwin.c_str (),
                                    (*it).cygwin.size ()) == 0) {
                            hostPath = (*it).host + cygwinPath.substr ((*it).cygwin.size ());
                            break;
                        }
                    }
                    if (hostPath.empty ()) {
                        hostPath = cygwinPath;
                    }
                    std::replace (hostPath.begin (), hostPath.end (), '/', '\\');
                }
                return hostPath;
            }

            std::string CygwinMountTable::ToCygwinPath (const std::string &hostPath) const {
                std::string cygwinPath;
                if (!hostPath.empty ()) {
                    const char *ptr = 0;
                    for (std::vector<Entry>::const_iterator
                            it = cygwinEntries.begin (),
                            end = cygwinEntries.end (); it != end; ++it) {
                        if (hostPath.size () >= (*it).host.size () &&
                                strncasecmp (hostPath.c_str (), (*it).host.c_str (),
                                    (*it).host.size ()) == 0) {
                            cygwinPath = (*it).cygwin;
                            ptr = &hostPath[(*it).host.size ()];
                            break;
                        }
                    }
                    if (ptr == 0) {
                        ptr = &hostPath[0];
                    }
                    while (*ptr != 0) {
                        if (*ptr == '\\') {
                            cygwinPath += "/";
                        }
                        else if (*ptr == ' ') {
                            cygwinPath += "\\ ";
                        }
                        else {
                            cygwinPath += *ptr;
                        }
                        ++ptr;
                    }
                }
                return cygwinPath;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // defined (TOOLCHAIN_OS_Windows)
