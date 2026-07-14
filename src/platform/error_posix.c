#include "platform/error.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

size_t oz_platform_err_format_system_error(const i32 error_code, char *buf,
                                           const size_t buf_size) {
    if (buf == nullptr || buf_size == 0) {
        return 0;
    }
    const char *msg = strerror(error_code);
    return (size_t)snprintf(buf, buf_size, "%s", msg);
}

OzeroErrorCode oz_platform_err_to_code(i32 platform_error) {
    switch (platform_error) {
    case 0:
        return OZ_OK;
    case ENOENT:
        return OZ_ERR_FILE_NOT_FOUND;
    case EACCES:
    case EPERM:
        return OZ_ERR_PERMISSION_DENIED;
    case EISDIR:
        return OZ_ERR_IS_DIRECTORY;
    case ENOTDIR:
        return OZ_ERR_NOT_DIRECTORY;
    case EEXIST:
        return OZ_ERR_ALREADY_EXISTS;
#ifdef EDQUOT
    case EDQUOT:
#endif
    case ENOSPC:
        return OZ_ERR_NO_SPACE;
    case EINVAL:
        return OZ_ERR_INVALID_CMD_ARGUMENT;
    case ENOMEM:
        return OZ_ERR_OOM;
    default:
        return OZ_ERR_IO;
    }
}

i32 oz_platform_err_get_last_error(void) { return errno; }
