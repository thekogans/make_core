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

#if !defined (__thekogans_make_core_Value_h)
#define __thekogans_make_core_Value_h

#include <string>
#include <vector>
#include "thekogans/util/Version.h"
#include "thekogans/util/GUID.h"
#include "thekogans/make/core/Config.h"

namespace thekogans {
    namespace make {
        namespace core {

            #define VALUE_TRUE "true"
            #define VALUE_FALSE "false"

            struct _LIB_THEKOGANS_MAKE_CORE_DECL Value {
                enum Type {
                    TYPE_Unknown,
                    TYPE_bool,
                    TYPE_int,
                    TYPE_float,
                    TYPE_string,
                    TYPE_GUID,
                    TYPE_Version
                } type;
                std::vector<std::string> value;

                Value () :
                    type (TYPE_Unknown) {}
                Value (Type type_) :
                    type (type_) {}
                Value (
                    Type type_,
                    const std::string &value_) :
                    type (type_),
                    value (1, value_) {}
                Value (
                    Type type_,
                    const std::vector<std::string> &value_) :
                    type (type_),
                    value (value_) {}
                Value (bool b) :
                    type (TYPE_bool),
                    value (1, b ? VALUE_TRUE : VALUE_FALSE) {}
                Value (util::i32 i) :
                    type (TYPE_int),
                    value (1, util::i32Tostring (i)) {}
                Value (util::ui32 ui) :
                    type (TYPE_int),
                    value (1, util::ui32Tostring (ui)) {}
                Value (util::f32 f) :
                    type (TYPE_float),
                    value (1, util::f32Tostring (f)) {}
                Value (const std::string &s) :
                    type (TYPE_string),
                    value (1, s) {}
                Value (const util::GUID &g) :
                    type (TYPE_GUID),
                    value (1, g.ToString ()) {}
                Value (const util::Version &v) :
                    type (TYPE_Version),
                    value (1, v.ToString ()) {}

                Value &operator += (const Value &rhs);

                static Value Parse (
                    Type type,
                    const std::string &str,
                    char separator = ' ');
                std::string ToString (
                    char separator = ' ',
                    bool quote = true,
                    char quoteCh = '"') const;
            };

        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_Value_h)
