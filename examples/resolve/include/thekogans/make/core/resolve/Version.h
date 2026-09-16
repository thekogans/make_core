#if !defined (__thekogans_make_core_resolve_Version_h)
#define __thekogans_make_core_resolve_Version_h

#include <thekogans/util/Version.h>

namespace thekogans {
    namespace make {
        namespace core {
            namespace resolve {

                /// \brief
                /// Return the compiled thekogans_make_core_resolve version.
                /// \return Compiled thekogans_make_core_resolve version.
                const thekogans::util::Version &GetVersion ();

            } // namespace resolve
        } // namespace core
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_core_resolve_Version_h)
