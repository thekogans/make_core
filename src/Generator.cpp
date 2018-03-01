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

#include <cassert>
#include "thekogans/util/Exception.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/make/core/Generator.h"

namespace thekogans {
    namespace make {
        namespace core {

            namespace {
                // Believe it or not, but just declaring map static
                // does not guarantee proper ctor call order!? Wrapping
                // it in an accessor function does.
                Generator::Map &GetMap () {
                    static Generator::Map map;
                    return map;
                }
            }

            Generator::UniquePtr Generator::Get (const std::string &type) {
                Map::iterator it = GetMap ().find (type);
                return it != GetMap ().end () ?
                    it->second () : Generator::UniquePtr ();
            }

            Generator::MapInitializer::MapInitializer (
                    const std::string &type,
                    Factory factory) {
                std::pair<Map::iterator, bool> result =
                    GetMap ().insert (Map::value_type (type, factory));
                assert (result.second);
                if (!result.second) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Duplicate Generator: %s", type.c_str ());
                }
            }

            void Generator::GetGenerators (std::list<std::string> &generators) {
                for (Map::const_iterator it = GetMap ().begin (),
                        end = GetMap ().end (); it != end; ++it) {
                    generators.push_back (it->first);
                }
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
