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

#if !defined (__thekogans_make_core_ProjectDependency_h)
#define __thekogans_make_core_ProjectDependency_h

#include <vector>
#include <string>
#include <set>
#include "thekogans/make/core/Project.h"
#include "thekogans/make/core/thekogans_make.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct ProjectDependency : public thekogans_make::Dependency {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (ProjectDependency)

                std::string organization;
                std::string name;
                mutable std::string branch;
                mutable std::string version;
                std::string example;
                std::string config;
                std::string type;
                std::set<std::string> features;
                const thekogans_make &config_;

                ProjectDependency (
                    const thekogans_make &dependent,
                    bool isPrivate,
                    const std::string &organization_,
                    const std::string &name_,
                    const std::string &branch_,
                    const std::string &version_,
                    const std::string &example_,
                    const std::string &config_,
                    const std::string &type_,
                    const std::set<std::string> &features_);

                virtual std::string GetOrganization () const override {
                    return organization;
                }
                virtual std::string GetName () const override {
                    return name;
                }

                virtual std::string GetProjectRoot () const override {
                    return Project::GetRoot (organization, name, branch, version, example);
                }

                virtual std::string GetConfigFile () const override {
                    return THEKOGANS_MAKE_XML;
                }

                virtual std::string GetGenerator () const override {
                    return dependent.generator;
                }

                virtual std::string GetConfig () const override {
                    return !config.empty () ? config : dependent.config;
                }

                virtual std::string GetType () const override {
                    return !type.empty () ? type : dependent.type;
                }

                virtual bool EquivalentTo (const Dependency &dependency) const override {
                    return dependency.GetProjectRoot () == GetProjectRoot ();
                }

                virtual void CollectVersions (Versions &versions) const override;
                virtual void SetMinVersion (
                    Versions &versions,
                    std::set<std::string> &visitedDependencies) const override;

                virtual void GetCommonPreprocessorDefinitions (
                    std::set<std::string> &preprocessorDefinitions) const override;
                virtual void GetFeatures (std::set<std::string> &features) const override;
                virtual bool HaveFeature (const std::string &feature) const override;

                virtual void GetIncludeDirectories (std::set<std::string> &include_directories) const override;
                virtual void GetLibraryDirectories (std::set<std::string> & /*library_directories*/) const override;
                virtual void GetFrameworkDirectories (std::set<std::string> & /*framework_directories*/) const override;

                virtual void GetLinkerFlags (std::set<std::string> &linker_flags) const override;
                virtual void GetLibrarianFlags (std::set<std::string> &librarian_flags) const override;
                virtual void GetMasmFlags (std::set<std::string> &masm_flags) const override;
                virtual void GetMasmPreprocessorDefinitions (std::set<std::string> &masm_preprocessor_definitions) const override;
                virtual void GetNasmFlags (std::set<std::string> &nasm_flags) const override;
                virtual void GetNasmPreprocessorDefinitions (std::set<std::string> &nasm_preprocessor_definitions) const override;
                virtual void GetCFlags (std::set<std::string> &c_flags) const override;
                virtual void GetCPreprocessorDefinitions (std::set<std::string> &c_preprocessor_definitions) const override;
                virtual void GetCPPFlags (std::set<std::string> &cpp_flags) const override;
                virtual void GetCPPPreprocessorDefinitions (std::set<std::string> &cpp_preprocessor_definitions) const override;
                virtual void GetObjectiveCFlags (std::set<std::string> &objective_c_flags) const override;
                virtual void GetObjectiveCPreprocessorDefinitions (std::set<std::string> &objective_c_preprocessor_definitions) const override;
                virtual void GetObjectiveCPPFlags (std::set<std::string> &objective_cpp_flags) const override;
                virtual void GetObjectiveCPPPreprocessorDefinitions (std::set<std::string> &objective_cpp_preprocessor_definitions) const override;
                virtual void GetRCFlags (std::set<std::string> &rc_flags) const override;
                virtual void GetRCPreprocessorDefinitions (std::set<std::string> &rc_preprocessor_definitions) const override;

                virtual void GetLinkLibraries (std::set<std::string> &link_libraries) const override;
                virtual void GetSharedLibraries (std::set<std::string> &shared_libraries) const override;

                virtual bool IsInstalled () const override {
                    return Project::IsInstalled (organization, name, branch, version, example);
                }

                // This method is used by the Installer. Since toolchain has no need for
                // branches or examples, we ommit these attributes.
                virtual std::string ToString (util::ui32 indentationLevel = 0) const override;

                virtual void ListDependencies (util::ui32 indentationLevel = 0) const override;
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_ProjectDependency_h)
