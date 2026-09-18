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
#include "thekogans/util/Environment.h"
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
#include "thekogans/make/core/ProjectDependency.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/core/ToolchainDependency.h"
#include "thekogans/make/core/LibraryDependency.h"
#include "thekogans/make/core/FrameworkDependency.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/core/Version.h"
#include "thekogans/make/core/Package.h"
#include "thekogans/make/core/PackageResolver.h"
#include "thekogans/make/core/PackageDependency.h"
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
            const char * const thekogans_make::ATTR_DESTINATION_PREFIX = "destination_prefix";
            const char * const thekogans_make::ATTR_NAME = "name";
            const char * const thekogans_make::ATTR_VALUE = "value";
            const char * const thekogans_make::ATTR_BRANCH = "branch";
            const char * const thekogans_make::ATTR_VERSION = "version";
            const char * const thekogans_make::ATTR_EXAMPLE = "example";
            const char * const thekogans_make::ATTR_CONFIG = "config";
            const char * const thekogans_make::ATTR_TYPE = "type";
            const char * const thekogans_make::ATTR_PRIVATE = "private";
            const char * const thekogans_make::ATTR_FLAGS = "flags";

            const char * const thekogans_make::TAG_THEKOGANS_MAKE = "thekogans_make";
            const char * const thekogans_make::TAG_CONSTANTS = "constants";
            const char * const thekogans_make::TAG_CONSTANT = "constant";
            const char * const thekogans_make::TAG_FEATURES = "features";
            const char * const thekogans_make::TAG_FEATURE = "feature";
            const char * const thekogans_make::TAG_PLUGIN_HOSTS = "plugin_hosts"; /**/
            const char * const thekogans_make::TAG_DEPENDENCIES = "dependencies";
            const char * const thekogans_make::TAG_DEPENDENCY = "dependency";
            const char * const thekogans_make::TAG_PRECOMPILED_HEADER = "precompiled_header";
            const char * const thekogans_make::TAG_FILE = "file";
            const char * const thekogans_make::TAG_OUTPUT_FILE = "output_file";
            const char * const thekogans_make::TAG_PROJECT = "project";
            const char * const thekogans_make::TAG_TOOLCHAIN = "toolchain";
            const char * const thekogans_make::TAG_PACKAGE = "package";
            const char * const thekogans_make::TAG_LIBRARY = "library";
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
            const char * const thekogans_make::TAG_BUNDLE = "bundle";
            const char * const thekogans_make::TAG_INFO_PLIST = "info_plist";
            const char * const thekogans_make::TAG_FRAMEWORKS = "frameworks";
            const char * const thekogans_make::TAG_FRAMEWORK = "framework";
            const char * const thekogans_make::TAG_PLUGINS = "plugins";
            const char * const thekogans_make::TAG_PLUGIN = "plugin";
            const char * const thekogans_make::TAG_SHARED_SUPPORTS = "shared_supports";
            const char * const thekogans_make::TAG_SHARED_SUPPORT = "shared_support";
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

            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make)
            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make::StringList)
            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make::FileList)
            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make::FileList::File)
            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make::FileList::File::CustomBuild)
            THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (thekogans_make::IncludeDirectoriesList)

            namespace {
                struct SymbolTableMgr {
                    SymbolTable &symbolTable;
                    explicit SymbolTableMgr (SymbolTable &symbolTable_) :
                        symbolTable (symbolTable_) {}
                    ~SymbolTableMgr () {
                        symbolTable.clear ();
                    }
                };
            }

            const char * const thekogans_make::PrecompiledHeader::TYPE_NONE = "None";
            const char * const thekogans_make::PrecompiledHeader::TYPE_USE = "Use";
            const char * const thekogans_make::PrecompiledHeader::TYPE_CREATE = "Create";

            std::string thekogans_make::PrecompiledHeader::TypeTostring (Type type) {
                return type == Use ? TYPE_USE :
                    type == Create ? TYPE_CREATE : TYPE_NONE;
            }

            thekogans_make::PrecompiledHeader::Type thekogans_make::PrecompiledHeader::stringToType (
                    const std::string &type) {
                return type == TYPE_USE ? Use :
                    type == TYPE_CREATE ? Create : None;
            }

            const thekogans_make &thekogans_make::GetConfig (
                    const std::string &project_root,
                    const std::string &config_file,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type) {
                using ConfigMap = std::map<std::string, thekogans_make::SharedPtr>;
                static ConfigMap configMap;
                std::string configKey =
                    GetConfigKey (project_root, config_file, generator, config, type);
                ConfigMap::iterator it = configMap.lower_bound (configKey);
                if (it == configMap.end () ||
                        configKey.size () > it->first.size () ||
                        !std::equal (configKey.begin (), configKey.end (), it->first.begin ())) {
                    std::pair<ConfigMap::iterator, bool> result =
                        configMap.insert (
                            ConfigMap::value_type (
                                configKey,
                                thekogans_make::SharedPtr (
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

            std::string thekogans_make::GetVersion () const {
                return major_version + VERSION_SEPARATOR + minor_version + VERSION_SEPARATOR + patch_version;
            }

            void thekogans_make::CheckDependencies () const {
                std::cout << "Checking dependencies for " <<
                    MakePath (project_root, config_file) << std::endl;
                std::cout.flush ();
                Dependency::Versions versions;
                for (auto dependency : dependencies) {
                    dependency->CollectVersions (versions);
                }
                std::set<std::string> visitedDependencies;
                for (auto dependency : dependencies) {
                    dependency->SetMinVersion (versions, visitedDependencies);
                }
            }

            void thekogans_make::ListDependencies (util::ui32 indentationLevel) const {
                std::cout <<
                    std::string (indentationLevel * 2, ' ') <<
                    MakePath (project_root, config_file) << std::endl;
                std::cout.flush ();
                for (auto dependency : dependencies) {
                    dependency->ListDependencies (indentationLevel + 1);
                }
            }

            thekogans_make::Dependency::SharedPtr thekogans_make::GetDependency (
                    const std::string &organization,
                    const std::string &name) const {
                for (auto dependency : dependencies) {
                    if (dependency->GetOrganization () == organization && dependency->GetName () == name) {
                        return dependency;
                    }
                }
                return nullptr;
            }

            void thekogans_make::GetFeatures (std::set<std::string> &features_) const {
                features_.insert (features.begin (), features.end ());
                for (auto dependency : dependencies) {
                    dependency->GetFeatures (features_);
                }
            }

            bool thekogans_make::HasFeature (const std::string &feature) const {
                return features.find (feature) != features.end ();
            }

            void thekogans_make::GetIncludeDirectories (
                    std::set<std::string> &include_directories_) const {
                for (auto include_directory : include_directories) {
                    std::string prefix = MakePath (project_root, include_directory->prefix);
                    for (const auto &path : include_directory->paths) {
                        include_directories_.insert (MakePath (prefix, path));
                    }
                }
                for (auto dependency : dependencies) {
                    dependency->GetIncludeDirectories (include_directories_);
                }
            }

            void thekogans_make::GetLibraryDirectories (
                    std::set<std::string> &library_directories) const {
                for (auto dependency : dependencies) {
                    dependency->GetLibraryDirectories (library_directories);
                }
            }

            void thekogans_make::GetFrameworkDirectories (
                    std::set<std::string> &framework_directories) const {
                for (auto dependency : dependencies) {
                    dependency->GetFrameworkDirectories (framework_directories);
                }
            }

            void thekogans_make::GetLinkLibraries (std::set<std::string> &link_libraries) const {
                for (auto dependency : dependencies) {
                    dependency->GetLinkLibraries (link_libraries);
                }
            }

            void thekogans_make::GetSharedLibraries (std::set<std::string> &shared_libraries) const {
                for (auto dependency : dependencies) {
                    dependency->GetSharedLibraries (shared_libraries);
                }
            }

            void thekogans_make::GetLinkerFlags (std::set<std::string> &linker_flags_) const {
                linker_flags_.insert (linker_flags.begin (), linker_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetLinkerFlags (linker_flags_);
                }
            }

            void thekogans_make::GetLibrarianFlags (std::set<std::string> &librarian_flags_) const {
                librarian_flags_.insert (librarian_flags.begin (), librarian_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetLibrarianFlags (librarian_flags_);
                }
            }

            void thekogans_make::GetMasmFlags (std::set<std::string> &masm_flags_) const {
                masm_flags_.insert (masm_flags.begin (), masm_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetMasmFlags (masm_flags_);
                }
            }

            void thekogans_make::GetMasmPreprocessorDefinitions (
                    std::set<std::string> &masm_preprocessor_definitions_) const {
                masm_preprocessor_definitions_.insert (
                    masm_preprocessor_definitions.begin (), masm_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetMasmPreprocessorDefinitions (masm_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetNasmFlags (std::set<std::string> &nasm_flags_) const {
                nasm_flags_.insert (nasm_flags.begin (), nasm_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetNasmFlags (nasm_flags_);
                }
            }

            void thekogans_make::GetNasmPreprocessorDefinitions (
                    std::set<std::string> &nasm_preprocessor_definitions_) const {
                nasm_preprocessor_definitions_.insert (
                    nasm_preprocessor_definitions.begin (), nasm_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetNasmPreprocessorDefinitions (nasm_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetCFlags (std::set<std::string> &c_flags_) const {
                c_flags_.insert (c_flags.begin (), c_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetCFlags (c_flags_);
                }
            }

            void thekogans_make::GetCPreprocessorDefinitions (
                    std::set<std::string> &c_preprocessor_definitions_) const {
                c_preprocessor_definitions_.insert (
                    c_preprocessor_definitions.begin (), c_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetCPreprocessorDefinitions (c_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetCPPFlags (std::set<std::string> &cpp_flags_) const {
                cpp_flags_.insert (cpp_flags.begin (), cpp_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetCPPFlags (cpp_flags_);
                }
            }

            void thekogans_make::GetCPPPreprocessorDefinitions (
                    std::set<std::string> &cpp_preprocessor_definitions_) const {
                cpp_preprocessor_definitions_.insert (
                    cpp_preprocessor_definitions.begin (), cpp_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetCPPPreprocessorDefinitions (cpp_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetObjectiveCFlags (
                    std::set<std::string> &objective_c_flags_) const {
                objective_c_flags_.insert (objective_c_flags.begin (), objective_c_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetObjectiveCFlags (objective_c_flags_);
                }
            }

            void thekogans_make::GetObjectiveCPreprocessorDefinitions (
                    std::set<std::string> &objective_c_preprocessor_definitions_) const {
                objective_c_preprocessor_definitions_.insert (
                    objective_c_preprocessor_definitions.begin (), objective_c_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetObjectiveCPreprocessorDefinitions (objective_c_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetObjectiveCPPFlags (
                    std::set<std::string> &objective_cpp_flags_) const {
                objective_cpp_flags_.insert (objective_cpp_flags.begin (), objective_cpp_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetObjectiveCPPFlags (objective_cpp_flags_);
                }
            }

            void thekogans_make::GetObjectiveCPPPreprocessorDefinitions (
                    std::set<std::string> &objective_cpp_preprocessor_definitions_) const {
                objective_cpp_preprocessor_definitions_.insert (
                    objective_cpp_preprocessor_definitions.begin (), objective_cpp_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetObjectiveCPPPreprocessorDefinitions (objective_cpp_preprocessor_definitions_);
                }
            }

            void thekogans_make::GetRCFlags (std::set<std::string> &rc_flags_) const {
                rc_flags_.insert (rc_flags.begin (), rc_flags.end ());
                for (auto dependency : dependencies) {
                    dependency->GetRCFlags (rc_flags_);
                }
            }

            void thekogans_make::GetRCPreprocessorDefinitions (
                    std::set<std::string> &rc_preprocessor_definitions_) const {
                rc_preprocessor_definitions_.insert (
                    rc_preprocessor_definitions.begin (), rc_preprocessor_definitions.end ());
                for (auto dependency : dependencies) {
                    dependency->GetRCPreprocessorDefinitions (rc_preprocessor_definitions_);
                }
            }

            bool thekogans_make::Eval (const char *expression) const {
                if (expression != nullptr) {
                    THEKOGANS_UTIL_TRY {
                        Tokenizer tokenizer (expression, *this);
                        Parser parser (tokenizer);
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
                it = EnvironmentSymbolTable::Instance ()->find (symbol);
                if (it != EnvironmentSymbolTable::Instance ()->end ()) {
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
                util::TenantReadBuffer buffer (util::HostEndian, format, formatLength);
                while (!buffer.IsEmpty ()) {
                    util::i8 ch;
                    buffer >> ch;
                    switch (ch) {
                        case '\\': {
                            if (!buffer.IsEmpty ()) {
                                buffer >> ch;
                                if (IsEscapableCh (ch)) {
                                    expanded += ch;
                                }
                                else {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Invalid escape sequence in: %s", format);
                                }
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid escape sequence (eof) in: %s", format);
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
                for (auto dependency : dependencies) {
                    ProjectDependency::SharedPtr projectDependency = dependency;
                    if (projectDependency != nullptr &&
                            projectDependency->organization == organization &&
                            projectDependency->name == project &&
                            projectDependency->example == example) {
                        return projectDependency->version;
                    }
                }
                return std::string ();
            }

            std::string thekogans_make::GetToolchainDependencyVersion (
                    const std::string &organization,
                    const std::string &project) const {
                for (auto dependency : dependencies) {
                    ToolchainDependency::SharedPtr toolchainDependency = dependency;
                    if (toolchainDependency != nullptr &&
                            toolchainDependency->organization == organization &&
                            toolchainDependency->name == project) {
                        return toolchainDependency->version;
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
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(project_root)/$(LIB_DIR)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(project_root)/$(LIB_DIR)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)");
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(BIN_DIR)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)" :
                        "$(project_root)/$(BIN_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(organization)_$(project).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)");
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(project_root)/$(LIB_DIR)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(project_root)/$(LIB_DIR)/$(TOOLCHAIN_BRANCH)/$(config)/$(type)/$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                }
                return std::string ();
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
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)");
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    return Expand ("$(TOOLCHAIN_DIR)/$(BIN_DIR)/$(organization)_$(project)-$(version)/$(organization)_$(project)$(TOOLCHAIN_PROGRAM_SUFFIX)");
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                }
                return std::string ();
            }

            std::string thekogans_make::GetToolchainLinkLibrary () const {
                return project_type == PROJECT_TYPE_LIBRARY ?
                    Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                        "$(TOOLCHAIN_DIR)/$(LIB_DIR)/$(organization)_$(project)-$(version)/$(config)/$(type)/$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)") :
                    std::string ();
            }

            void thekogans_make::GetCommonPreprocessorDefinitions (
                    std::set<std::string> &preprocessorDefinitions) const {
                std::string ORGANIZATION = util::StringToUpper (SanitizeName (organization).c_str ());
                std::string PROJECT = util::StringToUpper (SanitizeName (project).c_str ());
                std::string PREFIX = ORGANIZATION + ORGANIZATION_PROJECT_SEPARATOR + PROJECT;
            #if defined (TOOLCHAIN_OS_Windows)
                if (project_type == PROJECT_TYPE_LIBRARY || project_type == PROJECT_TYPE_PLUGIN) {
                    preprocessorDefinitions.insert ("_LIB_" + PREFIX + "_BUILD");
                }
            #endif // defined (TOOLCHAIN_OS_Windows)
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_NAMING_CONVENTION=\\\"" + Expand ("$(TOOLCHAIN_NAMING_CONVENTION)") + "\\\"");
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_OS_" + Expand ("$(TOOLCHAIN_OS)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_ARCH_" + Expand ("$(TOOLCHAIN_ARCH)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_COMPILER_" + Expand ("$(TOOLCHAIN_COMPILER)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_ENDIAN_" + Expand ("$(TOOLCHAIN_ENDIAN)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_CONFIG_" + Expand ("$(config)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_TYPE_" + Expand ("$(type)"));
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_TRIPLET=\\\"" + Expand ("$(TOOLCHAIN_TRIPLET)") + "\\\"");
                preprocessorDefinitions.insert (
                    "TOOLCHAIN_BRANCH=\\\"" + Expand ("$(TOOLCHAIN_BRANCH)") + "\\\"");
                preprocessorDefinitions.insert (
                    PREFIX + "_MAJOR_VERSION=" + Expand ("$(major_version)"));
                preprocessorDefinitions.insert (
                    PREFIX + "_MINOR_VERSION=" + Expand ("$(minor_version)"));
                preprocessorDefinitions.insert (
                    PREFIX + "_PATCH_VERSION=" + Expand ("$(patch_version)"));
                preprocessorDefinitions.insert (
                    util::FormatString ("%s_VERSION=0x%08x",
                        PREFIX.c_str (),
                        (util::stringToui32 (major_version.c_str ()) << 16) +
                        (util::stringToui32 (minor_version.c_str ()) << 8) +
                        util::stringToui32 (patch_version.c_str ())));
                preprocessorDefinitions.insert (PREFIX + "_CONFIG_" + Expand ("$(config)"));
                preprocessorDefinitions.insert (PREFIX + "_TYPE_" + Expand ("$(type)"));
                for (auto dependency : dependencies) {
                    dependency->GetCommonPreprocessorDefinitions (preprocessorDefinitions);
                }
            }

            std::string thekogans_make::GetGoalFileName () const {
                if (project_type == PROJECT_TYPE_LIBRARY) {
                #if defined (TOOLCHAIN_OS_Windows)
                    if (type == TYPE_SHARED) {
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                            "$(LIB_PREFIX)$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                    }
                    else {
                #endif // defined (TOOLCHAIN_OS_Windows)
                        return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                            "$(LIB_PREFIX)$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(link_library_suffix)" :
                            "$(LIB_PREFIX)$(organization)_$(project).$(version).$(link_library_suffix)");
                #if defined (TOOLCHAIN_OS_Windows)
                    }
                #endif // defined (TOOLCHAIN_OS_Windows)
                }
                else if (project_type == PROJECT_TYPE_PROGRAM) {
                    return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)" :
                        "$(organization)_$(project).$(version)$(TOOLCHAIN_PROGRAM_SUFFIX)");
                }
                else if (project_type == PROJECT_TYPE_PLUGIN) {
                    return Expand (naming_convention == NAMING_CONVENTION_FLAT ?
                        "$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)" :
                        "$(organization)_$(project).$(version).$(TOOLCHAIN_SHARED_LIBRARY_SUFFIX)");
                }
                return std::string ();
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
                    type (type_) {
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
                else if (config.empty ()) {
                    config = CONFIG_RELEASE;
                }
                build_type = project_type == PROJECT_TYPE_PLUGIN ?
                    TYPE_SHARED : root.attribute (ATTR_BUILD_TYPE).value ();
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
                else if (type.empty ()) {
                    type = TYPE_SHARED;
                }
                std::string guidString = root.attribute (ATTR_GUID).value ();
                if (!guidString.empty ()) {
                    guid = util::GUID::FromHexString (guidString);
                }
                schema_version = root.attribute (ATTR_SCHEMA_VERSION).value ();
                if (schema_version.empty ()) {
                    schema_version = util::ui32Tostring (THEKOGANS_MAKE_XML_SCHEMA_VERSION);
                }
                if (util::stringToui32 (schema_version.c_str ()) > THEKOGANS_MAKE_XML_SCHEMA_VERSION) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "%s schema version (%s) is greater then we support (%u). "
                        "Please update your version (%s) of thekogans_make_core.",
                        MakePath (project_root, config_file).c_str (),
                        schema_version.c_str (),
                        THEKOGANS_MAKE_XML_SCHEMA_VERSION,
                        core::GetVersion ().ToString ().c_str ());
                }
                CreateGlobalSymbolTable ();
                for (auto &child : root.children ()) {
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
                        else if (childName == TAG_PRECOMPILED_HEADER) {
                            Parseprecompiled_header (child, precompiled_header);
                        }
                        else if (childName == TAG_INCLUDE_DIRECTORIES) {
                            IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                            {
                                includeDirectories->prefix =
                                    Expand (child.attribute (ATTR_PREFIX).value ());
                                includeDirectories->isPrivate =
                                    Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                                SymbolTableMgr symbolTableMgr (localSymbolTable);
                                localSymbolTable[ATTR_PREFIX] = Value (includeDirectories->prefix);
                                localSymbolTable[ATTR_PRIVATE] = Value (includeDirectories->isPrivate);
                                Parselist (child, TAG_INCLUDE_DIRECTORY, includeDirectories->paths);
                            }
                            if (!includeDirectories->paths.empty ()) {
                                include_directories.push_back (includeDirectories);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_MASM_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                masm_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_MASM_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_MASM_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                masm_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_MASM_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_MASM_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                masm_tests.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_NASM_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                nasm_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_NASM_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_NASM_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                nasm_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_NASM_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_NASM_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                nasm_tests.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_C_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                c_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_C_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_C_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                c_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_C_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_C_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                c_tests.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_CPP_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                cpp_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_CPP_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_CPP_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                cpp_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_CPP_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_CPP_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                cpp_tests.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_C_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                objective_c_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_C_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_C_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_c_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_C_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_C_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_c_tests.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainIncludeDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_HEADER, *fileList);
                            if (!fileList->files.empty ()) {
                                {
                                    IncludeDirectoriesList::SharedPtr includeDirectories (new IncludeDirectoriesList);
                                    includeDirectories->isPrivate = fileList->isPrivate;
                                    includeDirectories->paths.push_back (fileList->prefix);
                                    include_directories.push_back (includeDirectories);
                                }
                                objective_cpp_headers.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_SOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainSrcDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_cpp_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_OBJECTIVE_CPP_TESTS) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainTestsDirectory ()));
                            ParseFileList (child, TAG_OBJECTIVE_CPP_TEST, *fileList);
                            if (!fileList->files.empty ()) {
                                objective_cpp_tests.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_RESOURCES) {
                            FileList::SharedPtr fileList (new FileList (GetToolchainResourcesDirectory ()));
                            ParseFileList (child, TAG_RESOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                resources.push_back (fileList);
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
                            FileList::SharedPtr fileList (new FileList (GetToolchainResourcesDirectory ()));
                            ParseFileList (child, TAG_RC_SOURCE, *fileList);
                            if (!fileList->files.empty ()) {
                                rc_sources.push_back (fileList);
                            }
                        }
                        else if (childName == TAG_SUBSYSTEM) {
                            subsystem = util::TrimSpaces (child.text ().get ());
                        }
                        else if (childName == TAG_DEF_FILE) {
                            def_file = Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        // This is only available on OS X.
                        else if (childName == TAG_BUNDLE) {
                            Parsebundle (child, root);
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
                                THEKOGANS_UTIL_LOG_WARNING ("Empty constant name, skipping.");
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
                    std::vector<Dependency::SharedPtr> &dependencies) {
                for (pugi::xml_node child = node.first_child ();
                        !child.empty (); child = child.next_sibling ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_DEPENDENCY) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string organization =
                                Expand (child.attribute (ATTR_ORGANIZATION).value ());
                            if (organization.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid dependency, missing organization.");
                            }
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (name.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid dependency, missing name.");
                            }
                            std::string branch;
                            std::string version = Expand (child.attribute (ATTR_VERSION).value ());
                            std::string config = Expand (child.attribute (ATTR_CONFIG).value ());
                            std::string type = Expand (child.attribute (ATTR_TYPE).value ());
                            std::set<std::string> features;
                            Parsedependencyfeatures (child, features);
                            if (Project::Find (organization, name, branch, version, std::string ())) {
                                dependencies.push_back (
                                    Dependency::SharedPtr (
                                        new ProjectDependency (
                                            *this,
                                            isPrivate,
                                            organization,
                                            name,
                                            branch,
                                            version,
                                            std::string (),
                                            config,
                                            type,
                                            features)));
                            }
                            else {
                                dependencies.push_back (
                                    Dependency::SharedPtr (
                                        new ToolchainDependency (
                                            *this,
                                            isPrivate,
                                            organization,
                                            name,
                                            version,
                                            config,
                                            type,
                                            features)));
                            }
                        }
                        else if (childName == TAG_PROJECT) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string organization =
                                Expand (child.attribute (ATTR_ORGANIZATION).value ());
                            if (organization.empty ()) {
                                organization = _TOOLCHAIN_DEFAULT_ORGANIZATION;
                                if (organization.empty ()) {
                                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                        "Invalid project dependency, missing organization.");
                                }
                            }
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (name.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
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
                                Dependency::SharedPtr (
                                    new ProjectDependency (
                                        *this,
                                        isPrivate,
                                        organization,
                                        name,
                                        branch,
                                        version,
                                        example,
                                        config,
                                        type,
                                        features)));
                        }
                        else if (childName == TAG_TOOLCHAIN) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string organization =
                                Expand (child.attribute (ATTR_ORGANIZATION).value ());
                            if (organization.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid toolchain dependency, missing organization.");
                            }
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (name.empty ()) {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Invalid toolchain dependency, missing name.");
                            }
                            std::string version = Expand (child.attribute (ATTR_VERSION).value ());
                            std::string config = Expand (child.attribute (ATTR_CONFIG).value ());
                            std::string type = Expand (child.attribute (ATTR_TYPE).value ());
                            std::set<std::string> features;
                            Parsedependencyfeatures (child, features);
                            dependencies.push_back (
                                Dependency::SharedPtr (
                                    new ToolchainDependency (
                                        *this,
                                        isPrivate,
                                        organization,
                                        name,
                                        version,
                                        config,
                                        type,
                                        features)));
                        }
                        else if (childName == TAG_PACKAGE) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (!name.empty ()) {
                                dependencies.push_back (
                                    Dependency::SharedPtr (
                                        new PackageDependency (*this, isPrivate, Package::Constraint::Parse (name))));
                            }
                        }
                        else if (childName == TAG_LIBRARY) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (!name.empty ()) {
                                dependencies.push_back (
                                    Dependency::SharedPtr (
                                        new LibraryDependency (*this, isPrivate, name)));
                            }
                        }
                        else if (childName == TAG_FRAMEWORK) {
                            bool isPrivate = Expand (child.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                            std::string name = Expand (child.attribute (ATTR_NAME).value ());
                            if (!name.empty ()) {
                                dependencies.push_back (
                                    Dependency::SharedPtr (
                                        new FrameworkDependency (*this, isPrivate, name)));
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
                for (auto &child : node.children ()) {
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
                for (auto &child : node.children ()) {
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
                    std::vector<std::string> &list) {
                for (auto &child : node.children ()) {
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
                        return std::regex::icase;
                    }
                    else if (flag == "nosubs") {
                        return std::regex::nosubs;
                    }
                    else if (flag == "optimize") {
                        return std::regex::optimize;
                    }
                    else if (flag == "collate") {
                        return std::regex::collate;
                    }
                    else if (flag == "ECMAScript") {
                        return std::regex::ECMAScript;
                    }
                    else if (flag == "basic") {
                        return std::regex::basic;
                    }
                    else if (flag == "extended") {
                        return std::regex::extended;
                    }
                    else if (flag == "awk") {
                        return std::regex::awk;
                    }
                    else if (flag == "grep") {
                        return std::regex::grep;
                    }
                    else if (flag == "egrep") {
                        return std::regex::egrep;
                    }
                    return std::regex::flag_type (0);
                }

                std::regex::flag_type ParseRegexFlags (const std::string &flags) {
                    std::regex::flag_type value = std::regex::flag_type (0);
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
                    return value == std::regex::flag_type (0) ? std::regex::ECMAScript | std::regex::icase : value;
                }

                void MatchComponent (
                        const std::string &prefix,
                        const std::string &branch,
                        const std::string &pattern,
                        std::regex::flag_type flags,
                        std::vector<std::string> &results) {
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
                        THEKOGANS_UTIL_THROW_EXCEPTION (error.code (), error.what ());
                    }
                }
            }

            void thekogans_make::ParseFileList (
                    pugi::xml_node &node,
                    const std::string &name,
                    FileList &fileList) {
                fileList.prefix = Expand (node.attribute (ATTR_PREFIX).value ());
                fileList.isPrivate = Expand (node.attribute (ATTR_PRIVATE).value ()) == VALUE_YES;
                std::string destinationPrefix =
                    Expand (node.attribute (ATTR_DESTINATION_PREFIX).value ());
                if (!destinationPrefix.empty ()) {
                    fileList.destinationPrefix = destinationPrefix;
                }
                SymbolTableMgr symbolTableMgr (localSymbolTable);
                localSymbolTable[ATTR_PREFIX] = Value (fileList.prefix);
                localSymbolTable[ATTR_PRIVATE] = Value (fileList.isPrivate);
                localSymbolTable[ATTR_DESTINATION_PREFIX] = Value (fileList.destinationPrefix);
                for (auto &child : node.children ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == name) {
                            std::string value = util::TrimSpaces (child.text ().get ());
                            if (!value.empty ()) {
                                FileList::File::SharedPtr file (
                                    new FileList::File (Expand (value.c_str ())));
                                ParseFile (child, *file);
                                fileList.files.push_back (file);
                            }
                        }
                        else if (childName == TAG_REGEX) {
                            std::regex::flag_type flags = ParseRegexFlags (
                                Expand (child.attribute (ATTR_FLAGS).value ()));
                            std::vector<std::string> components;
                            util::Path (
                                util::TrimSpaces (
                                    child.text ().get ())).GetComponents (components);
                            std::string prefix = core::MakePath (project_root, fileList.prefix);
                            std::vector<std::string> results;
                            results.push_back (std::string ());
                            for (const auto &component : components) {
                                std::vector<std::string> branches;
                                for (const auto &result : results) {
                                    MatchComponent (
                                        prefix, result, Expand (component.c_str ()), flags, branches);
                                }
                                std::swap (results, branches);
                            }
                            for (const auto &result : results) {
                                FileList::File::SharedPtr file (new FileList::File (result));
                                ParseFile (child, *file);
                                fileList.files.push_back (file);
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

            void thekogans_make::ParseFile (
                    pugi::xml_node &node,
                    FileList::File &file) {
                for (auto &child : node.children ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_PRECOMPILED_HEADER) {
                            Parseprecompiled_header (child, file.precompiled_header);
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
                FileList::File::SharedPtr file (new FileList::File (name, true));
                for (auto &child : node.children ()) {
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
                    fileList.files.push_back (file);
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
                for (auto &child : node.children ()) {
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

            void thekogans_make::Parseprecompiled_header (
                    pugi::xml_node &node,
                    PrecompiledHeader &precompiledHeader) {
                precompiledHeader.type = PrecompiledHeader::stringToType (
                    Expand (node.attribute (ATTR_TYPE).value ()));
                for (auto &child : node.children ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_FILE) {
                            precompiledHeader.file =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        if (childName == TAG_OUTPUT_FILE) {
                            precompiledHeader.outputFile =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        else {
                            ParseDefault (child, node);
                        }
                    }
                }
            }

            void thekogans_make::Parsedependencies (
                    pugi::xml_node &node,
                    FileList &fileList,
                    FileList::File::CustomBuild &customBuild) {
                std::vector<std::string> dependencies;
                std::string prefix = MakePath (project_root, fileList.prefix);
                for (auto &child : node.children ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_DEPENDENCY) {
                            std::string dependency =
                                Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                            if (!dependency.empty ()) {
                                customBuild.dependencies.push_back (dependency);
                                dependencies.push_back (MakePath (prefix, dependency));
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

            void thekogans_make::Parsebundle (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent) {
                for (auto &child : node.children ()) {
                    if (child.type () == pugi::node_element) {
                        std::string childName = child.name ();
                        if (childName == TAG_INFO_PLIST) {
                            bundle.info_plist = Expand (util::TrimSpaces (child.text ().get ()).c_str ());
                        }
                        else if (childName == TAG_FRAMEWORKS) {
                            Parselist (child, TAG_FRAMEWORK, bundle.frameworks);
                        }
                        else if (childName == TAG_PLUGINS) {
                            Parselist (child, TAG_PLUGIN, bundle.plugins);
                        }
                        else if (childName == TAG_SHARED_SUPPORTS) {
                            Parselist (child, TAG_SHARED_SUPPORT, bundle.shared_supports);
                        }
                        else {
                            ParseDefault (node, parent);
                        }
                    }
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
                for (auto &child : node.children ()) {
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
                        THEKOGANS_UTIL_LOG_INFO (
                            "Info: %s",
                            Expand (text.c_str ()).c_str ());
                    }
                }
                else if (nodeName == TAG_WARNING) {
                    std::string text = util::TrimSpaces (node.text ().get ());
                    if (!text.empty ()) {
                        THEKOGANS_UTIL_LOG_WARNING (
                            "Warning: %s",
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
