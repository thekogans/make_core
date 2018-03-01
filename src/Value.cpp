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

#include "thekogans/make/core/Value.h"

namespace thekogans {
    namespace make {
        namespace core {

            Value &Value::operator += (const Value &rhs) {
                if (type == TYPE_Unknown) {
                    type = rhs.type;
                    value = rhs.value;
                }
                else if (type == rhs.type) {
                    for (std::size_t i = 0, count = rhs.value.size (); i < count; ++i) {
                        value.push_back (rhs.value[i]);
                    }
                }
                return *this;
            }

            Value Value::Parse (
                    Type type,
                    const std::string &str,
                    char separator) {
                Value value;
                std::string::size_type start = 0;
                std::string::size_type end = str.find_first_of (separator, start);
                do {
                    std::string component;
                    if (end == std::string::npos) {
                        component = str.substr (start);
                    }
                    else {
                        component = str.substr (start, end - start);
                    }
                    if (!component.empty ()) {
                        value.value.push_back (component);
                    }
                    start = end + 1;
                    end = str.find_first_of (separator, start);
                } while (start != 0);
                return value;
            }

            std::string Value::ToString (
                    char separator,
                    bool quote,
                    char quoteCh) const {
                std::string str;
                std::size_t count = value.size ();
                if (count > 0) {
                    if (count > 1 && quote) {
                        str = quoteCh;
                    }
                    str += value[0];
                    for (std::size_t i = 1; i < count; ++i) {
                        str += separator + value[i];
                    }
                    if (count > 1 && quote) {
                        str += quoteCh;
                    }
                }
                return str;
            }

        } // namespace core
    } // namespace make
} // namespace thekogans
