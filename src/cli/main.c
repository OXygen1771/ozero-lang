#include "defines.h"
#include "io/logger.h"
#include "io/terminal.h"
#include "oz_assertions.h"

int main() {
    oz_term_init();

    OZ_DEBUG("Hello, World! I am (not yet) an interpreter for OZeroLang!");

    i32 val = 0;
    OZ_TRACE("tracing: %d", val++);
    OZ_DEBUG("debugging: %d", val++);
    OZ_INFO("information: %d", val++);
    OZ_ASSERT(val > 0, "val must be > 0, but it was %d", val);
    val++;
    OZ_ASSERT_DEBUG(val > 3, "val must be > 3, but it was %d", val);

    OZ_WARN("warning");
    OZ_ERROR("error");
    OZ_FATAL("REALLY bad error");

    return 0;
}
