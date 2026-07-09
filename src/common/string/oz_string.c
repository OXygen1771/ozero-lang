#include "oz_string.h"

// TODO: implement without stdlib
#include <stdlib.h>
#include <string.h>

#define OZ_STRING_NULL ((OzeroString){.length = 0, .bytes = nullptr})
#define OZ_STRING_SLICE_NULL ((OzeroStringSlice){.length = 0, .bytes = nullptr})

OzeroString oz_string_from_cstr(const char *cstr) {
    if (cstr == nullptr) {
        return OZ_STRING_NULL;
    }

    size_t len = strlen(cstr);
    return oz_string_from_bytes(cstr, len);
}

OzeroString oz_string_from_bytes(const char *bytes, size_t length) {
    if (bytes == nullptr && length > 0) {
        return OZ_STRING_NULL;
    }

    char *buf = (char *)malloc(length + 1);
    if (buf == nullptr) {
        return OZ_STRING_NULL;
    }

    if (length > 0) {
        memcpy(buf, bytes, length);
    }
    buf[length] = '\0';

    return (OzeroString){.length = length, .bytes = buf};
}

void oz_string_free(OzeroString *str) {
    if (str == nullptr) {
        return;
    }
    if (str->bytes != nullptr) {
        free(str->bytes);
    }
    *str = OZ_STRING_NULL;
}

OzeroStringSlice oz_string_slice_from_cstr(const char *cstr) {
    if (cstr == nullptr) {
        return OZ_STRING_SLICE_NULL;
    }

    size_t len = strlen(cstr);
    return (OzeroStringSlice){.length = len, .bytes = cstr};
}

bool oz_strings_equal(OzeroStringSlice sl_a, OzeroStringSlice sl_b) {
    if (sl_a.length != sl_b.length) {
        return false;
    }
    if (sl_a.length == 0) {
        return true; // empty slices are equal
    }
    if (sl_a.bytes == sl_b.bytes) {
        return true;
    }
    return memcmp(sl_a.bytes, sl_b.bytes, sl_a.length) == 0;
}
