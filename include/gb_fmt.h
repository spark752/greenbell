// Greenbell header file to include the right fmt settings.
// Named to try and avoid confusion with the fmt header itself.
#ifndef GB_FMT_H
#define GB_FMT_H

// Currently this is header only but may be changed to static linked in the
// future to improve build times.
#define FMT_HEADER_ONLY
#include "../fmt/include/fmt/core.h"
#include "../fmt/include/fmt/color.h"
#include "../fmt/include/fmt/printf.h"

#endif
