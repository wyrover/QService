#include "sigar.h"

static sigar_version_t sigar_version = {
    __DATE__,
    "@SCM_REVISION@",
    "libsigar 1.6.2",
    "",
    "",
    "",
    "SIGAR-1.6.2, "
    "SCM revision @SCM_REVISION@, "
    "built "__DATE__,
    1,
    6,
    2,
    0
};

SIGAR_DECLARE(sigar_version_t *) sigar_version_get(void)
{
    return &sigar_version;
}
