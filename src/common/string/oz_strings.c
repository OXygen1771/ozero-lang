#include "oz_strings.h"

#include <string.h> // TODO: implement without stdlib

bool oz_strings_equal(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}
