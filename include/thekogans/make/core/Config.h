// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_make_core.
//
// libthekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_core_Config_h)
#define __thekogans_make_core_Config_h

#if !defined (__cplusplus)
    #error libthekogans_make_core requires C++ compilation (use a .cpp suffix)
#endif // !defined (__cplusplus)

#if defined (TOOLCHAIN_OS_Windows)
    #define _LIB_THEKOGANS_MAKE_CORE_API __stdcall
    #if defined (TOOLCHAIN_TYPE_Shared)
        #if defined (_LIB_THEKOGANS_MAKE_CORE_BUILD)
            #define _LIB_THEKOGANS_MAKE_CORE_DECL __declspec (dllexport)
        #else // defined (_LIB_THEKOGANS_MAKE_CORE_BUILD)
            #define _LIB_THEKOGANS_MAKE_CORE_DECL __declspec (dllimport)
        #endif // defined (_LIB_THEKOGANS_MAKE_CORE_BUILD)
    #else // defined (TOOLCHAIN_TYPE_Shared)
        #define _LIB_THEKOGANS_MAKE_CORE_DECL
    #endif // defined (TOOLCHAIN_TYPE_Shared)
    #if defined (_MSC_VER)
        #pragma warning (disable: 4251)  // using non-exported as public in exported
        #pragma warning (disable: 4786)
    #endif // defined (_MSC_VER)
#else // defined (TOOLCHAIN_OS_Windows)
    #define _LIB_THEKOGANS_MAKE_CORE_API
    #define _LIB_THEKOGANS_MAKE_CORE_DECL
#endif // defined (TOOLCHAIN_OS_Windows)

/// \def THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN(type)
/// A convenient macro to suppress copy construction and assignment.
#define THEKOGANS_MAKE_CORE_DISALLOW_COPY_AND_ASSIGN(type)\
private:\
    type (const type &);\
    type &operator = (const type &);

/// \def THEKOGANS_MAKE_CORE
/// Logging subsystem name.
#define THEKOGANS_MAKE_CORE "thekogans_make_core"

#endif // !defined (__thekogans_make_core_Config_h)
