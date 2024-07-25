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

#if !defined (__thekogans_make_core_thekogans_make_h)
#define __thekogans_make_core_thekogans_make_h

#include <memory>
#include <string>
#include <list>
#include <set>
#include <map>
#include "pugixml/pugixml.hpp"
#include "thekogans/util/Heap.h"
#include "thekogans/util/GUID.h"
#include "thekogans/make/core/Config.h"
#include "thekogans/make/core/Value.h"
#include "thekogans/make/core/Installer.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/core/Utils.h"

namespace thekogans {
    namespace make {
        namespace core {

            /// \struct thekogans_make thekogans_make.h thekogans/make/thekogans_make.h
            ///
            /// \brief
            /// Used to retrieve various info from the project's thekogans_make.xml file.

            struct _LIB_THEKOGANS_MAKE_CORE_DECL thekogans_make {
                typedef std::unique_ptr<thekogans_make> Ptr;

                THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                static const char * const ATTR_ORGANIZATION;
                static const char * const ATTR_PROJECT;
                static const char * const ATTR_PROJECT_TYPE;
                static const char * const ATTR_MAJOR_VERSION;
                static const char * const ATTR_MINOR_VERSION;
                static const char * const ATTR_PATCH_VERSION;
                static const char * const ATTR_NAMING_CONVENTION;
                static const char * const ATTR_BUILD_CONFIG;
                static const char * const ATTR_BUILD_TYPE;
                static const char * const ATTR_GUID;
                static const char * const ATTR_SCHEMA_VERSION;
                static const char * const ATTR_CONDITION;
                static const char * const ATTR_PREFIX;
                static const char * const ATTR_INSTALL;
                static const char * const ATTR_DESTINATION_PREFIX;
                static const char * const ATTR_NAME;
                static const char * const ATTR_VALUE;
                static const char * const ATTR_BRANCH;
                static const char * const ATTR_VERSION;
                static const char * const ATTR_EXAMPLE;
                static const char * const ATTR_CONFIG;
                static const char * const ATTR_TYPE;
                static const char * const ATTR_FLAGS;
                static const char * const ATTR_PATH;

                static const char * const TAG_THEKOGANS_MAKE;
                static const char * const TAG_GOAL;
                static const char * const TAG_CONSTANTS;
                static const char * const TAG_CONSTANT;
                static const char * const TAG_FEATURES;
                static const char * const TAG_FEATURE;
                static const char * const TAG_PLUGIN_HOSTS;
                static const char * const TAG_DEPENDENCIES;
                static const char * const TAG_DEPENDENCY;
                static const char * const TAG_PRECOMPILED_HEADER;
                static const char * const TAG_FILE;
                static const char * const TAG_OUTPUT_FILE;
                static const char * const TAG_PROJECT;
                static const char * const TAG_TOOLCHAIN;
                static const char * const TAG_LIBRARY;
                static const char * const TAG_FRAMEWORK;
                static const char * const TAG_SYSTEM;
                static const char * const TAG_INCLUDE_DIRECTORIES;
                static const char * const TAG_INCLUDE_DIRECTORY;
                static const char * const TAG_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_PREPROCESSOR_DEFINITION;
                static const char * const TAG_LINKER_FLAGS;
                static const char * const TAG_LINKER_FLAG;
                static const char * const TAG_LIBRARIAN_FLAGS;
                static const char * const TAG_LIBRARIAN_FLAG;
                static const char * const TAG_LINK_LIBRARIES;
                static const char * const TAG_LINK_LIBRARY;
                static const char * const TAG_MASM_FLAGS;
                static const char * const TAG_MASM_FLAG;
                static const char * const TAG_MASM_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_MASM_PREPROCESSOR_DEFINITION;
                static const char * const TAG_MASM_HEADERS;
                static const char * const TAG_MASM_HEADER;
                static const char * const TAG_MASM_SOURCES;
                static const char * const TAG_MASM_SOURCE;
                static const char * const TAG_MASM_TESTS;
                static const char * const TAG_MASM_TEST;
                static const char * const TAG_NASM_FLAGS;
                static const char * const TAG_NASM_FLAG;
                static const char * const TAG_NASM_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_NASM_PREPROCESSOR_DEFINITION;
                static const char * const TAG_NASM_HEADERS;
                static const char * const TAG_NASM_HEADER;
                static const char * const TAG_NASM_SOURCES;
                static const char * const TAG_NASM_SOURCE;
                static const char * const TAG_NASM_TESTS;
                static const char * const TAG_NASM_TEST;
                static const char * const TAG_C_FLAGS;
                static const char * const TAG_C_FLAG;
                static const char * const TAG_C_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_C_PREPROCESSOR_DEFINITION;
                static const char * const TAG_C_HEADERS;
                static const char * const TAG_C_HEADER;
                static const char * const TAG_C_SOURCES;
                static const char * const TAG_C_SOURCE;
                static const char * const TAG_C_TESTS;
                static const char * const TAG_C_TEST;
                static const char * const TAG_CPP_FLAGS;
                static const char * const TAG_CPP_FLAG;
                static const char * const TAG_CPP_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_CPP_PREPROCESSOR_DEFINITION;
                static const char * const TAG_CPP_HEADERS;
                static const char * const TAG_CPP_HEADER;
                static const char * const TAG_CPP_SOURCES;
                static const char * const TAG_CPP_SOURCE;
                static const char * const TAG_CPP_TESTS;
                static const char * const TAG_CPP_TEST;
                static const char * const TAG_OBJECTIVE_C_FLAGS;
                static const char * const TAG_OBJECTIVE_C_FLAG;
                static const char * const TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_OBJECTIVE_C_PREPROCESSOR_DEFINITION;
                static const char * const TAG_OBJECTIVE_C_HEADERS;
                static const char * const TAG_OBJECTIVE_C_HEADER;
                static const char * const TAG_OBJECTIVE_C_SOURCES;
                static const char * const TAG_OBJECTIVE_C_SOURCE;
                static const char * const TAG_OBJECTIVE_C_TESTS;
                static const char * const TAG_OBJECTIVE_C_TEST;
                static const char * const TAG_OBJECTIVE_CPP_FLAGS;
                static const char * const TAG_OBJECTIVE_CPP_FLAG;
                static const char * const TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_OBJECTIVE_CPP_PREPROCESSOR_DEFINITION;
                static const char * const TAG_OBJECTIVE_CPP_HEADERS;
                static const char * const TAG_OBJECTIVE_CPP_HEADER;
                static const char * const TAG_OBJECTIVE_CPP_SOURCES;
                static const char * const TAG_OBJECTIVE_CPP_SOURCE;
                static const char * const TAG_OBJECTIVE_CPP_TESTS;
                static const char * const TAG_OBJECTIVE_CPP_TEST;
                static const char * const TAG_REGEX;
                static const char * const TAG_CUSTOM_BUILD;
                static const char * const TAG_OUTPUTS;
                static const char * const TAG_OUTPUT;
                static const char * const TAG_MESSAGE;
                static const char * const TAG_RECIPE;
                static const char * const TAG_RESOURCES;
                static const char * const TAG_RESOURCE;
                static const char * const TAG_RC_FLAGS;
                static const char * const TAG_RC_FLAG;
                static const char * const TAG_RC_PREPROCESSOR_DEFINITIONS;
                static const char * const TAG_RC_PREPROCESSOR_DEFINITION;
                static const char * const TAG_RC_SOURCES;
                static const char * const TAG_RC_SOURCE;
                static const char * const TAG_SUBSYSTEM;
                static const char * const TAG_DEF_FILE;
                static const char * const TAG_BUNDLE;
                static const char * const TAG_INFO_PLIST;
                static const char * const TAG_FRAMEWORKS;
                static const char * const TAG_PLUGINS;
                static const char * const TAG_PLUGIN;
                static const char * const TAG_SHARED_SUPPORTS;
                static const char * const TAG_SHARED_SUPPORT;
                static const char * const TAG_IF;
                static const char * const TAG_CHOOSE;
                static const char * const TAG_WHEN;
                static const char * const TAG_OTHERWISE;
                static const char * const TAG_INFO;
                static const char * const TAG_WARNING;
                static const char * const TAG_ERROR;

                static const char * const VAR_PROJECT_ROOT;
                static const char * const VAR_CONFIG_FILE;
                static const char * const VAR_GENERATOR;
                static const char * const VAR_CONFIG;
                static const char * const VAR_TYPE;
                static const char * const VAR_PROJECT_DIRECTORY;
                static const char * const VAR_BUILD_DIRECTORY;
                static const char * const VAR_VERSION;
                static const char * const VAR_LINK_LIBRARY_SUFFIX;

                // ctor arguments.
                std::string project_root;
                std::string config_file;
                std::string generator;
                std::string config;
                std::string type;
                // thekogans_make tag attributes.
                std::string organization;
                std::string project;
                std::string project_type;
                std::string major_version;
                std::string minor_version;
                std::string patch_version;
                std::string naming_convention;
                std::string build_config;
                std::string build_type;
                util::GUID guid;
                std::string schema_version;
                // thekogans_make body.
                std::string goal;
                std::set<std::string> features;
                struct _LIB_THEKOGANS_MAKE_CORE_DECL Dependency {
                    typedef std::unique_ptr<Dependency> Ptr;

                    virtual ~Dependency () {}

                    virtual const thekogans_make &GetDependent () const = 0;

                    virtual std::string GetProjectRoot () const = 0;
                    virtual std::string GetConfigFile () const = 0;
                    virtual std::string GetGenerator () const = 0;
                    virtual std::string GetConfig () const = 0;
                    virtual std::string GetType () const = 0;

                    virtual bool EquivalentTo (
                        const Dependency & /*dependency*/) const = 0;

                    typedef std::pair<std::string, std::string> VersionAndBranch;
                    typedef std::set<VersionAndBranch> VersionSet;
                    typedef std::map<std::string, VersionSet> Versions;

                    virtual void CollectVersions (Versions & /*versions*/) const = 0;
                    virtual void SetMinVersion (
                        Versions & /*versions*/,
                        std::set<std::string> & /*visitedDependencies*/) const = 0;

                    virtual void GetPreprocessorDefinitions (
                        std::list<std::string> & /*preprocessorDefinitions*/) const = 0;
                    virtual void GetFeatures (
                        std::set<std::string> & /*features*/) const = 0;

                    virtual void GetIncludeDirectories (
                        std::set<std::string> & /*include_directories*/) const = 0;

                    virtual void GetLinkLibraries (
                        std::list<std::string> & /*link_libraries*/) const = 0;

                    virtual void GetSharedLibraries (
                        std::set<std::string> & /*shared_libraries*/) const = 0;

                    virtual bool IsInstalled () const = 0;
                    virtual std::string ToString (util::ui32 /*indentationLevel*/ = 0) const = 0;

                    virtual void ListDependencies (util::ui32 /*indentationLevel*/ = 0) const = 0;
                };
                std::list<Dependency::Ptr> plugin_hosts;
                std::list<Dependency::Ptr> dependencies;
                struct _LIB_THEKOGANS_MAKE_CORE_DECL PrecompiledHeader {
                    enum Type {
                        None,
                        Use,
                        Create
                    } type;
                    std::string file;
                    std::string outputFile;

                    static const char * const TYPE_NONE;
                    static const char * const TYPE_USE;
                    static const char * const TYPE_CREATE;

                    static std::string TypeTostring (Type type);
                    static Type stringToType (const std::string &type);

                    PrecompiledHeader () :
                        type (None) {}
                } precompiled_header;
                struct _LIB_THEKOGANS_MAKE_CORE_DECL FileList {
                    typedef std::unique_ptr<FileList> Ptr;

                    THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                    std::string prefix;
                    bool install;
                    std::string destinationPrefix;
                    struct _LIB_THEKOGANS_MAKE_CORE_DECL File {
                        typedef std::unique_ptr<File> Ptr;

                        THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                        std::string name;
                        struct _LIB_THEKOGANS_MAKE_CORE_DECL CustomBuild {
                            typedef std::unique_ptr<CustomBuild> Ptr;

                            THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                            std::vector<std::string> outputs;
                            std::vector<std::string> dependencies;
                            std::string message;
                            std::string recipe;

                            CustomBuild () {}
                            CustomBuild (
                                const std::string &message_,
                                const std::string &recipe_) :
                                message (message_),
                                recipe (recipe_) {}

                            THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (CustomBuild)
                        };
                        CustomBuild::Ptr customBuild;
                        PrecompiledHeader precompiled_header;

                        File () {}
                        File (
                            const std::string &name_,
                            bool createCustomBuild = false) :
                            name (name_),
                            customBuild (createCustomBuild ? new CustomBuild : 0) {}

                        THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (File)
                    };
                    std::list<File::Ptr> files;

                    explicit FileList (const std::string &destinationPrefix_) :
                        install (false),
                        destinationPrefix (destinationPrefix_) {}

                    THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (FileList)
                };
                struct _LIB_THEKOGANS_MAKE_CORE_DECL IncludeDirectories {
                    typedef std::unique_ptr<IncludeDirectories> Ptr;

                    THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                    std::string prefix;
                    bool install;
                    std::list<std::string> paths;

                    IncludeDirectories () :
                        install (false) {}

                    THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (IncludeDirectories)
                };
                std::list<IncludeDirectories::Ptr> include_directories;
                std::list<std::string> preprocessor_definitions;
                std::list<std::string> linker_flags;
                std::list<std::string> librarian_flags;
                struct _LIB_THEKOGANS_MAKE_CORE_DECL LinkLibraries {
                    typedef std::unique_ptr<LinkLibraries> Ptr;

                    THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

                    std::string prefix;
                    bool install;
                    std::list<std::string> files;

                    LinkLibraries (
                        const std::string &prefix_,
                        bool install_) :
                        prefix (prefix_),
                        install (install_) {}

                    THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (LinkLibraries)
                };
                std::list<LinkLibraries::Ptr> link_libraries;
                std::list<std::string> masm_flags;
                std::list<std::string> masm_preprocessor_definitions;
                std::list<FileList::Ptr> masm_headers;
                std::list<FileList::Ptr> masm_sources;
                std::list<FileList::Ptr> masm_tests;
                std::list<std::string> nasm_flags;
                std::list<std::string> nasm_preprocessor_definitions;
                std::list<FileList::Ptr> nasm_headers;
                std::list<FileList::Ptr> nasm_sources;
                std::list<FileList::Ptr> nasm_tests;
                std::list<std::string> c_flags;
                std::list<std::string> c_preprocessor_definitions;
                std::list<FileList::Ptr> c_headers;
                std::list<FileList::Ptr> c_sources;
                std::list<FileList::Ptr> c_tests;
                std::list<std::string> cpp_flags;
                std::list<std::string> cpp_preprocessor_definitions;
                std::list<FileList::Ptr> cpp_headers;
                std::list<FileList::Ptr> cpp_sources;
                std::list<FileList::Ptr> cpp_tests;
                std::list<std::string> objective_c_flags;
                std::list<std::string> objective_c_preprocessor_definitions;
                std::list<FileList::Ptr> objective_c_headers;
                std::list<FileList::Ptr> objective_c_sources;
                std::list<FileList::Ptr> objective_c_tests;
                std::list<std::string> objective_cpp_flags;
                std::list<std::string> objective_cpp_preprocessor_definitions;
                std::list<FileList::Ptr> objective_cpp_headers;
                std::list<FileList::Ptr> objective_cpp_sources;
                std::list<FileList::Ptr> objective_cpp_tests;
                std::list<FileList::Ptr> resources;
                // Windows specific.
                std::list<std::string> rc_flags;
                std::list<std::string> rc_preprocessor_definitions;
                std::list<FileList::Ptr> rc_sources;
                std::string subsystem;
                std::string def_file;
                // OSX specific.
                struct _LIB_THEKOGANS_MAKE_CORE_DECL Bunde {
                    std::string info_plist;
                    std::list<std::string> resources;
                    std::list<std::string> frameworks;
                    std::list<std::string> plugins;
                    std::list<std::string> shared_supports;
                } bundle;
                SymbolTable globalSymbolTable;
                SymbolTable localSymbolTable;

                static std::string GetOrganization (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetProject (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetProjectType (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetVersion (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetNamingConvention (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetBuildConfig (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetBuildType (
                    const std::string &project_root,
                    const std::string &config_file);
                static util::GUID GetGUID (
                    const std::string &project_root,
                    const std::string &config_file);
                static std::string GetSchemaVersion (
                    const std::string &project_root,
                    const std::string &config_file);

                static const thekogans_make &GetConfig (
                    const std::string &project_root,
                    const std::string &config_file,
                    const std::string &generator,
                    const std::string &config,
                    const std::string &type);

                void CheckDependencies () const;
                void ListDependencies (util::ui32 indentationLevel) const;

                std::string GetVersion () const;
                void GetFeatures (std::set<std::string> &features_) const;
                bool HasFeature (const std::string &feature) const;
                void GetIncludeDirectories (std::set<std::string> &include_directories_) const;
                void GetFrameworkDirectories (std::set<std::string> &framework_directories) const;
                void GetLinkLibraries (std::list<std::string> &link_libraries_) const;
                void GetSharedLibraries (std::set<std::string> &shared_libraries) const;

                inline bool HasGoal () const {
                    return
                        !goal.empty () ||
                        !masm_sources.empty () ||
                        !nasm_sources.empty () ||
                        !c_sources.empty () ||
                        !cpp_sources.empty () ||
                        !objective_c_sources.empty () ||
                        !objective_cpp_sources.empty () ||
                        !rc_sources.empty ();
                }

                bool Eval (const char *expression) const;
                Value LookupSymbol (const std::string &symbol) const;
                std::string Expand (const char *format) const;

                std::string GetProjectDependencyVersion (
                    const std::string &organization,
                    const std::string &project,
                    const std::string &example) const;
                std::string GetToolchainDependencyVersion (
                    const std::string &organization,
                    const std::string &project) const;

                std::string GetProjectBinDirectory () const;
                std::string GetProjectLibDirectory () const;
                std::string GetProjectIncludeDirectory () const;
                std::string GetProjectSrcDirectory () const;
                std::string GetProjectResourcesDirectory () const;
                std::string GetProjectTestsDirectory () const;
                std::string GetProjectDocDirectory () const;
                std::string GetProjectGoal () const;
                std::string GetProjectLinkLibrary () const;

                std::string GetToolchainConfigFile () const;
                std::string GetToolchainBinDirectory () const;
                std::string GetToolchainLibDirectory () const;
                std::string GetToolchainIncludeDirectory () const;
                std::string GetToolchainSrcDirectory () const;
                std::string GetToolchainResourcesDirectory () const;
                std::string GetToolchainTestsDirectory () const;
                std::string GetToolchainDocDirectory () const;
                std::string GetToolchainGoal () const;
                std::string GetToolchainLinkLibrary () const;

                void GetCommonPreprocessorDefinitions (
                    std::list<std::string> &preprocessorDefinitions) const;
                std::string GetGoalFileName () const;

            private:
                thekogans_make (
                    const std::string &project_root_,
                    const std::string &config_file_,
                    const std::string &generator_,
                    const std::string &config_,
                    const std::string &type_);

                void Parseconstants (pugi::xml_node &node);
                void Parsedependencies (
                    pugi::xml_node &node,
                    std::list<Dependency::Ptr> &dependencies);
                void Parsedependencyfeatures (
                    pugi::xml_node &node,
                    std::set<std::string> &features);
                void Parseset (
                    pugi::xml_node &node,
                    const std::string &name,
                    std::set<std::string> &set);
                void Parselist (
                    pugi::xml_node &node,
                    const std::string &name,
                    std::list<std::string> &list);
                void ParseFileList (
                    pugi::xml_node &node,
                    const std::string &name,
                    FileList &fileList);
                void ParseFile (
                    pugi::xml_node &node,
                    FileList::File &file);
                void Parsecustom_build (
                    pugi::xml_node &node,
                    FileList &fileList);
                void Parseoutputs (
                    pugi::xml_node &node,
                    FileList &fileList,
                    FileList::File::CustomBuild &customBuild);
                void Parseprecompiled_header (
                    pugi::xml_node &node,
                    PrecompiledHeader &precompiledHeader);
                void Parsedependencies (
                    pugi::xml_node &node,
                    FileList &fileList,
                    FileList::File::CustomBuild &customBuild);
                void Parsebundle (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent);
                bool Parseif (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent);
                void Parsechoose (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent);
                void ParseDefault (
                    const pugi::xml_node &node,
                    pugi::xml_node &parent);
                void CreateGlobalSymbolTable ();
                static void CreateDOM (
                    const std::string &project_root,
                    const std::string &config_file,
                    pugi::xml_document &document,
                    pugi::xml_node &root);

                THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (thekogans_make)
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_thekogans_make_h)
