#pragma once

#include "defines.h"

// Error codes for internal errors.
// User-facing errors will use common/diagnostics later
typedef enum {
    OZ_OK = 0,

    // I/O errors
    OZ_ERR_IO,                //!< Generic I/O error.
    OZ_ERR_FILE_NOT_FOUND,    //!< File or directory does not exist.
    OZ_ERR_PERMISSION_DENIED, //!< Permission denied.
    OZ_ERR_IS_DIRECTORY,      //!< Expected file, got directory.
    OZ_ERR_NOT_DIRECTORY,     //!< Expected directory, got file.
    OZ_ERR_ALREADY_EXISTS,    //!< File or directory already exists.
    OZ_ERR_NO_SPACE,          //!< No free space.

    // Memory errors
    OZ_ERR_OOM, //!< Out of memory.

    // Other
    OZ_ERR_UNKNOWN, //!< Generic error.
} OzeroErrorCode;

// Tagged union for detailed error information.
typedef struct {
    OzeroErrorCode code;

    union {
        // I/O errors
        struct {
            const char *path; //!< File path.
            i32 os_errno;     //!< Platform/OS error code.
        } io;

        // Memory errors
        struct {
            size_t requested_size; //!< Bytes that failed to allocate.
        } oom;

        // Generic and custom message errors.
        struct {
            const char *message; //!< Need to figure out allocation.
        } generic;
    } payload;
} OzeroError;

// Get human-readable message for a given error code.
const char *oz_error_get_associated_message(OzeroErrorCode code);

// Format error with payload details into a buffer.
// Returns the number of characters written, excluding the null terminator.
size_t oz_error_format_message(const OzeroError *err, char *buf,
                               size_t buf_size);

// Get the I/O error associated with the given path.
// NOTE: the path isn't copied, the argument MUST outlive the error.
OzeroError oz_error_get_io_error_from_system(const char *path);
