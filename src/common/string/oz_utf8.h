#pragma once

#include "oz_string.h"

// Count Unicode characters (codepoints) in a UTF-8 string.
// WARNING: does NOT validate the string encoding.
size_t oz_utf8_char_count(OzeroStringSlice str);

// Validate that a string is valid UTF-8.
bool oz_utf8_is_valid(OzeroStringSlice str);
