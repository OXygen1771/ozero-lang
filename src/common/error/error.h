#pragma once

#include "defines.h"
#include "error/allocation_error.h"

// Error codes for internal errors.
// User-facing errors will use common/diagnostics later
typedef enum OzeroErrorCode {
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
    OZ_ERR_OOM,                         //!< Out of memory.
    OZ_ERR_INVALID_ALLOCATION_ARGUMENT, //!< Invalid argument passed to
                                        //!< allocator, e.g. size=0, align=2k+1,
                                        //!< etc.
    OZ_ERR_UNSUPPORTED_ALLOCATION_OPERATION, //!< Operation is not supported for
                                             //!< this allocator.

    // Other
    OZ_ERR_UNKNOWN, //!< Generic error.
} OzeroErrorCode;

// Tagged union for detailed error information.
typedef struct OzeroError {
    OzeroErrorCode code;

    union {
        // I/O errors
        struct {
            const char *path; //!< File path.
            i32 os_errno;     //!< Platform/OS error code.
        } io;

        // Memory errors
        // OOM
        struct {
            size_t requested_size; //!< Bytes that failed to allocate.
            size_t requested_align;
        } oom;
        // Invalid allocation argument
        struct {
            size_t requested_size;  //!< Invalid size (0 or overflow).
            size_t requested_align; //!< Invalid align (not power of 2).
        } invalid_alloc_arg;
        // Unsupported operation
        struct {
            OzeroAllocationOperation operation;
        } unsupported_alloc_op;

        // Generic and custom message errors.
        struct {
            const char *message; //!< Custom message, MUST outlive the error.
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

// macros for convenience

// Generate an "Out of memory" error.
#define OZ_ERROR_MAKE_OOM(size, align)                                         \
    ((OzeroError){.code = OZ_ERR_OOM,                                          \
                  .payload.oom = {.requested_size = (size),                    \
                                  .requested_align = (align)}})

// Generate an "Invalid allocation argument" error.
#define OZ_ERROR_MAKE_INVALID_ALLOC_ARG(size, align)                           \
    ((OzeroError){.code = OZ_ERR_INVALID_ALLOCATION_ARGUMENT,                  \
                  .payload.invalid_alloc_arg = {.requested_size = (size),      \
                                                .requested_align = (align)}})

// Generate an "Unsupported allocation operation" error.
#define OZ_ERROR_MAKE_UNSUPPORTED_ALLOC_OP(op)                                 \
    ((OzeroError){.code = OZ_ERR_UNSUPPORTED_ALLOCATION_OPERATION,             \
                  .payload.unsupported_alloc_op = {.operation = (op)}})

// Generate a generic error.
#define OZ_ERROR_MAKE_GENERIC_MSG(msg_code, msg)                               \
    ((OzeroError){.code = (msg_code), .payload.generic = {.message = (msg)}})
