// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_crypto.
//
// libthekogans_crypto is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_crypto is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_crypto. If not, see <http://www.gnu.org/licenses/>.

#include <string>
#include <vector>
#include "thekogans/util/CommandLineOptions.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/util/ConsoleLogger.h"
#include "thekogans/make/core/Package.h"
#include "thekogans/make/core/PackageResolver.h"

using namespace thekogans;

int main (
        int argc,
        const char *argv[]) {
    struct Options : public util::CommandLineOptions {
        bool help;
        std::string name;
        std::string type;

        Options () :
            help (false) {}

        virtual void DoOption (
                char option,
                const std::string &value) {
            switch (option) {
                case 'h': {
                    help = true;
                    break;
                }
                case 'n': {
                    name = value;
                    break;
                }
                case 't': {
                    type = value;
                    break;
                }
            }
        }
    } options;
    options.Parse (argc, argv, "hnt");
    if (options.help || options.name.empty ()) {
        std::cout << "usage: " << argv[0] << " [-h] -n:name [-t:Shared|Static]" << std::endl;
        return 1;
    }
    THEKOGANS_UTIL_LOG_INIT (
        util::LoggerMgr::Debug,
        util::LoggerMgr::All);
    THEKOGANS_UTIL_LOG_ADD_LOGGER (util::Logger::SharedPtr (new util::ConsoleLogger));
    THEKOGANS_UTIL_IMPLEMENT_LOG_FLUSHER;
    make::core::Package::SharedPtr package = make::core::PackageResolver::Instance ()->Resolve (
        options.name, options.type == "Static");
    auto print_bucket = [] (
            const std::string &name,
            const std::vector<std::string> &bucket) {
        std::cout << name << ": ";
        for (const auto &item : bucket) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    };
    std::cout << "--- Categorized Output for: " << package->name << " (" << package->path << ") ---\n";
    print_bucket ("Include Directories (-I)            ", package->include_directories);
    print_bucket ("Preprocessor Definitions (-D)       ", package->preprocessor_definitions);
    print_bucket ("Other Compiler Flags                ", package->other_cflags);
    print_bucket ("Library Search Paths (-L)           ", package->library_directories);
    print_bucket ("Framework Search Paths (-F)         ", package->framework_directories);
    print_bucket ("Library Names Linking (-l)          ", package->library_names);
    print_bucket ("Framework Names Linking (-framework)", package->framework_names);
    print_bucket ("Other Linker Flags                  ", package->other_libs);
    return 0;
}
