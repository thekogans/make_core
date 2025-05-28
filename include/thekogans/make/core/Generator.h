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

#if !defined (__thekogans_make_core_Generator_h)
#define __thekogans_make_core_Generator_h

#include <memory>
#include <string>
#include <list>
#include <map>
#include "pugixml/pugixml.hpp"
#include "thekogans/util/Types.h"
#include "thekogans/util/DynamicCreatable.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            /// \struct Generator Generator.h thekogans/make/Generator.h
            ///
            /// \brief
            /// Base class used to represent an abstract build system generator.

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Generator : public util::DynamicCreatable {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE_ABSTRACT_BASE (Generator)

                /// \struct Signer::Parameters Signer.h thekogans/crypto/Signer.h
                ///
                /// \brief
                /// Pass these parameters to DynamicCreatable::CreateType to
                /// parametarize the new instance.
                struct Parameters : public util::DynamicCreatable::Parameters {
                    /// \brief
                    /// Private key.
                    bool rootProject;

                    /// \brief
                    /// ctor.
                    /// \param[in] privateKey_ Private key.
                    /// \param[in] messageDigest_ Message digest.
                    Parameters (bool rootProject_) :
                        rootProject (rootProject_) {}

                /// \brief
                /// Apply the encapsulated parameters to the passed in instance.
                /// \param[in] dynamicCreatable Signer instance to apply the
                /// encapsulated parameters to.
                virtual void Apply (DynamicCreatable::SharedPtr dynamicCreatable) override {
                    Generator::SharedPtr generator = dynamicCreatable;
                    if (generator != nullptr) {
                        generator->Init (rootProject);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                            THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                    }
                }
            };

            protected:
                /// \brief
                /// true == root project, false == child project.
                bool rootProject;

            public:
                /// \brief
                /// ctor.
                /// \param[in] rootProject_ true == root project, false == child project.
                Generator (bool rootProject_ = true) :
                    rootProject (rootProject_) {}

                static SharedPtr CreateGenerator (
                    const std::string &type,
                    bool rootProject);

                virtual void Init (bool rootProject_) {
                    rootProject = rootProject_;
                }

                /// \brief
                /// Generate a build system.
                /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
                /// \param[in] config Debug or Release.
                /// \param[in] type Static or Shared.
                /// \param[in] generateDependencies true = Generate Dependencies.
                /// \param[in] force true = Don't bother checking
                /// the timestamps and force generation.
                /// \return true = Generated the build system,
                /// false = The build system was up to date.
                virtual bool Generate (
                    const std::string &project_root,
                    const std::string &config,
                    const std::string &type,
                    bool generateDependencies,
                    bool force) = 0;

                /// \brief
                /// Delete a build system.
                /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
                /// \param[in] config Debug or Release.
                /// \param[in] type Static or Shared.
                /// \param[in] deleteDependencies true = Delete Dependencies.
                virtual void Delete (
                    const std::string &project_root,
                    const std::string &config,
                    const std::string &type,
                    bool deleteDependencies) = 0;
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Generator_h)
