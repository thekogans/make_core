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

#include <string>
#include <vector>
#include <sstream>
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/ProjectDependency.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                std::string FormatFeatures (const std::vector<std::string> &features) {
                    std::string featureList;
                    for (const auto &feature : features) {
                        featureList += feature +  ", ";
                    }
                    return featureList;
                }
            }

            ProjectDependency::ProjectDependency (
                    const thekogans_make &dependent,
                    bool isPrivate,
                    const std::string &organization_,
                    const std::string &name_,
                    const std::string &branch_,
                    const std::string &version_,
                    const std::string &example_,
                    const std::string &config_,
                    const std::string &type_,
                    const std::set<std::string> &features_) :
                    Dependency (dependent, isPrivate),
                    organization (organization_),
                    name (name_),
                    branch (branch_),
                    version (version_),
                    example (example_),
                    config (config_),
                    type (type_),
                    features (features_),
                    config_ (
                        thekogans_make::GetConfig (
                            GetProjectRoot (),
                            GetConfigFile (),
                            GetGenerator (),
                            GetConfig (),
                            GetType ())) {
                if (Project::Find (organization, name, branch, version, example)) {
                    if (!features.empty ()) {
                        std::vector<std::string> missingFeatures;
                        for (const auto &feature : features) {
                            if (this->config_.features.find (feature) == this->config_.features.end ()) {
                                missingFeatures.push_back (feature);
                            }
                        }
                        if (!missingFeatures.empty ()) {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Project dependency %s is missing required feature(s): %s",
                                MakePath (GetProjectRoot (), GetConfigFile ()).c_str (),
                                FormatFeatures (missingFeatures).c_str ());
                        }
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to resolve project dependency: %s",
                        MakePath (GetProjectRoot (), GetConfigFile ()).c_str ());
                }
            }

            void ProjectDependency::CollectVersions (Versions &versions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    std::string projectName =
                        GetFileName (
                            organization,
                            name,
                            std::string (),
                            std::string (),
                            std::string ());
                    if (!example.empty ()) {
                        projectName += PROJECT_EXAMPLE_SEPARATOR + example;
                    }
                    versions[projectName].insert (
                        VersionAndBranch (
                            version.empty () ? config_.GetVersion () : version, branch));
                    for (auto dependency : config_.dependencies) {
                        dependency->CollectVersions (versions);
                    }
                }
                else {
                    config_.CheckDependencies ();
                }
            }

            void ProjectDependency::SetMinVersion (
                    Versions &versions,
                    std::set<std::string> &visitedDependencies) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    std::string projectName =
                        GetFileName (
                            organization,
                            name,
                            std::string (),
                            std::string (),
                            std::string ());
                    if (!example.empty ()) {
                        projectName += PROJECT_EXAMPLE_SEPARATOR + example;
                    }
                    const VersionSet &versionSet = versions[projectName];
                    if (versionSet.size () > 1) {
                        if (visitedDependencies.insert (projectName).second) {
                            VersionSet::const_iterator it = versionSet.begin ();
                            VersionSet::const_iterator end = versionSet.end ();
                            std::string dependencyVersions = !it->second.empty () ?
                                it->second + DECORATIONS_SEPARATOR + it->first : it->first;
                            while (++it != end) {
                                dependencyVersions += ", " +
                                    (!it->second.empty () ?
                                        it->second + DECORATIONS_SEPARATOR + it->first :
                                        it->first);

                            }
                            std::cout << "WARNING: Found multiple versions for " <<
                                projectName << ": " << dependencyVersions << " (using " <<
                                (!versionSet.begin ()->second.empty () ?
                                    versionSet.begin ()->second + DECORATIONS_SEPARATOR + versionSet.begin ()->first :
                                    versionSet.begin ()->first) << ")" << std::endl;
                            std::cout.flush ();
                        }
                        if (version.empty ()) {
                            std::string floatingVersion = config_.GetVersion ();
                            if (util::Version (floatingVersion) >
                                util::Version (versionSet.begin ()->first)) {
                                branch = versionSet.begin ()->second;
                                version = versionSet.begin ()->first;
                            }
                        }
                        else {
                            branch = versionSet.begin ()->second;
                            version = versionSet.begin ()->first;
                        }
                    }
                    for (auto dependency : config_.dependencies) {
                        dependency->SetMinVersion (versions, visitedDependencies);
                    }
                }
            }

            void ProjectDependency::GetCommonPreprocessorDefinitions (std::set<std::string> &preprocessorDefinitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    std::string ORGANIZATION =
                        util::StringToUpper (SanitizeName (organization).c_str ());
                    std::string NAME =
                        util::StringToUpper (SanitizeName (name).c_str ());
                    std::string PREFIX = ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + NAME;
                    if (!example.empty ()) {
                        std::string EXAMPLE =
                            util::StringToUpper (SanitizeName (example).c_str ());
                        PREFIX += PROJECT_EXAMPLE_SEPARATOR + EXAMPLE;
                    }
                    preprocessorDefinitions.insert (PREFIX + "_CONFIG_" + GetConfig ());
                    preprocessorDefinitions.insert (PREFIX + "_TYPE_" + GetType ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetCommonPreprocessorDefinitions (preprocessorDefinitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetFeatures (std::set<std::string> &features) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    features.insert (config_.features.begin (), config_.features.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetFeatures (features);
                        }
                    }
                }
            }

            bool ProjectDependency::HaveFeature (const std::string &feature) const {
                return config_.features.find (feature) != config_.features.end ();
            }

            void ProjectDependency::GetIncludeDirectories (std::set<std::string> &include_directories) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    for (auto include_directory : config_.include_directories) {
                        if (!include_directory->isPrivate) {
                            std::string prefix =
                                MakePath (config_.project_root, include_directory->prefix);
                            for (const auto &path : include_directory->paths) {
                                include_directories.insert (MakePath (prefix, path));
                            }
                        }
                    }
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetIncludeDirectories (include_directories);
                        }
                    }
                }
            }

            void ProjectDependency::GetLibraryDirectories (std::set<std::string> & /*library_directories*/) const {
            }

            void ProjectDependency::GetFrameworkDirectories (std::set<std::string> & /*framework_directories*/) const {
            }

            void ProjectDependency::GetLinkerFlags (std::set<std::string> &linker_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    linker_flags.insert (config_.linker_flags.begin (), config_.linker_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetLinkerFlags (linker_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetLibrarianFlags (std::set<std::string> &librarian_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    librarian_flags.insert (config_.librarian_flags.begin (), config_.librarian_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetLibrarianFlags (librarian_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetMasmFlags (std::set<std::string> &masm_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    masm_flags.insert (config_.masm_flags.begin (), config_.masm_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetMasmFlags (masm_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetMasmPreprocessorDefinitions (std::set<std::string> &masm_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    masm_preprocessor_definitions.insert (
                        config_.masm_preprocessor_definitions.begin (), config_.masm_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetMasmPreprocessorDefinitions (masm_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetNasmFlags (std::set<std::string> &nasm_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    nasm_flags.insert (config_.nasm_flags.begin (), config_.nasm_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetNasmFlags (nasm_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetNasmPreprocessorDefinitions (std::set<std::string> &nasm_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    nasm_preprocessor_definitions.insert (
                        config_.nasm_preprocessor_definitions.begin (), config_.nasm_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetNasmPreprocessorDefinitions (nasm_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetCFlags (std::set<std::string> &c_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    c_flags.insert (config_.c_flags.begin (), config_.c_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetCFlags (c_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetCPreprocessorDefinitions (std::set<std::string> &c_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    c_preprocessor_definitions.insert (
                        config_.c_preprocessor_definitions.begin (), config_.c_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetCPreprocessorDefinitions (c_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetCPPFlags (std::set<std::string> &cpp_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    cpp_flags.insert (config_.cpp_flags.begin (), config_.cpp_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetCPPFlags (cpp_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetCPPPreprocessorDefinitions (std::set<std::string> &cpp_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    cpp_preprocessor_definitions.insert (
                        config_.cpp_preprocessor_definitions.begin (), config_.cpp_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetCPPPreprocessorDefinitions (cpp_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetObjectiveCFlags (std::set<std::string> &objective_c_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    objective_c_flags.insert (config_.objective_c_flags.begin (), config_.objective_c_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetObjectiveCFlags (objective_c_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetObjectiveCPreprocessorDefinitions (std::set<std::string> &objective_c_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    objective_c_preprocessor_definitions.insert (
                        config_.objective_c_preprocessor_definitions.begin (), config_.objective_c_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetObjectiveCPreprocessorDefinitions (objective_c_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetObjectiveCPPFlags (std::set<std::string> &objective_cpp_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    objective_cpp_flags.insert (config_.objective_cpp_flags.begin (), config_.objective_cpp_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetObjectiveCPPFlags (objective_cpp_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetObjectiveCPPPreprocessorDefinitions (std::set<std::string> &objective_cpp_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    objective_cpp_preprocessor_definitions.insert (
                        config_.objective_cpp_preprocessor_definitions.begin (), config_.objective_cpp_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetObjectiveCPPPreprocessorDefinitions (objective_cpp_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetRCFlags (std::set<std::string> &rc_flags) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    rc_flags.insert (config_.rc_flags.begin (), config_.rc_flags.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetRCFlags (rc_flags);
                        }
                    }
                }
            }

            void ProjectDependency::GetRCPreprocessorDefinitions (std::set<std::string> &rc_preprocessor_definitions) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    rc_preprocessor_definitions.insert (
                        config_.rc_preprocessor_definitions.begin (), config_.rc_preprocessor_definitions.end ());
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetRCPreprocessorDefinitions (rc_preprocessor_definitions);
                        }
                    }
                }
            }

            void ProjectDependency::GetLinkLibraries (std::vector<std::string> &link_libraries) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    if (config_.HasGoal ()) {
                        link_libraries.push_back (config_.GetProjectLinkLibrary ());
                    }
                    for (auto dependency : config_.dependencies) {
                        if (!dependency->IsPrivate () || GetType () == TYPE_STATIC) {
                            dependency->GetLinkLibraries (link_libraries);
                        }
                    }
                }
            }

            void ProjectDependency::GetSharedLibraries (std::set<std::string> &shared_libraries) const {
                if (config_.project_type == PROJECT_TYPE_LIBRARY) {
                    if (GetType () == TYPE_SHARED && config_.HasGoal ()) {
                        shared_libraries.insert (config_.GetProjectGoal ());
                    }
                    for (auto dependency : config_.dependencies) {
                        dependency->GetSharedLibraries (shared_libraries);
                    }
                }
            }

            // This method is used by the Installer. Since toolchain has no need for
            // branches or examples, we ommit these attributes.
            std::string ProjectDependency::ToString (util::ui32 indentationLevel) const {
                util::Attributes attributes;
                if (isPrivate) {
                    attributes.push_back (util::Attribute (thekogans_make::ATTR_PRIVATE, isPrivate ? VALUE_YES : VALUE_NO));
                }
                attributes.push_back (util::Attribute (thekogans_make::ATTR_ORGANIZATION, organization));
                attributes.push_back (util::Attribute (thekogans_make::ATTR_NAME, name));
                // if (!branch.empty ()) {
                //     attributes.push_back (util::Attribute (thekogans_make::ATTR_BRANCH, branch));
                // }
                attributes.push_back (
                    util::Attribute (thekogans_make::ATTR_VERSION, version.empty () ?
                        config_.GetVersion () : version));
                // if (!example.empty ()) {
                //     attributes.push_back (util::Attribute (thekogans_make::ATTR_EXAMPLE, example));
                // }
                if (!config.empty ()) {
                    attributes.push_back (util::Attribute (thekogans_make::ATTR_CONFIG, config));
                }
                if (!type.empty ()) {
                    attributes.push_back (util::Attribute (thekogans_make::ATTR_TYPE, type));
                }
                return util::OpenTag (indentationLevel, thekogans_make::TAG_TOOLCHAIN, attributes, true, true);
            }

            void ProjectDependency::ListDependencies (util::ui32 indentationLevel) const {
                std::cout <<
                    std::string (indentationLevel * 2, ' ') <<
                    MakePath (GetProjectRoot (), GetConfigFile ()) << std::endl;
                std::cout.flush ();
                for (auto dependency : config_.dependencies) {
                    dependency->ListDependencies (indentationLevel + 1);
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
