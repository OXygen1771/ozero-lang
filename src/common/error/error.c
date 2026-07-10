#include "error.h"
#include "platform/error.h"

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

    // Other
    case OZ_ERR_UNKNOWN:
    default:
        return "unknown error";
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

    case OZ_ERR_UNKNOWN:
    default:
        return (size_t)snprintf(buf, buf_size, "%s (code %d)", base_msg,
                                err->code);
    }
}

OzeroError oz_error_get_io_error_from_system(const char *path) {
    i32 platform_error = oz_platform_err_get_last_error();
    OzeroErrorCode code = oz_platform_err_to_code(platform_error);

    OzeroError err = {.code = code,
                      .payload.io = {.path = path, .os_errno = platform_error}};
    return err;
}
