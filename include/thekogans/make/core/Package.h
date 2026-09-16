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

#if !defined (__thekogans_make_core_Package_h)
#define __thekogans_make_core_Package_h

#include <string>
#include <vector>
#include "thekogans/util/RefCounted.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Version.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct PackageResolver;

            // Standard properties
            // Name
            // Version
            // Description
            // URL
            // Libs             // when parsing look for -L (library_directory),
            // Libs.private     // -F (framework_directory), -framework (framework) and -l (library)
            //                  // everything else is a linker_flag.
            // Cflags           // when parsing look for -I (include_directory),
            // Cflags.private   // -D (preprocessor_definition)
            //                  // everything else is a c_flag.
            // Requires         // when parsing account for version.
            // Requires.private
            // Conflicts
            // Provides
            // Extracted fields from a single .pc file
            struct _LIB_THEKOGANS_MAKE_CORE_DECL Package : public util::RefCounted {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (Package)

                std::string path;
                std::string fileName;
                std::string name;
                util::Version version;
                std::string description;
                std::string url;
                // Holds structured version comparison/conflict criteria
                struct Constraint {
                    std::string name;
                    util::Version::OP op;
                    util::Version version;

                    Constraint (
                        const std::string &name_ = std::string (),
                        util::Version::OP op_ = util::Version::NOP,
                        const util::Version &version_ = util::Version ()) :
                        name (name_),
                        op (op_),
                        version (version_) {}

                    /// \brief
                    /// ctor.
                    /// Constraint syntax is [[op]version]. Everything is optional but
                    /// if anything is provided, version better be one of them.
                    /// Ex:
                    /// constraint -> 1.4.5 - We need version 1.4.5 (same as = 1.4.5).
                    ///            -> >= 1.4.5 - Any version above and including 1.4.5 is acceptable.
                    ///            -> < 1.4.5 - We're only compatible with versions before 1.4.5.
                    ///            -> != 1.4.5 - Version 1.4.5 has a known security flaw.
                    /// \param[in] name_ Package name.
                    /// \param[in] constraint Optional properly formatted (see above) version constraint.
                    static Constraint Parse (const std::string &requirement);

                    bool MatchesRequirement (const Constraint &requirement) const;
                };
                std::vector<Constraint> requires_public;
                std::vector<Constraint> requires_private;
                std::vector<Constraint> conflicts;
                std::vector<Constraint> provides;
                std::vector<std::string> libs_public;
                std::vector<std::string> libs_private;
                std::vector<std::string> cflags_public;
                std::vector<std::string> cflags_private;
                // Postprocessed libs and cflags.
                std::vector<std::string> include_directories;      // -I
                std::vector<std::string> preprocessor_definitions; // -D
                std::vector<std::string> library_directories;      // -L
                std::vector<std::string> framework_directories;    // -F
                std::vector<std::string> library_names;            // -l
                std::vector<std::string> framework_names;          // -framework
                std::vector<std::string> other_cflags;             // Other compiler flags
                std::vector<std::string> other_libs;               // Other linker flags (e.g. -pthread)

                int CompareVersion (const util::Version &targetVersion) const;
                bool SatisfiesConstraint (
                    util::Version::OP op,
                    const util::Version &targetVersion) const;
                bool MatchesRequirement (const Constraint &requirement) const;

            private:
                explicit Package (const std::string &path_) :
                    path (path_),
                    fileName (util::Path (path).GetFileName ()) {}

                friend struct PackageResolver;
            };

            inline bool operator == (
                    const Package::Constraint &constraint1,
                    const Package::Constraint &constraint2) {
                return constraint1.name == constraint2.name &&
                    constraint1.op == constraint2.op &&
                    constraint1.version == constraint2.version;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Package_h)
