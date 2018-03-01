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

#include <algorithm>
#include <regex>
#include <sstream>
#include "thekogans/util/Types.h"
#include "thekogans/util/Version.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Buffer.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/ByteSwap.h"
#include "thekogans/util/Exception.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/make/core/Parser.h"
#include "thekogans/make/core/Function.h"
#include "thekogans/make/core/Project.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Version.h"
#include "thekogans/make/core/thekogans_make.h"

namespace thekogans {
    namespace make {
        namespace core {

            const char * const thekogans_make::ATTR_ORGANIZATION = "organization";
            const char * const thekogans_make::ATTR_PROJECT = "project";
            const char * const thekogans_make::ATTR_PROJECT_TYPE = "project_type";
            const char * const thekogans_make::ATTR_MAJOR_VERSION = "major_version";
            const char * const thekogans_make::ATTR_MINOR_VERSION = "minor_version";
            const char * const thekogans_make::ATTR_PATCH_VERSION = "patch_version";
            const char * const thekogans_make::ATTR_NAMING_CONVENTION = "naming_convention";
            const char * const thekogans_make::ATTR_BUILD_CONFIG = "build_config";
            const char * const thekogans_make::ATTR_BUILD_TYPE = "build_type";
            const char * const thekogans_make::ATTR_GUID = "guid";
            const char * const thekogans_make::ATTR_SCHEMA_VERSION = "schema_version";
            const char * const thekogans_make::ATTR_CONDITION = "condition";
            const char * const thekogans_make::ATTR_PREFIX = "prefix";
            const char * const thekogans_make::ATTR_INSTALL = "install";
            const char * const thekogans_make::ATTR_NAME = "name";
            const char * const thekogans_make::ATTR_VALUE = "value";
            const char * const thekogans_make::ATTR_BRANCH = "branch";
            const char * const thekogans_make::ATTR_VERSION = "version";
            const char * const thekogans_make::ATTR_EXAMPLE = "example";
            const char * const thekogans_make::ATTR_CONFIG = "config";
            const char * const thekogans_make::ATTR_TYPE = "type";
            const char * const thekogans_make::ATTR_FLAGS = "flags";

            const char * const thekogans_make::TAG_THEKOGANS_MAKE = "thekogans_make";
            const char * const thekogans_make::TAG_CONSTANTS = "constants";
            const char * const thekogans_make::TAG_CONSTANT = "constant";
            const char * const thekogans_make::TAG_FEATURES = "features";
            const char * const thekogans_make::TAG_FEATURE = "feature";
            const char * const thekogans_make::TAG_PLUGIN_HOSTS = "plugin_hosts";
            const char * const thekogans_make::TAG_DEPENDENCIES = "dependencies";
            const char * const thekogans_make::TAG_DEPENDENCY = "dependency";
            const char * const thekogans_make::TAG_PROJECT = "project";
            const char * const thekogans_make::TAG_TOOLCHAIN = "toolchain";
            const char * const thekogans_make::TAG_LIBRARY = "library";
            const char * const thekogans_make::TAG_FRAMEWORK = "framework";
            const char * const thekogans_make::TAG_SYSTEM = "system";
            const char * const thekogans_make::TAG_INCLUDE_DIRECTORIES = "include_directories";
            const char * const thekogans_make::TAG_INCLUDE_DIRECTORY = "include_directory";
            const char * const thekogans_make::TAG_PREPROCESSOR_DEFINITIONS = "preprocessor_definitions";
            const char * const thekogans_make::TAG_PREPROCESSOR_DEFINITION = "preprocessor_definition";
            const char * const thekogans_make::TAG_LINKER_FLAGS = "linker_flags";
            const char * const thekogans_make::TAG_LINKER_FLAG = "linker_flag";
            const char * const thekogans_make::TAG_LIBRARIAN_FLAGS = "librarian_flags";
            const char * const thekogans_make::TAG_LIBRARIAN_FLAG = "librarian_flag";
            const char * const thekogans_make::TAG_LINK_LIBRARIES = "link_libraries";
            const char * const thekogans_make::TAG_LINK_LIBRARY = "link_library";
            const char * const thekogans_make::TAG_MASM_FLAGS = "masm_flags";
            const char * const thekogans_make::TAG_MASM_FLAG = "masm_flag";
            const char * const thekogans_make::TAG_MASM_PREPROCESSOR_DEFINITIONS = "masm_preprocessor_definitions";
            const char * const thekogans_make::TAG_MASM_PREPROCESSOR_DEFINITION = "masm_preprocessor_definition";
            const char * const thekogans_make::TAG_MASM_HEADERS = "masm_headers";
            const char * const thekogans_make::TAG_MASM_HEADER = "masm_header";
            const char * const thekogans_make::TAG_MASM_SOURCES = "masm_sources";
            const char * const thekogans_make::TAG_MASM_SOURCE = "masm_source";
            const char * const thekogans_make::TAG_MASM_TESTS = "masm_tests";
            const char * const thekogans_make::TAG_MASM_TEST = "masm_test";
            const char * const thekogans_make::TAG_NASM_FLAGS = "nasm_flags";
            const char * const thekogans_make::TAG_NASM_FLAG = "nasm_flag";
            const char * const thekogans_make::TAG_NASM_PREPROCESSOR_DEFINITIONS = "nasm_preprocessor_definitions";
            const char * const thekogans_make::TAG_NASM_PREPROCESSOR_DEFINITION = "nasm_preprocessor_definition";
            const char * const thekogans_make::TAG_NASM_HEADERS = "nasm_headers";
            const char * const thekogans_make::TAG_NASM_HEADER = "nasm_header";
            const char * const thekogans_make::TAG_NASM_SOURCES = "nasm_sources";
            const char * const thekogans_make::TAG_NASM_SOURCE = "nasm_source";
            const char * const thekogans_make::TAG_NASM_TESTS = "nasm_tests";
            const char * const thekogans_make::TAG_NASM_TEST = "nasm_test";
            const char * const thekogans_make::TAG_C_FLAGS = "c_flags";
            const char * const thekogans_make::TAG_C_FLAG = "c_flag";
            const char * const thekogans_make::TAG_C_PREPROCESSOR_DEFINITIONS = "c_preprocessor_definitions";
            const char * const thekogans_make::TAG_C_PREPROCESSOR_DEFINITION = "c_preprocessor_definition";
            const char * const thekogans_make::TAG_C_HEADERS = "c_headers";
            const char * const thekogans_make::TAG_C_HEADER = "c_header";
            const char * const thekogans_make::TAG_C_SOURCES = "c_sources";
            const char * const thekogans_make::TAG_C_SOURCE = "c_source";
            const char * const thekogans_make::TAG_C_TESTS = "c_tests";
            const char * const thekogans_make::TAG_C_TEST = "c_test";
            const char * const thekogans_make::TAG_CPP_FLAGS = "cpp_flags";
            const char * const thekogans_make::TAG_CPP_FLAG = "cpp_flag";
            const char * const thekogans_make::TAG_CPP_PREPROCESSOR_DEFINITIONS = "cpp_preprocessor_definitions";
            const char * const thekogans_make::TAG_CPP_PREPROCESSOR_DEFINITION = "cpp_preprocessor_definition";
            const char * const thekogans_make::TAG_CPP_HEADERS = "cpp_headers";
            const char * const thekogans_make::TAG_CPP_HEADER = "cpp_header";
            const char * const thekogans_make::TAG_CPP_SOURCES = "cpp_sources";
            const char * const thekogans_make::TAG_CPP_SOURCE = "cpp_source";
            const char * const thekogans_make::TAG_CPP_TESTS = "cpp_tests";
            const char * const thekogans_make::TAG_CPP_TEST = "cpp_test";
            const char * const thekogans_make::TAG_OBJECTIVE_C_FLAGS = "objective_c_flags";
            const char * const thekogans_make::TAG_OBJECTIVE_C_FLAG = "objective_c_flag";
            const char * const thekogans_make::TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITIONS = "objective_c_preprocessor_definitions";
            const char * const thekogans_make::TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITION = "objective_c_preprocessor_definition";
            const char * const thekogans_make::TAG_OBJECTIVE_C_HEADERS = "objective_c_headers";
            const char * const thekogans_make::TAG_OBJECTIVE_C_HEADER = "objective_c_header";
            const char * const thekogans_make::TAG_OBJECTIVE_C_SOURCES = "objective_c_sources";
            const char * const thekogans_make::TAG_OBJECTIVE_C_SOURCE = "objective_c_source";
            const char * const thekogans_make::TAG_OBJECTIVE_C_TESTS = "objective_c_tests";
            const char * const thekogans_make::TAG_OBJECTIVE_C_TEST = "objective_c_test";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_FLAGS = "objective_cpp_flags";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_FLAG = "objective_cpp_flag";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITIONS = "objective_cpp_preprocessor_definitions";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITION = "objective_cpp_preprocessor_definition";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_HEADERS = "objective_cpp_headers";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_HEADER = "objective_cpp_header";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_SOURCES = "objective_cpp_sources";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_SOURCE = "objective_cpp_source";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_TESTS = "objective_cpp_tests";
            const char * const thekogans_make::TAG_OBJECTIVE_CPP_TEST = "objective_cpp_test";
            const char * const thekogans_make::TAG_REGEX = "regex";
            const char * const thekogans_make::TAG_CUSTOM_BUILD = "custom_build";
            const char * const thekogans_make::TAG_OUTPUTS = "outputs";
            const char * const thekogans_make::TAG_OUTPUT = "output";
            const char * const thekogans_make::TAG_MESSAGE = "message";
            const char * const thekogans_make::TAG_RECIPE = "recipe";
            const char * const thekogans_make::TAG_RESOURCES = "resources";
            const char * const thekogans_make::TAG_RESOURCE = "resource";
            const char * const thekogans_make::TAG_RC_FLAGS = "rc_flags";
            const char * const thekogans_make::TAG_RC_FLAG = "rc_flag";
            const char * const thekogans_make::TAG_RC_PREPROCESSOR_DEFINITIONS = "rc_preprocessor_definitions";
            const char * const thekogans_make::TAG_RC_PREPROCESSOR_DEFINITION = "rc_preprocessor_definition";
            const char * const thekogans_make::TAG_RC_SOURCES = "rc_sources";
            const char * const thekogans_make::TAG_RC_SOURCE = "rc_source";
            const char * const thekogans_make::TAG_SUBSYSTEM = "subsystem";
            const char * const thekogans_make::TAG_DEF_FILE = "def_file";
            const char * const thekogans_make::TAG_IF = "if";
            const char * const thekogans_make::TAG_CHOOSE = "choose";
            const char * const thekogans_make::TAG_WHEN = "when";
            const char * const thekogans_make::TAG_OTHERWISE = "otherwise";
            const char * const thekogans_make::TAG_INFO = "info";
            const char * const thekogans_make::TAG_WARNING = "warning";
            const char * const thekogans_make::TAG_ERROR = "error";

            const char * const thekogans_make::VAR_PROJECT_ROOT = "project_root";
            const char * const thekogans_make::VAR_CONFIG_FILE = "config_file";
            const char * const thekogans_make::VAR_GENERATOR = "generator";
            const char * const thekogans_make::VAR_CONFIG = "config";
            const char * const thekogans_make::VAR_TYPE = "type";
            const char * const thekogans_make::VAR_PROJECT_DIRECTORY = "project_directory";
            const char * const thekogans_make::VAR_BUILD_DIRECTORY = "build_directory";
            const char * const thekogans_make::VAR_VERSION = "version";
            const char * const thekogans_make::VAR_LINK_LIBRARY_SUFFIX = "link_library_suffix";

            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::FileList::File)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::FileList::File::CustomBuild)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::IncludeDirectories)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::LinkLibraries)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::MASMHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::MASMSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::MASMTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::NASMHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::NASMSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::NASMTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CPPHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CPPSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::CPPTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCPPHeaders)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCPPSources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::ObjectiveCPPTests)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::Resources)
            THEKOGANS_UTIL_IMPLEMENT_HEAP (thekogans_make::RCSources)

            namespace {
                std::string FormatFeatures (const std::set<std::string> &features) {
                    std::string featureList;
                    if (!features.empty ()) {
                        std::set<std::string>::const_iterator it = features.begin ();
                        featureList = *it++;
                        for (std::set<std::string>::const_iterator
                                end = features.end (); it != end; ++it) {
                            featureList += ", " + *it;
                        }
                    }
                    return featureList;
                }

                struct ProjectDependency : public thekogans_make::Dependency {
                    std::string organization;
                    std::string name;
                    mutable std::string branch;
                    mutable std::string version;
                    std::string example;
                    std::string config;
                    std::string type;
                    std::set<std::string> features;
                    const thekogans_make &dependent;

                    ProjectDependency (
                            const std::string &organization_,
                            const std::string &name_,
                            const std::string &branch_,
                            const std::string &version_,
                            const std::string &example_,
                            const std::string &config_,
                            const std::string &type_,
                            const std::set<std::string> &features_,
                            const thekogans_make &dependent_) :
                            organization (organization_),
                            name (name_),
                            branch (branch_),
                            version (version_),
                            example (example_),
                            config (config_),
                            type (type_),
                            features (features_),
                            dependent (dependent_) {
                        if (Project::Find (organization, name, branch, version, example)) {
                            if (!features.empty ()) {
                                const thekogans_make &config =
                                    thekogans_make::GetConfig (
                                        GetProjectRoot (),
                                        GetConfigFile (),
                                        GetGenerator (),
                                        GetConfig (),
                                        GetType ());
                                std::set<std::string> missingFeatures;
                                for (std::set<std::string>::const_iterator
                                        it = features.begin (),
                                        end = features.end (); it != end; ++it) {
                                    if (config.features.find (*it) == config.features.end ()) {
                                        missingFeatures.insert (*it);
                                    }
                                }
                                if (!missingFeatures.empty ()) {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Dependency %s is missing required features: %s",
                                        MakePath (GetProjectRoot (), GetConfigFile ()).c_str (),
                                        FormatFeatures (features).c_str ());
                                }
                            }
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Unable to resolve dependency: %s",
                                MakePath (GetProjectRoot (), GetConfigFile ()).c_str ());
                        }
                    }

                    virtual const thekogans_make &GetDependent () const {
                        return dependent;
                    }

                    virtual std::string GetProjectRoot () const {
                        return Project::GetRoot (organization, name, branch, version, example);
                    }

                    virtual std::string GetConfigFile () const {
                        return THEKOGANS_MAKE_XML;
                    }

                    virtual std::string GetGenerator () const {
                        return dependent.generator;
                    }

                    virtual std::string GetConfig () const {
                        return !config.empty () ? config : dependent.config;
                    }

                    virtual std::string GetType () const {
                        return !type.empty () ? type : dependent.type;
                    }

                    virtual bool EquivalentTo (const Dependency &dependency) const {
                        const ProjectDependency *projectDependency =
                            dynamic_cast<const ProjectDependency *> (&dependency);
                        return projectDependency != 0 &&
                            projectDependency->GetProjectRoot () == GetProjectRoot ();
                    }

                    virtual void CollectVersions (Versions &versions) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_PROGRAM ||
                                config.project_type == PROJECT_TYPE_PLUGIN) {
                            config.CheckDependencies ();
                        }
                        else {
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
                                VersionAndBranch (version.empty () ? config.GetVersion () : version, branch));
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->CollectVersions (versions);
                            }
                        }
                    }

                    virtual void SetMinVersion (
                            Versions &versions,
                            std::set<std::string> &visitedDependencies) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
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
                                if (visitedDependencies.find (projectName) == visitedDependencies.end ()) {
                                    visitedDependencies.insert (projectName);
                                    VersionSet::const_iterator it = versionSet.begin ();
                                    VersionSet::const_iterator end = versionSet.end ();
                                    std::string dependencyVersions = !it->second.empty () ?
                                        it->second + DECORATIONS_SEPARATOR + it->first : it->first;
                                    while (++it != end) {
                                        dependencyVersions += ", " +
                                            (!it->second.empty () ? it->second + DECORATIONS_SEPARATOR + it->first : it->first);

                                    }
                                    std::cout << "WARNING: Found multiple versions for " <<
                                        projectName << ": " << dependencyVersions << " (using " <<
                                        (!versionSet.begin ()->second.empty () ?
                                            versionSet.begin ()->second + DECORATIONS_SEPARATOR + versionSet.begin ()->first :
                                            versionSet.begin ()->first) << ")" << std::endl;
                                    std::cout.flush ();
                                }
                                if (version.empty ()) {
                                    std::string floatingVersion = config.GetVersion ();
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
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->SetMinVersion (versions, visitedDependencies);
                            }
                        }
                    }

                    virtual void GetFeatures (
                            std::set<std::string> &features) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            for (std::set<std::string>::const_iterator
                                    it = config.features.begin (),
                                    end = config.features.end (); it != end; ++it) {
                                features.insert (*it);
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetFeatures (features);
                            }
                        }
                    }

                    virtual void GetIncludeDirectories (
                            std::set<std::string> &include_directories) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            for (std::list<thekogans_make::IncludeDirectories::Ptr>::const_iterator
                                    it = config.include_directories.begin (),
                                    end = config.include_directories.end (); it != end; ++it) {
                                if ((*it)->install) {
                                    std::string prefix =
                                        MakePath (config.project_root, (*it)->prefix);
                                    for (std::list<std::string>::const_iterator
                                            jt = (*it)->paths.begin (),
                                            end = (*it)->paths.end (); jt != end; ++jt) {
                                        include_directories.insert (MakePath (prefix, *jt));
                                    }
                                }
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetIncludeDirectories (include_directories);
                            }
                        }
                    }

                    virtual void GetLinkLibraries (
                            std::list<std::string> &link_libraries) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            if (config.HasGoal ()) {
                                link_libraries.push_back (config.GetProjectLinkLibrary ());
                            }
                            if (config.type == TYPE_STATIC) {
                                for (std::list<Dependency::Ptr>::const_iterator
                                        it = config.dependencies.begin (),
                                        end = config.dependencies.end (); it != end; ++it) {
                                    (*it)->GetLinkLibraries (link_libraries);
                                }
                            }
                        }
                    }

                    virtual void GetSharedLibraries (std::set<std::string> &shared_libraries) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            if (GetType () == TYPE_SHARED && config.HasGoal ()) {
                                shared_libraries.insert (config.GetProjectGoal ());
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetSharedLibraries (shared_libraries);
                            }
                        }
                    }

                    virtual bool IsInstalled () const {
                        return Project::IsInstalled (organization, name, branch, version, example);
                    }

                    virtual std::string ToString (util::ui32 indentationLevel = 0) const {
                        util::Attributes attributes;
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_ORGANIZATION,
                                organization));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_NAME,
                                name));
                        attributes.push_back (
                            util::Attribute (thekogans_make::ATTR_VERSION, version.empty () ?
                                thekogans_make::GetConfig (
                                    GetProjectRoot (),
                                    GetConfigFile (),
                                    GetGenerator (),
                                    GetConfig (),
                                    GetType ()).GetVersion () :
                                version));
                        if (!config.empty ()) {
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONFIG,
                                    config));
                        }
                        if (!type.empty ()) {
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_TYPE,
                                    type));
                        }
                        return util::OpenTag (
                            indentationLevel,
                            thekogans_make::TAG_TOOLCHAIN,
                            attributes,
                            true,
                            true);
                    }

                    virtual void ListDependencies (util::ui32 indentationLevel = 0) const {
                        std::cout <<
                            std::string (indentationLevel * 2, ' ') <<
                            MakePath (GetProjectRoot (), GetConfigFile ()) << std::endl;
                        std::cout.flush ();
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        for (std::list<Dependency::Ptr>::const_iterator
                                it = config.dependencies.begin (),
                                end = config.dependencies.end (); it != end; ++it) {
                            (*it)->ListDependencies (indentationLevel + 1);
                        }
                    }
                };

                struct ToolchainDependency : public thekogans_make::Dependency {
                    std::string organization;
                    std::string name;
                    mutable std::string version;
                    std::string config;
                    std::string type;
                    std::set<std::string> features;
                    const thekogans_make &dependent;

                    ToolchainDependency (
                            const std::string &organization_,
                            const std::string &name_,
                            const std::string &version_,
                            const std::string &config_,
                            const std::string &type_,
                            const std::set<std::string> &features_,
                            const thekogans_make &dependent_) :
                            organization (organization_),
                            name (name_),
                            version (version_),
                            config (config_),
                            type (type_),
                            features (features_),
                            dependent (dependent_) {
                        if (Toolchain::Find (organization, name, version)) {
                            if (!features.empty ()) {
                                const thekogans_make &config =
                                    thekogans_make::GetConfig (
                                        GetProjectRoot (),
                                        GetConfigFile (),
                                        GetGenerator (),
                                        GetConfig (),
                                        GetType ());
                                std::set<std::string> missingFeatures;
                                for (std::set<std::string>::const_iterator
                                        it = features.begin (),
                                        end = features.end (); it != end; ++it) {
                                    if (config.features.find (*it) == config.features.end ()) {
                                        missingFeatures.insert (*it);
                                    }
                                }
                                if (!missingFeatures.empty ()) {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Dependency %s is missing required features: %s",
                                        MakePath (GetProjectRoot (), GetConfigFile ()).c_str (),
                                        FormatFeatures (features).c_str ());
                                }
                            }
                        }
                        else {
                            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                "Unable to resolve dependency: %s",
                                MakePath (GetProjectRoot (), GetConfigFile ()).c_str ());
                        }
                    }

                    virtual const thekogans_make &GetDependent () const {
                        return dependent;
                    }

                    virtual std::string GetProjectRoot () const {
                        return _TOOLCHAIN_DIR;
                    }

                    virtual std::string GetConfigFile () const {
                        return MakePath (
                            CONFIG_DIR,
                            GetFileName (organization, name, std::string (), version, XML_EXT));
                    }

                    virtual std::string GetGenerator () const {
                        return std::string ();
                    }

                    virtual std::string GetConfig () const {
                        return !config.empty () ? config : dependent.config;
                    }

                    virtual std::string GetType () const {
                        return !type.empty () ? type : dependent.type;
                    }

                    virtual bool EquivalentTo (const Dependency &dependency) const {
                        const ToolchainDependency *toolchainDependency =
                            dynamic_cast<const ToolchainDependency *> (&dependency);
                        return toolchainDependency != 0 &&
                            toolchainDependency->GetConfigFile () == GetConfigFile ();
                    }

                    virtual void CollectVersions (Versions &versions) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_PROGRAM ||
                                config.project_type == PROJECT_TYPE_PLUGIN) {
                            config.CheckDependencies ();
                        }
                        else {
                            std::string projectName =
                                GetFileName (
                                    organization,
                                    name,
                                    std::string (),
                                    std::string (),
                                    std::string ());
                            versions[projectName].insert (VersionAndBranch (version, std::string ()));
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->CollectVersions (versions);
                            }
                        }
                    }

                    virtual void SetMinVersion (
                            Versions &versions,
                            std::set<std::string> &visitedDependencies) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            std::string projectName =
                                GetFileName (
                                    organization,
                                    name,
                                    std::string (),
                                    std::string (),
                                    std::string ());
                            const VersionSet &versionSet = versions[projectName];
                            if (versionSet.size () > 1) {
                                if (visitedDependencies.find (projectName) == visitedDependencies.end ()) {
                                    visitedDependencies.insert (projectName);
                                    VersionSet::const_iterator it = versionSet.begin ();
                                    VersionSet::const_iterator end = versionSet.end ();
                                    std::string dependencyVersions = it->first;
                                    while (++it != end) {
                                        dependencyVersions += ", " + it->first;
                                    }
                                    std::cout << "WARNING: Found multiple versions for " <<
                                        projectName << ": " << dependencyVersions << " (using " <<
                                        versionSet.begin ()->first << ")" << std::endl;
                                    std::cout.flush ();
                                }
                                version = versionSet.begin ()->first;
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->SetMinVersion (versions, visitedDependencies);
                            }
                        }
                    }

                    virtual void GetFeatures (
                            std::set<std::string> &features) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            for (std::set<std::string>::const_iterator
                                    it = config.features.begin (),
                                    end = config.features.end (); it != end; ++it) {
                                features.insert (*it);
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetFeatures (features);
                            }
                        }
                    }

                    virtual void GetIncludeDirectories (
                            std::set<std::string> &include_directories) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            if (!config.include_directories.empty ()) {
                                for (std::list<thekogans_make::IncludeDirectories::Ptr>::const_iterator
                                        it = config.include_directories.begin (),
                                        end = config.include_directories.end (); it != end; ++it) {
                                    std::string prefix =
                                        MakePath (config.project_root, (*it)->prefix);
                                    for (std::list<std::string>::const_iterator
                                            jt = (*it)->paths.begin (),
                                            end = (*it)->paths.end (); jt != end; ++jt) {
                                        include_directories.insert (MakePath (prefix, *jt));
                                    }
                                }
                            }
                            else {
                                std::string include_directory = config.GetToolchainIncludeDirectory ();
                                if (util::Path (ToSystemPath (include_directory)).Exists ()) {
                                    include_directories.insert (include_directory);
                                }
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetIncludeDirectories (include_directories);
                            }
                        }
                    }

                    virtual void GetLinkLibraries (
                            std::list<std::string> &link_libraries) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            if (!config.link_libraries.empty ()) {
                                for (std::list<thekogans_make::LinkLibraries::Ptr>::const_iterator
                                        it = config.link_libraries.begin (),
                                        end = config.link_libraries.end (); it != end; ++it) {
                                    std::string prefix =
                                        MakePath (config.project_root, (*it)->prefix);
                                    for (std::list<std::string>::const_iterator
                                            jt = (*it)->files.begin (),
                                            end = (*it)->files.end (); jt != end; ++jt) {
                                        link_libraries.push_back (MakePath (prefix, *jt));
                                    }
                                }
                            }
                            else {
                                std::string link_library = config.GetToolchainLinkLibrary ();
                                if (util::Path (ToSystemPath (link_library)).Exists ()) {
                                    link_libraries.push_back (link_library);
                                }
                            }
                            if (config.type == TYPE_STATIC) {
                                for (std::list<Dependency::Ptr>::const_iterator
                                        it = config.dependencies.begin (),
                                        end = config.dependencies.end (); it != end; ++it) {
                                    (*it)->GetLinkLibraries (link_libraries);
                                }
                            }
                        }
                    }

                    virtual void GetSharedLibraries (std::set<std::string> &shared_libraries) const {
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        if (config.project_type == PROJECT_TYPE_LIBRARY) {
                            if (GetType () == TYPE_SHARED) {
                                if (!config.link_libraries.empty ()) {
                                    for (std::list<thekogans_make::LinkLibraries::Ptr>::const_iterator
                                            it = config.link_libraries.begin (),
                                            end = config.link_libraries.end (); it != end; ++it) {
                                        std::string prefix =
                                            MakePath (config.project_root, (*it)->prefix);
                                        for (std::list<std::string>::const_iterator
                                                jt = (*it)->files.begin (),
                                                end = (*it)->files.end (); jt != end; ++jt) {
                                            std::string shared_library = MakePath (prefix, *jt);
                                        #if defined (TOOLCHAIN_OS_Windows)
                                            std::string::size_type dot =
                                                shared_library.find_last_of ('.');
                                            if (dot != std::string::npos) {
                                                shared_library.erase (dot + 1);
                                                shared_library += _TOOLCHAIN_SHARED_LIBRARY_SUFFIX;
                                            }
                                        #endif // defined (TOOLCHAIN_OS_Windows)
                                            shared_libraries.insert (shared_library);
                                        }
                                    }
                                }
                                else {
                                    std::string shared_library = config.GetToolchainGoal ();
                                    if (util::Path (ToSystemPath (shared_library)).Exists ()) {
                                        shared_libraries.insert (shared_library);
                                    }
                                }
                            }
                            for (std::list<Dependency::Ptr>::const_iterator
                                    it = config.dependencies.begin (),
                                    end = config.dependencies.end (); it != end; ++it) {
                                (*it)->GetSharedLibraries (shared_libraries);
                            }
                        }
                    }

                    virtual bool IsInstalled () const {
                        return Toolchain::IsInstalled (organization, name, version);
                    }

                    virtual std::string ToString (util::ui32 indentationLevel = 0) const {
                        util::Attributes attributes;
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_ORGANIZATION,
                                organization));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_NAME,
                                name));
                        attributes.push_back (
                            util::Attribute (
                                thekogans_make::ATTR_VERSION,
                                version));
                        if (!config.empty ()) {
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_CONFIG,
                                    config));
                        }
                        if (!type.empty ()) {
                            attributes.push_back (
                                util::Attribute (
                                    thekogans_make::ATTR_TYPE,
                                    type));
                        }
                        return util::OpenTag (
                            indentationLevel,
                            thekogans_make::TAG_TOOLCHAIN,
                            attributes,
                            true,
                            true);
                    }

                    virtual void ListDependencies (util::ui32 indentationLevel = 0) const {
                        std::cout <<
                            std::string (indentationLevel * 2, ' ') <<
                            MakePath (GetProjectRoot (), GetConfigFile ()) << std::endl;
                        std::cout.flush ();
                        const thekogans_make &config =
                            thekogans_make::GetConfig (
                                GetProjectRoot (),
                                GetConfigFile (),
                                GetGenerator (),
                                GetConfig (),
                                GetType ());
                        for (std::list<Dependency::Ptr>::const_iterator
                                it = config.dependencies.begin (),
                                end = config.dependencies.end (); it != end; ++it) {
                            (*it)->ListDependencies (indentationLevel + 1);
                        }
                    }
                };

                struct LibraryDependency : public thekogans_make::Dependency {
                    std::string library;
                    const thekogans_make &dependent;

                    LibraryDependency (
                        const std::string &library_,
                        const thekogans_make &dependent_) :
                        library (library_),
                        dependent (dependent_) {}

                    virtual const thekogans_make &GetDependent () const {
                        return dependent;
                    }

                    virtual std::string GetProjectRoot () const {
                        return std::string ();
                    }
                    virtual std::string GetConfigFile () const {
                        return std::string ();
                    }

                    virtual std::string GetGenerator () const {
                        return std::string ();
                    }

                    virtual std::string GetConfig () const {
                        return std::string ();
                    }

                    virtual std::string GetType () const {
                        return std::string ();
                    }

                    virtual bool EquivalentTo (const Dependency &dependency) const {
                        const LibraryDependency *libraryDependency =
                            dynamic_cast<const LibraryDependency *> (&dependency);
                        return libraryDependency != 0 &&
                            libraryDependency->library == library;
                    }

                    virtual void CollectVersions (
                            Versions & /*versions*/) const {
                    }
                    virtual void SetMinVersion (
                            Versions & /*versions*/,
                            std::set<std::string> & /*visitedDependencies*/) const {
                    }

                    virtual void  GetFeatures (
                            std::set<std::string> & /*features*/) const {
                    }

                    virtual void GetIncludeDirectories (
                            std::set<std::string> & /*include_directories*/) const {
                    }

                    virtual void GetLinkLibraries (
                            std::list<std::string> &link_libraries) const {
                    #if defined (TOOLCHAIN_OS_Windows)
                        std::string prefix;
                    #else // defined (TOOLCHAIN_OS_Windows)
                        std::string prefix = "-l";
                    #endif // defined (TOOLCHAIN_OS_Windows)
                        link_libraries.push_back (prefix + library);
                    }

                    virtual void GetSharedLibraries (
                            std::set<std::string> & /*shared_libraries*/) const {
                    }

                    virtual bool IsInstalled () const {
                        return true;
                    }

                    virtual std::string ToString (util::ui32 indentationLevel = 0) const {
                        return
                            util::OpenTag (indentationLevel, thekogans_make::TAG_LIBRARY) +
                            library +
                            util::CloseTag (0, thekogans_make::TAG_LIBRARY);
                    }

                    virtual void ListDependencies (util::ui32 indentationLevel = 0) const {
                        std::cout <<
                            std::string (indentationLevel * 2, ' ') <<
                            "library: " << library << std::endl;
                        std::cout.flush ();
                    }
                };

                struct FrameworkDependency : public thekogans_make::Dependency {
                    std::string framework;
                    const thekogans_make &dependent;

                    FrameworkDependency (
                        const std::string &framework_,
                        const thekogans_make &dependent_) :
                        framework (framework_),
                        dependent (dependent_) {}

                    virtual const thekogans_make &GetDependent () const {
                        return dependent;
                    }

                    virtual std::string GetProjectRoot () const {
                        return std::string ();
                    }
                    virtual std::string GetConfigFile () const {
                        return std::string ();
                    }

                    virtual std::string GetGenerator () const {
                        return std::string ();
                    }

                    virtual std::string GetConfig () const {
                        return std::string ();
                    }

                    virtual std::string GetType () const {
                        return std::string ();
                    }

                    virtual bool EquivalentTo (const Dependency &dependency) const {
                        const FrameworkDependency *frameworkDependency =
                            dynamic_cast<const FrameworkDependency *> (&dependency);
                        return frameworkDependency != 0 &&
                            frameworkDependency->framework == framework;
                    }

                    virtual void CollectVersions (
                            Versions & /*versions*/) const {
                    }
                    virtual void SetMinVersion (
                            Versions & /*versions*/,
                            std::set<std::string> & /*visitedDependencies*/) const {
                    }

                    virtual void  GetFeatures (
                            std::set<std::string> & /*features*/) const {
                    }

                    virtual void GetIncludeDirectories (
                            std::set<std::string> & /*include_directories*/) const {
                    }

                    virtual void GetLinkLibraries (
                            std::list<std::string> &link_libraries) const {
                        link_libraries.push_back ("-framework " + framework);
                    }

                    virtual void GetSharedLibraries (
                            std::set<std::string> & /*shared_libraries*/) const {
                    }

                    virtual bool IsInstalled () const {
                        return true;
                    }

                    virtual std::string ToString (util::ui32 indentationLevel = 0) const {
                        return
                            util::OpenTag (indentationLevel, thekogans_make::TAG_FRAMEWORK) +
                            framework +
                            util::CloseTag (0, thekogans_make::TAG_FRAMEWORK);
                    }

                    virtual void ListDependencies (util::ui32 indentationLevel = 0) const {
                        std::cout <<
                            std::string (indentationLevel * 2, ' ') <<
                            "framework: " << framework << std::endl;
                        std::cout.flush ();
                    }
                };

                struct SystemDependency : public thekogans_make::Dependency {
                    std::string library;
                    const thekogans_make &dependent;

                    SystemDependency (
                        const std::string &library_,
                        const thekogans_make &dependent_) :
                        library (library_),
                        dependent (dependent_) {}

                    virtual const thekogans_make &GetDependent () const {
                        return dependent;
                    }

                    virtual std::string GetProjectRoot () const {
                        return std::string ();
                    }
                    virtual std::string GetConfigFile () const {
                        return std::string ();
                    }

                    virtual std::string GetGenerator () const {
                        return std::string ();
                    }

                    virtual std::string GetConfig () const {
                        return std::string ();
                    }

                    virtual std::string GetType () const {
                        return std::string ();
                    }

                    virtual bool EquivalentTo (const Dependency &dependency) const {
                        const SystemDependency *systemDependency =
                            dynamic_cast<const SystemDependency *> (&dependency);
                        return systemDependency != 0 &&
                            systemDependency->library == library;
                    }

                    virtual void CollectVersions (
                            Versions & /*versions*/) const {
                    }
                    virtual void SetMinVersion (
                            Versions & /*versions*/,
                            std::set<std::string> & /*visitedDependencies*/) const {
                    }

                    virtual void  GetFeatures (
                            std::set<std::string> & /*features*/) const {
                    }

                    virtual void GetIncludeDirectories (
                            std::set<std::string> & /*include_directories*/) const {
                    }

                    virtual void GetLinkLibraries (
                            std::list<std::string> &link_libraries) const {
                        link_libraries.push_back (library);
                    }

                    virtual void GetSharedLibraries (
                            std::set<std::string> & /*shared_libraries*/) const {
                    }

                    virtual bool IsInstalled () const {
                        return true;
                    }

                    virtual std::string ToString (util::ui32 indentationLevel = 0) const {
                        return
                            util::OpenTag (indentationLevel, thekogans_make::TAG_SYSTEM) +
                            library +
                            util::CloseTag (0, thekogans_make::TAG_SYSTEM);
                    }

                    virtual void ListDependencies (util::ui32 indentationLevel = 0) const {
                        std::cout <<
                            std::string (indentationLevel * 2, ' ') << library << std::endl;
                        std::cout.flush ();
                    }
                };

                struct SymbolTableMgr {
                    SymbolTable &symbolTable;
                    explicit SymbolTableMgr (SymbolTable &symbolTable_) :
                        symbolTable (symbolTable_) {}
                    ~SymbolTableMgr () {
                        symbolTable.clear ();
                    }
                };
            }

            std::string thekogans_make::GetOrganization (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).organization;
            }

            std::string thekogans_make::GetProject (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).project;
            }

            std::string thekogans_make::GetProjectType (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).project_type;
            }

            std::string thekogans_make::GetVersion (
                    const std::string &project_root,
                    const std::string &config_file) {
                const thekogans_make &config = GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ());
                return config.major_version + VERSION_SEPARATOR +
                    config.minor_version + VERSION_SEPARATOR +
                    config.patch_version;
            }

            std::string thekogans_make::GetNamingConvention (
                    const std::string &project_root,
                    const std::string &config_file) {
                std::string naming_convention = GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).naming_convention;
                return naming_convention.empty () ?
                    _TOOLCHAIN_NAMING_CONVENTION :
                    naming_convention;
            }

            std::string thekogans_make::GetBuildConfig (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).build_config;
            }

            std::string thekogans_make::GetBuildType (
                    const std::string &project_root,
                    const std::string &config_file) {
                const thekogans_make &config = GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ());
                if (config.build_type.empty ()) {
                    if (config.project_type == PROJECT_TYPE_PLUGIN) {
                        return TYPE_SHARED;
                    }
                }
                return config.build_type;
            }

            util::GUID thekogans_make::GetGUID (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).guid;
            }

            std::string thekogans_make::GetSchemaVersion (
                    const std::string &project_root,
                    const std::string &config_file) {
                return GetConfig (
                    project_root,
                    config_file,
                    std::string (),
                    std::string (),
                    std::string ()).schema_version;
            }

            namespace {
                typedef std::map<std::string, thekogans_make::Ptr> ConfigMap;

                ConfigMap &GetConfigMap () {
                    static ConfigMap configMap;
                    return configMap;
                }
            }

            const thekogans_make &thekogans_make::GetConfig (
                    const std::string &project_root,
                    const std::string &config_file,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                ConfigMap &configMap = GetConfigMap ();
                std::string configKey =
                    GetConfigKey (project_root, config_file, generator, config, type);
                ConfigMap::iterator it = configMap.lower_bound (configKey);
                if (it == configMap.end () ||
                        configKey.size () > it->first.size () ||
                        !std::equal (
                            configKey.begin (),
                            configKey.begin () + configKey.size (),
                            it->first.begin ())) {
                    std::pair<ConfigMap::iterator, bool> result =
                        configMap.insert (
                            ConfigMap::value_type (
                                configKey,
                                thekogans_make::Ptr (
                                    new thekogans_make (
                                        project_root,
                                        config_file,
                                        generator,
                                        config,
                                        type))));
                    if (result.second) {
                        it = result.first;
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to map '%s'.",
                            configKey.c_str ());
                    }
                }
                return *it->second;
            }

            void thekogans_make::CheckDependencies () const {
                std::cout << "Checking dependencies for " <<
                    MakePath (project_root, config_file) << std::endl;
                std::cout.flush ();
                Dependency::Versions versions;
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->CollectVersions (versions);
                }
                std::set<std::string> visitedDependencies;
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->SetMinVersion (versions, visitedDependencies);
                }
            }

            void thekogans_make::ListDependencies (util::ui32 indentationLevel) const {
                std::cout <<
                    std::string (indentationLevel * 2, ' ') <<
                    MakePath (project_root, config_file) << std::endl;
                std::cout.flush ();
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->ListDependencies (indentationLevel + 1);
                }
            }

            std::string thekogans_make::GetVersion () const {
                return major_version + VERSION_SEPARATOR + minor_version + VERSION_SEPARATOR + patch_version;
            }

            void thekogans_make::GetFeatures (std::set<std::string> &features_) const {
                for (std::set<std::string>::const_iterator
                        it = features.begin (),
                        end = features.end (); it != end; ++it) {
                    features_.insert (*it);
                }
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->GetFeatures (features_);
                }
            }

            void thekogans_make::GetIncludeDirectories (
                    std::set<std::string> &include_directories_) const {
                for (std::list<IncludeDirectories::Ptr>::const_iterator
                        it = include_directories.begin (),
                        end = include_directories.end (); it != end; ++it) {
                    std::string prefix = MakePath (project_root, (*it)->prefix);
                    for (std::list<std::string>::const_iterator
                            jt = (*it)->paths.begin (),
                            end = (*it)->paths.end (); jt != end; ++jt) {
                        include_directories_.insert (MakePath (prefix, *jt));
                    }
                }
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->GetIncludeDirectories (include_directories_);
                }
            }

            void thekogans_make::GetLinkLibraries (
                    std::list<std::string> &link_libraries_) const {
                std::list<std::string> link_libraries;
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->GetLinkLibraries (link_libraries);
                }
                std::set<std::string> visited_link_libraries;
                for (std::list<std::string>::const_reverse_iterator
                        it = link_libraries.rbegin (),
                        end = link_libraries.rend (); it != end; ++it) {
                    if (visited_link_libraries.find (*it) == visited_link_libraries.end ()) {
                        visited_link_libraries.insert (*it);
                        link_libraries_.push_front (*it);
                    }
                }
            }

            void thekogans_make::GetSharedLibraries (
                    std::set<std::string> &shared_libraries) const {
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    (*it)->GetSharedLibraries (shared_libraries);
                }
            }

            bool thekogans_make::Eval (const char *expression) const {
                if (expression != 0) {
                    THEKOGANS_UTIL_TRY {
                        Tokenizer tokenizer (expression, *this);
                        Parser parser (tokenizer, *this);
                        return parser.Parse ();
                    }
                    THEKOGANS_UTIL_CATCH (util::Exception) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Error parsing: %s (%s)",
                            expression,
                            exception.Report ().c_str ());
                    }
                }
                else {
                    THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                        THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                }
            }

            Value thekogans_make::LookupSymbol (const std::string &symbol) const {
                SymbolTable::const_iterator it = localSymbolTable.find (symbol);
                if (it != localSymbolTable.end ()) {
                    return it->second;
                }
                it = globalSymbolTable.find (symbol);
                if (it != globalSymbolTable.end ()) {
                    return it->second;
                }
                it = EnvironmentSymbolTable::Instance ().find (symbol);
                if (it != EnvironmentSymbolTable::Instance ().end ()) {
                    return it->second;
                }
                std::string environmentVariable =
                    util::GetEnvironmentVariable (symbol.c_str ());
                if (!environmentVariable.empty ()) {
                    return Value (environmentVariable);
                }
                return Value ();
            }

            std::string thekogans_make::Expand (const char *format) const {
                std::string expanded;
                std::size_t formatLength = strlen (format);
                util::TenantReadBuffer buffer (
                    util::HostEndian, (util::ui8 *)format, (util::ui32)formatLength);
                while (buffer.GetDataAvailableForReading () > 0) {
                    util::i8 ch;
                    buffer >> ch;
                    switch (ch) {
                        case '\\': {
                            buffer >> ch;
                            if (IsEscapableCh (ch)) {
                                expanded += ch;
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid escape sequence in: %s", format);
                            }
                            break;
                        }
                        case '\'':
                        case '"':
                            expanded += ParseQuotedString (*this, buffer, ch);
                            break;
                        case '$': {
                            expanded += Function::ParseAndExec (*this, buffer).ToString ();
                            break;
                        }
                        default: {
                            expanded += ch;
                            break;
                        }
                    }
                }
                return expanded;
            }

            std::string thekogans_make::GetProjectDependencyVersion (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &example) const {
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    ProjectDependency *dependency =
                        dynamic_cast<ProjectDependency *> ((*it).get ());
                    if (dependency != 0 &&
                            dependency->organization == organization &&
                            dependency->name == project &&
                            dependency->example == example) {
                        return dependency->version;
                    }
                }
                return std::string ();
            }

            std::string thekogans_make::GetToolchainDependencyVersion (
                    const std::string &organization,
                    const std::string &project) const {
                for (std::list<Dependency::Ptr>::const_iterator
                        it = dependencies.begin (),
                        end = dependencies.end (); it != end; ++it) {
                    ToolchainDependency *dependency =
                        dynamic_cast<ToolchainDependency *> ((*it).get ());
                    if (dependency != 0 &&
                            dependency->organization == organization &&
                            dependency->name == project) {
                        return dependency->version;
                    }
                }
                return std::string ();
            }

            std::string thekogans_make::GetProjectBinDirectory () const {
                return project_type == PROJECT_TYPE_PROGRAM ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(BIN_DIR)" :
                        "$(project_root)/$(BIN_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)") :
                    std::string ();
            }

            std::string thekogans_make::GetProjectLibDirectory () const {
                return project_type == PROJECT_TYPE_LIBRARY || project_type == PROJECT_TYPE_PLUGIN ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(LIB_DIR)" :
                        "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)") :
                    std::string ();
            }

            std::string thekogans_make::GetProjectIncludeDirectory () const {
                return Expand ("$(project_root)/$(INCLUDE_DIR)");
            }

            std::string thekogans_make::GetProjectSrcDirectory () const {
                return Expand ("$(project_root)/$(SRC_DIR)");
            }

            std::string thekogans_make::GetProjectResourcesDirectory () const {
                return Expand ("$(project_root)/$(RESOURCES_DIR)");
            }

            std::string thekogans_make::GetProjectTestsDirectory () const {
                return Expand ("$(project_root)/$(TESTS_DIR)");
            }

            std::string thekogans_make::GetProjectDocDirectory () const {
                return Expand ("$(project_root)/$(DOC_DIR)");
            }

            std::string thekogans_make::GetProjectGoal () const {
                const char *goal = "";
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        goal = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(project_root)/$(LIB_DIR)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        goal = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(project_root)/$(LIB_DIR)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)";
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    goal = naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(BIN_DIR)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)" :
                        "$(project_root)/$(BIN_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(organization)_$(project).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)";
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    goal = naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(LIB_DIR)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                }
                return Expand (goal);
            }

            std::string thekogans_make::GetProjectLinkLibrary () const {
                return project_type == PROJECT_TYPE_LIBRARY ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(LIB_DIR)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                        "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)") :
                    std::string ();
            }

            std::string thekogans_make::GetToolchainConfigFile () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(CONFIG_DIR)/$(organization)_$(project)-$(version).$(XML_EXT)");
            }

            std::string thekogans_make::GetToolchainBinDirectory () const {
                return project_type == PROJECT_TYPE_PROGRAM ?
                    Expand ("$(TOOLCHAIN_DIR)/$(BIN_DIR)/$(organization)_$(project)-$(version)") :
                    std::string ();
            }

            std::string thekogans_make::GetToolchainLibDirectory () const {
                return project_type == PROJECT_TYPE_LIBRARY ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)" :
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)") :
                    std::string ();
            }

            std::string thekogans_make::GetToolchainIncludeDirectory () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(INCLUDE_DIR)/$(organization)_$(project)-$(version)");
            }

            std::string thekogans_make::GetToolchainSrcDirectory () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(SRC_DIR)/$(organization)_$(project)-$(version)");
            }

            std::string thekogans_make::GetToolchainResourcesDirectory () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(RESOURCES_DIR)/$(organization)_$(project)-$(version)");
            }

            std::string thekogans_make::GetToolchainTestsDirectory () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(TESTS_DIR)/$(organization)_$(project)-$(version)");
            }

            std::string thekogans_make::GetToolchainDocDirectory () const {
                return Expand ("$(TOOLCHAIN_DIR)/$(DOC_DIR)/$(organization)_$(project)-$(version)");
            }

            std::string thekogans_make::GetToolchainGoal () const {
                const char *goal = "";
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        goal = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        goal = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)";
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    goal = "$(TOOLCHAIN_DIR)/$(BIN_DIR)/$(organization)_$(project)-$(version)/$(organization)_$(project)$(TOOLCHAIN_PROGRAM_SUFFIX)";
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    goal = naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                }
                return Expand (goal);
            }

            std::string thekogans_make::GetToolchainLinkLibrary () const {
                return project_type == PROJECT_TYPE_LIBRARY ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)") :
                    std::string ();
            }

            namespace {
                std::string SanitizeName (const std::string &name) {
                    std::string sanitizedName;
                    for (std::size_t i = 0, count = name.size (); i < count; ++i) {
                        if (isalnum (name[i])) {
                            sanitizedName += name[i];
                        }
                        else {
                            sanitizedName += "_";
                        }
                    }
                    return sanitizedName;
                }
            }

            void thekogans_make::GetCommonPreprocessorDefinitions (
                    std::list<std::string> &preprocessorDefinitions) const {
                std::string ORGANIZATION =
                    util::StringToUpper (SanitizeName (organization).c_str ());
                std::string PROJECT =
                    util::StringToUpper (SanitizeName (project).c_str ());
            #if defined (TOOLCHAIN_OS_Windows)
                if (project_type == PROJECT_TYPE_LIBRARY ||
                        project_type == PROJECT_TYPE_PLUGIN) {
                    preprocessorDefinitions.push_back (
                        "_LIB_" + ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + PROJECT + "_BUILD");
                }
            #endif // defined (TOOLCHAIN_OS_Windows)
                preprocessorDefinitions.push_back (
                    "TOOLCHAIN_NAMING_CONVENTION=\\\"" + Expand ("$(naming_convention)") + "\\\"");
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_OS_$(TOOLCHAIN_OS)"));
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_ARCH_$(TOOLCHAIN_ARCH)"));
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_COMPILER_$(TOOLCHAIN_COMPILER)"));
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_ENDIAN_$(TOOLCHAIN_ENDIAN)"));
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_CONFIG_$(config)"));
                preprocessorDefinitions.push_back (
                    Expand ("TOOLCHAIN_TYPE_$(type)"));
                preprocessorDefinitions.push_back (
                    "TOOLCHAIN_TRIPLET=\\\"" + Expand ("$(TOOLCHAIN_TRIPLET)") + "\\\"");
                preprocessorDefinitions.push_back (
                    Expand (
                        (ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + PROJECT + "_MAJOR_VERSION=$(major_version)").c_str ()));
                preprocessorDefinitions.push_back (
                    Expand (
                        (ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + PROJECT + "_MINOR_VERSION=$(minor_version)").c_str ()));
                preprocessorDefinitions.push_back (
                    Expand (
                        (ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + PROJECT + "_PATCH_VERSION=$(patch_version)").c_str ()));
                preprocessorDefinitions.push_back (
                    util::FormatString ("%s_%s_VERSION=0x%08x",
                        ORGANIZATION.c_str (),
                        PROJECT.c_str (),
                        (atoi (major_version.c_str ()) << 16) +
                        (atoi (minor_version.c_str ()) << 8) +
                        atoi (patch_version.c_str ())));
            }

            std::string thekogans_make::GetGoalFileName () const {
                const char *goalFileName = "";
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        goalFileName = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        goalFileName = naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)";
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    goalFileName = naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)" :
                        "$(organization)_$(project).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)";
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    goalFileName = naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)";
                }
                return Expand (goalFileName);
            }

            thekogans_make::thekogans_make (
                    const std::string &project_root_,
                    const std::string &config_file_,
                    const std::string &generator_,
                    const std::string &config_,
                    const std::string &type_) :
                    project_root (project_root_),
                    config_file (config_file_),
                    generator (generator_),
                    config (config_),
                    type (type_),
                    guid (util::GUID::Empty) {
                if (generator.empty ()) {
                    generator = MAKE;
                }
                pugi::xml_document document;
                pugi::xml_node root;
                CreateDOM (project_root, config_file, document, root);
                organization = root.attribute (ATTR_ORGANIZATION).value ();
                if (organization.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Must specify organization in: %s",
                        MakePath (project_root, config_file).c_str ());
                }
                project = root.attribute (ATTR_PROJECT).value ();
                if (project.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Must specify project in: %s",
                        MakePath (project_root, config_file).c_str ());
                }
                project_type = root.attribute (ATTR_PROJECT_TYPE).value ();
                if (project_type.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Must specify project_type in: %s",
                        MakePath (project_root, config_file).c_str ());
                }
                if (project_type == PROJECT_TYPE_PLUGIN && type != TYPE_SHARED) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Plugins must be specified with type = \"" TYPE_SHARED "\" in: %s",
                        MakePath (project_root, config_file).c_str ());
                }
                major_version = root.attribute (ATTR_MAJOR_VERSION).value ();
                minor_version = root.attribute (ATTR_MINOR_VERSION).value ();
                patch_version = root.attribute (ATTR_PATCH_VERSION).value ();
                if (major_version.empty () || minor_version.empty () || patch_version.empty ()) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Must specify major_version, minor_version and patch_version in: %s",
                        MakePath (project_root, config_file).c_str ());
                }
                naming_convention = root.attribute (ATTR_NAMING_CONVENTION).value ();
                if (naming_convention.empty ()) {
                    naming_convention = _TOOLCHAIN_NAMING_CONVENTION;
                }
                if (naming_convention != NAMING_CONVENTION_FLAT &&
                        naming_convention != NAMING_CONVENTION_HIERARCHICAL) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unknown naming_convention = %s in: %s",
                        naming_convention.c_str (),
                        MakePath (project_root, config_file).c_str ());
                }
                build_config = root.attribute (ATTR_BUILD_CONFIG).value ();
                if (!build_config.empty ()) {
                    if (config.empty ()) {
                        config = build_config;
                    }
                    else if (config != build_config) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "config (%s)/build_config (%s) mismatch in: %s",
                            config.c_str (),
                            build_config.c_str (),
                            MakePath (project_root, config_file).c_str ());
                    }
                }
                build_type = root.attribute (ATTR_BUILD_TYPE).value ();
                if (!build_type.empty ()) {
                    if (type.empty ()) {
                        type = build_type;
                    }
                    else if (type != build_type) {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "type (%s)/build_type (%s) mismatch in: %s",
                            type.c_str (),
                            build_type.c_str (),
                            MakePath (project_root, config_file).c_str ());
                    }
                }
                std::string guidString = root.attribute (ATTR_GUID).value ();
                if (!guidString.empty ()) {
                    guid = util::GUID (guidString);
                }
                schema_version = root.attribute (ATTR_SCHEMA_VERSION).value ();
                if (schema_version.empty ()) {
                    schema_version = util::ui32Tostring (THEKOGANS_MAKE_XML_SCHEMA_VERSION);
                }
                if (atoi (schema_version.c_str ()) > THEKOGANS_MAKE_XML_SCHEMA_VERSION) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "%s schema version (%s) is greater then we support (%u). "
                        "Please update your version (%s) of thekogans_make_core.",
                        MakePath (project_root, config_file).c_str (),
                        schema_version.c_str (),
                        THEKOGANS_MAKE_XML_SCHEMA_VERSION,
                        core::GetVersion ().ToString ().c_str ());
                }
                CreateGlobalSymbolTable ();
                for (pugi::xml_node child = root.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_CONSTANTS) {
                            Parseconstants (child);
                        }
                        else if (childName == TAG_FEATURES) {
                            Parseset (child, TAG_FEATURE, features);
                        }
                        else if (childName == TAG_PLUGIN_HOSTS) {
                            if (project_type == PROJECT_TYPE_PLUGIN) {
                                Parsedependencies (child, plugin_hosts);
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "%s is not a plugin project.",
                                    MakePath (project_root, config_file).c_str ());
                            }
                        }
                        else if (childName == TAG_DEPENDENCIES) {
                            Parsedependencies (child, dependencies);
                        }
                        else if (childName == TAG_INCLUDE_DIRECTORIES) {
                            IncludeDirectories::Ptr includeDirectories (
                                new IncludeDirectories (*this));
                            {
                                includeDirectories->prefix =
                                    Expand (child.attribute (ATTR_PREFIX).value ());
                                includeDirectories->install =
                                    Expand (child.attribute (ATTR_INSTALL).value ()) == VALUE_YES;
                                SymbolTableMgr symbolTableMgr (localSymbolTable);
                                localSymbolTable[ATTR_PREFIX] = Value (includeDirectories->prefix);
                                localSymbolTable[ATTR_INSTALL] = Value (includeDirectories->install);
                                Parselist (child, TAG_INCLUDE_DIRECTORY, includeDirectories->paths);
                            }
                            if (!includeDirectories->paths.empty ()) {
                                include_directories.push_back (std::move (includeDirectories));
                            }
                        }
                        else if (childName == TAG_PREPROCESSOR_DEFINITIONS) {
                            Parselist (child, TAG_PREPROCESSOR_DEFINITION, preprocessor_definitions);
                        }
                        else if (childName == TAG_LINKER_FLAGS) {
                            Parselist (child, TAG_LINKER_FLAG, linker_flags);
                        }
                        else if (childName == TAG_LIBRARIAN_FLAGS) {
                            Parselist (child, TAG_LIBRARIAN_FLAG, librarian_flags);
                        }
                        else if (childName == TAG_LINK_LIBRARIES) {
                            LinkLibraries::Ptr linkLibraries (
                                new LinkLibraries (
                                    Expand (child.attribute (ATTR_PREFIX).value ()),
                                    Expand (child.attribute (ATTR_INSTALL).value ()) == VALUE_YES, *this));
                            {
                                SymbolTableMgr symbolTableMgr (localSymbolTable);
                                localSymbolTable[ATTR_PREFIX] = Value (linkLibraries->prefix);
                                localSymbolTable[ATTR_INSTALL] = Value (linkLibraries->install);
                                Parselist (child, TAG_LINK_LIBRARY, linkLibraries->files);
                            }
                            if (!linkLibraries->files.empty ()) {
                                link_libraries.push_back (std::move (linkLibraries));
                            }
                        }
                        else if (childName == TAG_MASM_FLAGS) {
                            Parselist (child, TAG_MASM_FLAG, masm_flags);
                        }
                        else if (childName == TAG_MASM_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_MASM_PREPROCESSOR_DEFINITION,
                                masm_preprocessor_definitions);
                        }
                        else if (childName == TAG_MASM_HEADERS) {
                            FileList::Ptr fileList (new MASMHeaders (*this));
                            ParseFileList (child, TAG_MASM_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                masm_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_MASM_SOURCES) {
                            FileList::Ptr fileList (new MASMSources (*this));
                            ParseFileList (child, TAG_MASM_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                masm_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_MASM_TESTS) {
                            FileList::Ptr fileList (new MASMTests (*this));
                            ParseFileList (child, TAG_MASM_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                masm_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_NASM_FLAGS) {
                            Parselist (child, TAG_NASM_FLAG, nasm_flags);
                        }
                        else if (childName == TAG_NASM_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_NASM_PREPROCESSOR_DEFINITION,
                                nasm_preprocessor_definitions);
                        }
                        else if (childName == TAG_NASM_HEADERS) {
                            FileList::Ptr fileList (new NASMHeaders (*this));
                            ParseFileList (child, TAG_NASM_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                nasm_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_NASM_SOURCES) {
                            FileList::Ptr fileList (new NASMSources (*this));
                            ParseFileList (child, TAG_NASM_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                nasm_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_NASM_TESTS) {
                            FileList::Ptr fileList (new NASMTests (*this));
                            ParseFileList (child, TAG_NASM_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                nasm_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_C_FLAGS) {
                            Parselist (child, TAG_C_FLAG, c_flags);
                        }
                        else if (childName == TAG_C_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_C_PREPROCESSOR_DEFINITION,
                                c_preprocessor_definitions);
                        }
                        else if (childName == TAG_C_HEADERS) {
                            FileList::Ptr fileList (new CHeaders (*this));
                            ParseFileList (child, TAG_C_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                c_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_C_SOURCES) {
                            FileList::Ptr fileList (new CSources (*this));
                            ParseFileList (child, TAG_C_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                c_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_C_TESTS) {
                            FileList::Ptr fileList (new CTests (*this));
                            ParseFileList (child, TAG_C_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                c_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_CPP_FLAGS) {
                            Parselist (child, TAG_CPP_FLAG, cpp_flags);
                        }
                        else if (childName == TAG_CPP_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_CPP_PREPROCESSOR_DEFINITION,
                                cpp_preprocessor_definitions);
                        }
                        else if (childName == TAG_CPP_HEADERS) {
                            FileList::Ptr fileList (new CPPHeaders (*this));
                            ParseFileList (child, TAG_CPP_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                cpp_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_CPP_SOURCES) {
                            FileList::Ptr fileList (new CPPSources (*this));
                            ParseFileList (child, TAG_CPP_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                cpp_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_CPP_TESTS) {
                            FileList::Ptr fileList (new CPPTests (*this));
                            ParseFileList (child, TAG_CPP_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                cpp_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_C_FLAGS) {
                            Parselist (
                                child,
                                TAG_OBJECTIVE_C_FLAG,
                                objective_c_flags);
                        }
                        else if (childName == TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITION,
                                objective_c_preprocessor_definitions);
                        }
                        else if (childName == TAG_OBJECTIVE_C_HEADERS) {
                            FileList::Ptr fileList (new ObjectiveCHeaders (*this));
                            ParseFileList (child, TAG_OBJECTIVE_C_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                objective_c_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_C_SOURCES) {
                            FileList::Ptr fileList (new ObjectiveCSources (*this));
                            ParseFileList (child, TAG_OBJECTIVE_C_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_c_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_C_TESTS) {
                            FileList::Ptr fileList (new ObjectiveCTests (*this));
                            ParseFileList (child, TAG_OBJECTIVE_C_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_c_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_FLAGS) {
                            Parselist (child, TAG_OBJECTIVE_CPP_FLAG, objective_cpp_flags);
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITION,
                                objective_cpp_preprocessor_definitions);
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_HEADERS) {
                            FileList::Ptr fileList (new ObjectiveCPPHeaders (*this));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectories::Ptr includeDirectories (
                                        new IncludeDirectories (*this));
                                    includeDirectories->install = fileList->install;
                                    includeDirectories->paths.push_back (
                                        Expand (child.attribute (ATTR_PREFIX).value ()));
                                    include_directories.push_back (std::move (includeDirectories));
                                }
                                objective_cpp_headers.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_SOURCES) {
                            FileList::Ptr fileList (new ObjectiveCPPSources (*this));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_cpp_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_TESTS) {
                            FileList::Ptr fileList (new ObjectiveCPPTests (*this));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_cpp_tests.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_RESOURCES) {
                            FileList::Ptr fileList (new Resources (*this));
                            ParseFileList (child, TAG_RESOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                resources.push_back (std::move (fileList));
                            }
                        }
                        // These five are only available on Windows.
                        else if (childName == TAG_RC_FLAGS) {
                            Parselist (child, TAG_RC_FLAG, rc_flags);
                        }
                        else if (childName == TAG_RC_PREPROCESSOR_DEFINITIONS) {
                            Parselist (
                                child,
                                TAG_RC_PREPROCESSOR_DEFINITION,
                                rc_preprocessor_definitions);
                        }
                        else if (childName == TAG_RC_SOURCES) {
                            FileList::Ptr fileList (new RCSources (*this));
                            ParseFileList (child, TAG_RC_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                rc_sources.push_back (std::move (fileList));
                            }
                        }
                        else if (childName == TAG_SUBSYSTEM) {
                            subsystem = util::TrimSpaces (child.text ().get ());
                        }
                        else if (childName == TAG_DEF_FILE) {
                            def_file = Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        else {
                            ParseDefault (child, root);
                        }
                    }
                }
            }

            void thekogans_make::Parseconstants (pugi::xml_node &node) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_CONSTANT) {
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (!name.empty ()) {
                                globalSymbolTable[name] =
                                    Expand (child.attribute (ATTR_VALUE).value ());
                            }
                            else {
                                THEKOGANS_UTIL_LOG_WARNING ("%s\n",
                                    "Empty constant name, skipping.");
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parsedependencies (
                    pugi::xml_node &node,
                    std::list<Dependency::Ptr> &dependencies) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_PROJECT) {
                            std::string organization =
                                Expand (child.attribute (ATTR_ORGANIZATION).value ());
                            if (organization.empty ()) {
                                organization = _TOOLCHAIN_DEFAULT_ORGANIZATION;
                                if (organization.empty ()) {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                        "Invalid project dependency, missing organization.");
                                }
                            }
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (name.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                    "Invalid project dependency, missing name.");
                            }
                            std::string branch = Expand (child.attribute (ATTR_BRANCH).value ());
                            std::string version = Expand (child.attribute (ATTR_VERSION).value ());
                            std::string example = Expand (child.attribute (ATTR_EXAMPLE).value ());
                            std::string config = Expand (child.attribute (ATTR_CONFIG).value ());
                            std::string type = Expand (child.attribute (ATTR_TYPE).value ());
                            std::set<std::string> features;
                            Parsedependencyfeatures (child, features);
                            dependencies.push_back (
                                Dependency::Ptr (
                                    new ProjectDependency (
                                        organization,
                                        name,
                                        branch,
                                        version,
                                        example,
                                        config,
                                        type,
                                        features,
                                        *this)));
                        }
                        else if (childName == TAG_TOOLCHAIN) {
                            std::string organization =
                                Expand (child.attribute (ATTR_ORGANIZATION).value ());
                            if (organization.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                    "Invalid toolchain dependency, missing organization.");
                            }
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (name.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                                    "Invalid toolchain dependency, missing name.");
                            }
                            std::string version = Expand (child.attribute (ATTR_VERSION).value ());
                            std::string config = Expand (child.attribute (ATTR_CONFIG).value ());
                            std::string type = Expand (child.attribute (ATTR_TYPE).value ());
                            std::set<std::string> features;
                            Parsedependencyfeatures (child, features);
                            dependencies.push_back (
                                Dependency::Ptr (
                                    new ToolchainDependency (
                                        organization,
                                        name,
                                        version,
                                        config,
                                        type,
                                        features,
                                        *this)));
                        }
                        else if (childName == TAG_LIBRARY) {
                            std::string library = util::TrimSpaces (child.text ().get ());
                            if (!library.empty ()) {
                                dependencies.push_back (
                                    Dependency::Ptr (
                                        new LibraryDependency (Expand (library.c_str ()), *this)));
                            }
                        }
                        else if (childName == TAG_FRAMEWORK) {
                            std::string framework = util::TrimSpaces (child.text ().get ());
                            if (!framework.empty ()) {
                                dependencies.push_back (
                                    Dependency::Ptr (
                                        new FrameworkDependency (Expand (framework.c_str ()), *this)));
                            }
                        }
                        else if (childName == TAG_SYSTEM) {
                            std::string library = util::TrimSpaces (child.text ().get ());
                            if (!library.empty ()) {
                                dependencies.push_back (
                                    Dependency::Ptr (
                                        new SystemDependency (Expand (library.c_str ()), *this)));
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parsedependencyfeatures (
                    pugi::xml_node &node,
                    std::set<std::string> &features) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_FEATURES) {
                            Parseset (child, TAG_FEATURE, features);
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parseset (
                    pugi::xml_node &node,
                    const std::string &name,
                    std::set<std::string> &set) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == name) {
                            std::string value = util::TrimSpaces (child.text ().get ());
                            if (!value.empty ()) {
                                set.insert (Expand (value.c_str ()));
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parselist (
                    pugi::xml_node &node,
                    const std::string &name,
                    std::list<std::string> &list) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == name) {
                            std::string value = util::TrimSpaces (child.text ().get ());
                            if (!value.empty ()) {
                                list.push_back (Expand (value.c_str ()));
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            namespace {
                std::regex::flag_type stringToflag (const std::string &flag) {
                    if (flag == "icase") {
                        return std::regex::flag_type::icase;
                    }
                    else if (flag == "nosubs") {
                        return std::regex::flag_type::nosubs;
                    }
                    else if (flag == "optimize") {
                        return std::regex::flag_type::optimize;
                    }
                    else if (flag == "collate") {
                        return std::regex::flag_type::collate;
                    }
                    else if (flag == "ECMAScript") {
                        return std::regex::flag_type::ECMAScript;
                    }
                    else if (flag == "basic") {
                        return std::regex::flag_type::basic;
                    }
                    else if (flag == "extended") {
                        return std::regex::flag_type::extended;
                    }
                    else if (flag == "awk") {
                        return std::regex::flag_type::awk;
                    }
                    else if (flag == "grep") {
                        return std::regex::flag_type::grep;
                    }
                    else if (flag == "egrep") {
                        return std::regex::flag_type::egrep;
                    }
                    return std::regex::flag_type (0);
                }

                std::regex::flag_type ParseRegexFlags (const std::string &flags) {
                    std::regex::flag_type value;
                    std::string::size_type lastPipe = 0;
                    std::string::size_type currPipe = flags.find_first_of ('|', 0);
                    for (; currPipe == std::string::npos;
                            lastPipe = ++currPipe,
                            currPipe = flags.find_first_of ('|', currPipe)) {
                        value |= stringToflag (
                            util::TrimSpaces (flags.substr (lastPipe, currPipe - lastPipe).c_str ()));
                    }
                    value |= stringToflag (
                        util::TrimSpaces (flags.substr (lastPipe).c_str ()));
                    return value != 0 ? value : std::regex::flag_type::ECMAScript;
                }

                void MatchComponent (
                        const std::string &prefix,
                        const std::string &branch,
                        const std::string &pattern,
                        std::regex::flag_type flags,
                        std::list<std::string> &results) {
                    util::Directory directory (ToSystemPath (MakePath (prefix, branch)));
                    util::Directory::Entry entry;
                    try {
                        std::regex regex (pattern, flags);
                        for (bool gotEntry = directory.GetFirstEntry (entry);
                                gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                            if (!util::IsDotOrDotDot (entry.name.c_str ()) &&
                                    std::regex_match (entry.name, regex)) {
                                results.push_back (MakePath (branch, entry.name));
                            }
                        }
                    }
                    catch (const std::regex_error &error) {
                        THEKOGANS_UTIL_THROW_EXCEPTION (error.code (), "%s", error.what ());
                    }
                }
            }

            void thekogans_make::ParseFileList (
                    pugi::xml_node &node,
                    const std::string &name,
                    FileList &fileList) {
                fileList.prefix = Expand (node.attribute (ATTR_PREFIX).value ());
                fileList.install = Expand (node.attribute (ATTR_INSTALL).value ()) == VALUE_YES;
                SymbolTableMgr symbolTableMgr (localSymbolTable);
                localSymbolTable[ATTR_PREFIX] = Value (fileList.prefix);
                localSymbolTable[ATTR_INSTALL] = Value (fileList.install);
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == name) {
                            std::string value = util::TrimSpaces (child.text ().get ());
                            if (!value.empty ()) {
                                fileList.files.push_back (
                                    FileList::File::Ptr (
                                        new FileList::File (Expand (value.c_str ()))));
                            }
                        }
                        else if (childName == TAG_REGEX) {
                            std::regex::flag_type flags = ParseRegexFlags (
                                Expand (child.attribute (ATTR_FLAGS).value ()));
                            std::list<std::string> components;
                            util::Path (
                                util::TrimSpaces (
                                    child.text ().get ())).GetComponents (components);
                            std::string prefix = core::MakePath (project_root, fileList.prefix);
                            std::list<std::string> results;
                            results.push_back (std::string ());
                            for (std::list<std::string>::const_iterator
                                    it = components.begin (),
                                    end = components.end (); it != end; ++it) {
                                std::list<std::string> branches;
                                for (std::list<std::string>::const_iterator
                                        jt = results.begin (),
                                        end = results.end (); jt != end; ++jt) {
                                    MatchComponent (
                                        prefix, *jt, Expand ((*it).c_str ()), flags, branches);
                                }
                                std::swap (results, branches);
                            }
                            for (std::list<std::string>::const_iterator
                                    it = results.begin (),
                                    end = results.end (); it != end; ++it) {
                                fileList.files.push_back (
                                    FileList::File::Ptr (new FileList::File (*it)));
                            }
                        }
                        else if (childName == TAG_CUSTOM_BUILD) {
                            Parsecustom_build (child, fileList);
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parsecustom_build (
                    pugi::xml_node &node,
                    FileList &fileList) {
                std::string name = Expand (node.attribute (ATTR_NAME).value ());
                localSymbolTable[ATTR_NAME] =
                    Value (MakePath (MakePath (project_root, fileList.prefix), name));
                FileList::File::Ptr file (new FileList::File (name, true));
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_OUTPUTS) {
                            Parseoutputs (child, fileList, *file->customBuild);
                        }
                        else if (childName == TAG_DEPENDENCIES) {
                            Parsedependencies (child, fileList, *file->customBuild);
                        }
                        else if (childName == TAG_MESSAGE) {
                            file->customBuild->message =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        else if (childName == TAG_RECIPE) {
                            file->customBuild->recipe =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
                if (!file->name.empty () &&
                        !file->customBuild->recipe.empty () &&
                        !file->customBuild->outputs.empty ()) {
                    fileList.files.push_back (std::move (file));
                }
            }

            void thekogans_make::Parseoutputs (
                    pugi::xml_node &node,
                    FileList &fileList,
                    FileList::File::CustomBuild &customBuild) {
                std::vector<std::string> outputs;
                std::string prefix =
                    MakePath (
                        MakePath (
                            project_root,
                            GetBuildDirectory (generator, config, type)),
                        fileList.prefix);
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_OUTPUT) {
                            std::string output =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                            if (!output.empty ()) {
                                customBuild.outputs.push_back (output);
                                outputs.push_back (MakePath (prefix, output));
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
                if (!outputs.empty ()) {
                    localSymbolTable[TAG_OUTPUTS] = Value (Value::TYPE_string, outputs);
                }
            }

            void thekogans_make::Parsedependencies (
                    pugi::xml_node &node,
                    FileList &fileList,
                    FileList::File::CustomBuild &customBuild) {
                std::vector<std::string> dependencies;
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_DEPENDENCY) {
                            std::string dependency =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                            if (!dependency.empty ()) {
                                customBuild.dependencies.push_back (dependency);
                                dependencies.push_back (MakePath (project_root, dependency));
                            }
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
                if (!dependencies.empty ()) {
                    localSymbolTable[TAG_DEPENDENCIES] =
                        Value (Value::TYPE_string, dependencies);
                }
            }

            bool thekogans_make::Parseif (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent) {
                if (Eval (node.attribute (ATTR_CONDITION).value ())) {
                    for (pugi::xml_node child = node.last_child ();
                            !child.empty (); child = child.previous_sibling ()) {
                        parent.insert_copy_after (child, node);
                    }
                    return true;
                }
                return false;
            }

            void thekogans_make::Parsechoose (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_WHEN) {
                            if (Eval (child.attribute (ATTR_CONDITION).value ())) {
                                for (pugi::xml_node grandChild = child.last_child ();
                                        !grandChild.empty ();
                                        grandChild = grandChild.previous_sibling ()) {
                                    parent.insert_copy_after (grandChild, node);
                                }
                                break;
                            }
                        }
                        else if (childName == TAG_OTHERWISE) {
                            for (pugi::xml_node grandChild = child.last_child ();
                                    !grandChild.empty ();
                                    grandChild = grandChild.previous_sibling ()) {
                                parent.insert_copy_after (grandChild, node);
                            }
                            break;
                        }
                        else {
                            THEKOGANS_UTIL_LOG_WARNING (
                                "Unrecognized tag '%s', skipping.\n",
                                childName.c_str ());
                        }
                    }
                }
            }

            void thekogans_make::ParseDefault (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent) {
                std::string nodeName = node.name ();
                if (nodeName == TAG_IF) {
                    Parseif (node, parent);
                }
                else if (nodeName == TAG_CHOOSE) {
                    Parsechoose (node, parent);
                }
                else if (nodeName == TAG_INFO) {
                    std::string text = util::TrimSpaces (node.text ().get ());
                    if (!text.empty ()) {
                        THEKOGANS_UTIL_LOG_INFO ("%s\n",
                            Expand (text.c_str ()).c_str ());
                    }
                }
                else if (nodeName == TAG_WARNING) {
                    std::string text = util::TrimSpaces (node.text ().get ());
                    if (!text.empty ()) {
                        THEKOGANS_UTIL_LOG_WARNING ("%s\n",
                            Expand (text.c_str ()).c_str ());
                    }
                }
                else if (nodeName == TAG_ERROR) {
                    std::string text = util::TrimSpaces (node.text ().get ());
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Error: %s",
                        Expand (text.c_str ()).c_str ());
                }
                else {
                    THEKOGANS_UTIL_LOG_WARNING (
                        "Unrecognized tag '%s::%s', skipping.\n",
                        parent.name (),
                        node.name ());
                }
            }

            void thekogans_make::CreateGlobalSymbolTable () {
                // ctor arguments.
                globalSymbolTable[VAR_PROJECT_ROOT] = Value (project_root);
                globalSymbolTable[VAR_CONFIG_FILE] = Value (config_file);
                globalSymbolTable[VAR_GENERATOR] = Value (generator);
                globalSymbolTable[VAR_CONFIG] = Value (config);
                globalSymbolTable[VAR_TYPE] = Value (type);
                // root tag (thekogans_make) attributes.
                globalSymbolTable[ATTR_ORGANIZATION] = Value (organization);
                globalSymbolTable[ATTR_PROJECT] = Value (project);
                globalSymbolTable[ATTR_PROJECT_TYPE] = Value (project_type);
                globalSymbolTable[VAR_PROJECT_DIRECTORY] = Value (GetDirectoryFromName (project));
                globalSymbolTable[VAR_BUILD_DIRECTORY] = Value (GetBuildDirectory (generator, config, type));
                globalSymbolTable[ATTR_MAJOR_VERSION] = Value (Value::TYPE_int, major_version);
                globalSymbolTable[ATTR_MINOR_VERSION] = Value (Value::TYPE_int, minor_version);
                globalSymbolTable[ATTR_PATCH_VERSION] = Value (Value::TYPE_int, patch_version);
                globalSymbolTable[VAR_VERSION] = Value (Value::TYPE_Version, GetVersion ());
                globalSymbolTable[ATTR_NAMING_CONVENTION] = Value (naming_convention);
                globalSymbolTable[ATTR_BUILD_CONFIG] = Value (build_config);
                globalSymbolTable[ATTR_BUILD_TYPE] = Value (build_type);
                globalSymbolTable[ATTR_GUID] = Value (guid);
                globalSymbolTable[ATTR_SCHEMA_VERSION] = Value (Value::TYPE_int, schema_version);
                // config/type dependent.
                if (project_type == PROJECT_TYPE_LIBRARY) {
                    globalSymbolTable[VAR_LINK_LIBRARY_SUFFIX] = Value (GetLinkLibrarySuffix (type));
                }
            }

            namespace {
                std::string GetLines (
                        util::ui32 count,
                        ptrdiff_t offset,
                        const char *buffer,
                        ptrdiff_t length) {
                    std::string lines;
                    while (offset < length) {
                        char ch = buffer[offset++];
                        lines += ch;
                        if (ch == '\n' && --count == 0) {
                            break;
                        }
                    }
                    return lines;
                }
            }

            void thekogans_make::CreateDOM (
                    const std::string &project_root,
                    const std::string &config_file,
                    pugi::xml_document &document,
                    pugi::xml_node &root) {
                std::string configFilePath =
                    ToSystemPath (MakePath (project_root, config_file));
                util::ReadOnlyFile configFile (util::HostEndian, configFilePath);
                // Protect yourself.
                const util::ui32 MAX_CONFIG_FILE_SIZE = 1024 * 1024;
                util::ui32 configFileSize = (util::ui32)configFile.GetSize ();
                if (configFileSize > MAX_CONFIG_FILE_SIZE) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "'%s' is bigger (%u) than expected. (%u)",
                        configFilePath.c_str (),
                        configFileSize,
                        MAX_CONFIG_FILE_SIZE);
                }
                util::Buffer buffer (util::HostEndian, configFileSize);
                if (buffer.AdvanceWriteOffset (
                        configFile.Read (
                            buffer.GetWritePtr (),
                            configFileSize)) != configFileSize) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to read %u bytes from '%s'.",
                        configFileSize,
                        configFilePath.c_str ());
                }
                pugi::xml_parse_result result =
                    document.load_buffer (
                        buffer.GetReadPtr (),
                        buffer.GetDataAvailableForReading ());
                if (!result) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to parse: %s (%s), near:\n%s",
                        configFilePath.c_str (),
                        result.description (),
                        GetLines (4, result.offset,
                            (const char *)buffer.GetReadPtr (),
                            buffer.GetDataAvailableForReading ()).c_str ());
                }
                root = document.document_element ();
                if (std::string (root.name ()) != TAG_THEKOGANS_MAKE) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "'%s' is not a valid config file.",
                        configFilePath.c_str ());
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
