#include "oz_utf8.h"
#include <stddef.h>

// Validate that a single UTF-8 sequence starting at `bytes` is valid, with
// `available` bytes remaining to end of input, including the current byte.
// Returns true if the sequence is valid, false otherwise.
//
// https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/#G31703
static bool oz__utf8_validate_one_sequence(const u8 *bytes, // NOLINT
                                           size_t available) {
    u8 b0 = bytes[0];

    // 1-byte: ASCII 0xxxxxxx, U+0000..U+007F
    if (b0 <= 0x7F) {
        return true;
    }

    // 2-byte sequence: 110xxxxx 10xxxxxx, U+0080..U+07FF
    if ((b0 & 0xE0) == 0xC0) {
        if (available < 2) {
            return false;
        }
        u8 b1 = bytes[1];

        // second byte must start with 10..
        if ((b1 & 0xC0) != 0x80) {
            return false;
        }
        // overlong encoding, disallowed
        // U+0080..U+07FF bytes: C2..DF, 80..BF
        if (b0 < 0xC2) {
            return false;
        }
        return true;
    }

    // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxxx
    // U+0800..U+0FFF, U+1000..U+CFFF, U+D000..U+D7FF, U+E000..U+FFFF
    if ((b0 & 0xF0) == 0xE0) {
        if (available < 3) {
            return false;
        }
        // both must start with 10..
        u8 b1 = bytes[1];
        u8 b2 = bytes[2];
        if ((b1 & 0xC0) != 0x80) {
            return false;
        }
        if ((b2 & 0xC0) != 0x80) {
            return false;
        }

        // overlong encoding, disallowed
        // U+0800..U+0FFF bytes: E0, A0..BF, 80..BF
        if (b0 == 0xE0 && b1 < 0xA0) {
            return false;
        }

        // U+1000..U+CFFF bytes: E1..EC, 80..BF, 80..BF

        // UTF-16 surrogates and overlong encoding, disallowed
        // U+D000..U+D7FF bytes: ED, 80..9F, 80..BF
        if (b0 == 0xED && b1 > 0x9F) {
            return false;
        }

        // U+E000..U+FFFF
        return true;
    }

    // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    // U+10000..U+3FFFF, U+40000..U+FFFFF, U+100000..U+10FFFF
    if ((b0 & 0xF8) == 0xF0) {
        if (available < 4) {
            return false;
        }
        // all must start with 10..
        u8 b1 = bytes[1];
        u8 b2 = bytes[2];
        u8 b3 = bytes[3];
        if ((b1 & 0xC0) != 0x80) {
            return false;
        }
        if ((b2 & 0xC0) != 0x80) {
            return false;
        }
        if ((b3 & 0xC0) != 0x80) {
            return false;
        }

        // overlong encoding, disallowed
        // U+10000..U+3FFFF bytes: F0, 90..BF, 80..BF, 80..BF
        if (b0 == 0xF0 && b1 < 0x90) {
            return false;
        }

        // U+40000..U+FFFFF bytes: F1..F3, 80..BF, 80..BF, 80..BF

        // Unicode17 has no codepoints above U+10FFFF
        if (b0 > 0xF4) {
            return false;
        }
        if (b0 == 0xF4 && b1 > 0x8F) {
            return false;
        }

        return true;
    }

    // lead byte invalid
    return false;
}

// Get the expected UTF-8 sequence length from its lead byte.
// Returns 0 if the lead byte is invalid.
static inline size_t oz__utf8_expected_sequence_length(u8 lead) {
    if (lead <= 0x7F) {
        return 1;
    }
    if ((lead & 0xE0) == 0xC0) {
        return 2;
    }
    if ((lead & 0xF0) == 0xE0) {
        return 3;
    }
    if ((lead & 0xF8) == 0xF0) {
        return 4;
    }
    return 0;
}

size_t oz_utf8_char_count(OzeroStringSlice str) {
    size_t count = 0;
    for (size_t i = 0; i < str.length; i++) {
        // count only non-continuation bytes
        if (((u8)str.bytes[i] & 0xC0) != 0x80) {
            count++;
        }
    }

    return count;
}

bool oz_utf8_is_valid(OzeroStringSlice str) {
    const u8 *bytes = (const u8 *)str.bytes;
    size_t offset = 0;

    while (offset < str.length) {
        size_t seq_len = oz__utf8_expected_sequence_length(bytes[offset]);

        // lead byte invalid
        if (seq_len == 0) {
            return false;
        }

        size_t available = str.length - offset;
        if (!oz__utf8_validate_one_sequence(bytes + offset, available)) {
            return false;
        }

        offset += seq_len;
    }

    return true;
}
