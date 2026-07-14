#include "error.h"
#include "platform/error.h"

#include <stddef.h>
#include <stdio.h>

const char *oz_error_get_associated_message(const OzeroErrorCode code) {
    switch (code) {
    case OZ_OK:
        return "success";

    // I/O errors
    case OZ_ERR_IO:
        return "I/O error";
    case OZ_ERR_FILE_NOT_FOUND:
        return "file not found";
    case OZ_ERR_PERMISSION_DENIED:
        return "permission denied";
    case OZ_ERR_IS_DIRECTORY:
        return "is a directory";
    case OZ_ERR_NOT_DIRECTORY:
        return "not a directory";
    case OZ_ERR_ALREADY_EXISTS:
        return "already exists";
    case OZ_ERR_NO_SPACE:
        return "no free space left";

    // Memory errors
    case OZ_ERR_OOM:
        return "out of memory";
    case OZ_ERR_INVALID_ALLOCATION_ARGUMENT:
        return "invalid allocation argument";
    case OZ_ERR_UNSUPPORTED_ALLOCATION_OPERATION:
        return "unsupported allocation operation";

    // Argument errors
    case OZ_ERR_INVALID_CMD_ARGUMENT:
        return "invalid argument";

    // Other
    case OZ_ERR_UNKNOWN:
    default:
        return "unknown error";
    }
}

// might move to the allocation_error.h header, don't see the need
static const char *
oz__alloc_operation_name(OzeroAllocationOperation op) { // NOLINT
    switch (op) {
    case OZ_ALLOC_OP_ALLOC:
        return "alloc";
    case OZ_ALLOC_OP_FREE:
        return "free";
    case OZ_ALLOC_OP_REALLOC:
        return "realloc";
    case OZ_ALLOC_OP_FREE_ALL:
        return "free_all";
    default:
        return "unknown";
    }
}

size_t oz_error_format_message(const OzeroError *err, char *buf,
                               size_t buf_size) {
    if (err == nullptr || buf == nullptr || buf_size == 0) {
        return 0;
    }

    const char *base_msg = oz_error_get_associated_message(err->code);

    switch (err->code) {
    case OZ_OK:
        return (size_t)snprintf(buf, buf_size, "%s", base_msg);

    // I/O errors: path + platform error message
    case OZ_ERR_IO:
    case OZ_ERR_FILE_NOT_FOUND:
    case OZ_ERR_PERMISSION_DENIED:
    case OZ_ERR_IS_DIRECTORY:
    case OZ_ERR_NOT_DIRECTORY:
    case OZ_ERR_ALREADY_EXISTS:
    case OZ_ERR_NO_SPACE: {
        const char *path =
            err->payload.io.path ? err->payload.io.path : "<unknown>";
        const i32 os_errno = err->payload.io.os_errno;
        const size_t sys_msg_buf_size = 256;

        char sys_msg[sys_msg_buf_size];
        oz_platform_err_format_system_error(os_errno, sys_msg, sizeof(sys_msg));

        return (size_t)snprintf(buf, buf_size, "%s: %s (error %d: %s)",
                                base_msg, path, os_errno, sys_msg);
    }

    case OZ_ERR_OOM:
        return (size_t)snprintf(buf, buf_size,
                                "%s: failed to allocate %zu bytes", base_msg,
                                err->payload.oom.requested_size);
    case OZ_ERR_INVALID_ALLOCATION_ARGUMENT:
        return (size_t)snprintf(buf, buf_size, "%s: size=%zu, align=%zu",
                                base_msg,
                                err->payload.invalid_alloc_arg.requested_size,
                                err->payload.invalid_alloc_arg.requested_align);

    case OZ_ERR_UNSUPPORTED_ALLOCATION_OPERATION:
        return (size_t)snprintf(
            buf, buf_size, "%s: operation '%s' is not supported", base_msg,
            oz__alloc_operation_name(
                err->payload.unsupported_alloc_op.operation));

    case OZ_ERR_INVALID_CMD_ARGUMENT:
    case OZ_ERR_UNKNOWN: {
        const char *msg = err->payload.generic.message;
        if (msg) {
            return (size_t)snprintf(buf, buf_size, "%s: %s", base_msg, msg);
        }
        return (size_t)snprintf(buf, buf_size, "%s", base_msg);
    }

    default:
        return (size_t)snprintf(buf, buf_size, "%s (code %d)", base_msg,
                                err->code);
    }
}

OzeroError oz_error_get_io_error_from_system(const char *path) {
    i32 platform_error = oz_platform_err_get_last_error();
    OzeroErrorCode code = oz_platform_err_to_code(platform_error);

    OzeroError err = {};
    err.code = code;

    if (code == OZ_ERR_OOM) {
        // unknown here
        err.payload.oom.requested_size = 0;
        err.payload.oom.requested_align = 0;
    } else {
        err.payload.io.path = path;
        err.payload.io.os_errno = platform_error;
    }
    return err;
}
