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

#if !defined (__thekogans_make_core_Function_h)
#define __thekogans_make_core_Function_h

#include <memory>
#include <string>
#include <list>
#include <map>
#include "thekogans/util/Buffer.h"
#include "thekogans/make/core/Config.h"
#include "thekogans/make/core/Value.h"

namespace thekogans {
    namespace make {
        namespace core {

            struct thekogans_make;

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Function {
                typedef std::unique_ptr<Function> UniquePtr;

                typedef UniquePtr (*Factory) ();
                typedef std::map<std::string, Factory> Map;

                typedef std::pair<std::string, std::string> Parameter;
                typedef std::list<Parameter> Parameters;

                static Value ParseAndExec (
                    const thekogans_make &config,
                    util::Buffer &buffer);

                typedef std::pair<std::string, util::ui32> Identifier;

                static Value Exec (
                    const thekogans_make &config,
                    const Identifier &identifier,
                    const Parameters &parameters);

                struct _LIB_THEKOGANS_MAKE_CORE_DECL MapInitializer {
                    Map::iterator it;

                    MapInitializer (
                        const std::string &name_,
                        Factory factory);
                    ~MapInitializer ();
                };

                virtual ~Function () {}

                virtual Value Exec (
                    const thekogans_make &config,
                    const Parameters &parameters) const = 0;
            };

            #define THEKOGANS_MAKE_CORE_DECLARE_FUNCTION(name)\
            public:\
                static thekogans::make::core::Function::MapInitializer mapInitializer;\
                static thekogans::make::core::Function::UniquePtr Create () {\
                    return thekogans::make::core::Function::UniquePtr (new name);\
                }

            #define THEKOGANS_MAKE_CORE_IMPLEMENT_FUNCTION(name)\
                thekogans::make::core::Function::MapInitializer name::mapInitializer (\
                    #name, name::Create);

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Function_h)
