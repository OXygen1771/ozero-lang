#pragma once

#include "error/error.h"
#include <stddef.h>

// Format a platform-specific error code into a human-readable message.
// Returns the number of characters written, excluding the null terminator.
size_t oz_platform_err_format_system_error(i32 error_code, char *buf,
                                           size_t buf_size);

// Map a platform-specific error code to OzeroErrorCode.
OzeroErrorCode oz_platform_err_to_code(i32 platform_error);

// Get the last platform-specific error code.
i32 oz_platform_err_get_last_error(void);
