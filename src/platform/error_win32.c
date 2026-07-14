#include "error.h"

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

size_t oz_platform_err_format_system_error(int error_code, char *buf,
                                           size_t buf_size) {
    if (!buf || buf_size == 0) {
        return 0;
    }

    static const size_t MSG_SIZE = 256;
    char win_msg[MSG_SIZE];
    DWORD len = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
        (DWORD)error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), win_msg,
        sizeof(win_msg), nullptr);

    if (len == 0) {
        return (size_t)snprintf(buf, buf_size, "Unknown error %d", error_code);
    }

    // Trim trailing \r\n from FormatMessage
    while (len > 0 && (win_msg[len - 1] == '\n' || win_msg[len - 1] == '\r')) {
        win_msg[--len] = '\0';
    }

    return (size_t)snprintf(buf, buf_size, "%s", win_msg);
}

OzeroErrorCode oz_platform_err_to_code(int platform_error) {
    DWORD code = (DWORD)platform_error;

    switch (code) {
    case ERROR_SUCCESS:
        return OZ_OK;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return OZ_ERR_FILE_NOT_FOUND;
    case ERROR_ACCESS_DENIED:
        return OZ_ERR_PERMISSION_DENIED;
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
        return OZ_ERR_ALREADY_EXISTS;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        return OZ_ERR_OOM;
    case ERROR_HANDLE_DISK_FULL:
    case ERROR_DISK_FULL:
        return OZ_ERR_NO_SPACE;
    case ERROR_DIRECTORY:
        return OZ_ERR_IS_DIRECTORY;
    case ERROR_INVALID_PARAMETER:
        return OZ_ERR_INVALID_CMD_ARGUMENT;
    default:
        return OZ_ERR_IO;
    }
}

int oz_platform_err_get_last_error(void) { return (int)GetLastError(); }
