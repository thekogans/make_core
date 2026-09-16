#include "thekogans/make/core/resolve/Version.h"

using namespace thekogans;

namespace thekogans {
    namespace make {
        namespace core {
            namespace resolve {

                const util::Version &GetVersion () {
                    static const util::Version *version = new util::Version (
                        THEKOGANS_MAKE_CORE_RESOLVE_MAJOR_VERSION,
                        THEKOGANS_MAKE_CORE_RESOLVE_MINOR_VERSION,
                        THEKOGANS_MAKE_CORE_RESOLVE_PATCH_VERSION);
                    return *version;
                }

            } // namespace resolve
        } // namespace core
    } // namespace make
} // namespace thekogans
