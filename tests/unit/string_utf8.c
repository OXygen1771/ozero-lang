#include "string/oz_string.h"
#include "string/oz_utf8.h"

#include "minunit.h"

// valid utf-8 suite

MU_TEST(utf8_valid_ascii) {
    // capital letters here, why not
    const OzeroStringSlice str = oz_string_slice_from_cstr("Hello, World!");
    mu_assert(oz_utf8_is_valid(str), "ASCII should be valid UTF-8");
    mu_assert(oz_utf8_char_count(str) == 13, "ASCII char count should be 13");
}

MU_TEST(utf8_valid_multibyte) {
    const OzeroStringSlice cyr = oz_string_slice_from_cstr("Привет, мир!");
    mu_assert(oz_utf8_is_valid(cyr), "cyrillic should be valid UTF-8");
    mu_assert(oz_utf8_char_count(cyr) == 12,
              "cyrillic char count should be 12");

    const OzeroStringSlice chi = oz_string_slice_from_cstr("你好世界");
    mu_assert(oz_utf8_is_valid(chi), "chinese should be valid UTF-8");
    mu_assert(oz_utf8_char_count(chi) == 4, "chinese char count should be 4");

    const OzeroStringSlice empty = oz_string_slice_from_cstr("");
    mu_assert(oz_utf8_is_valid(empty), "empty string should be valid UTF-8");
    mu_assert(oz_utf8_char_count(empty) == 0, "empty char count should be 0");

    const OzeroStringSlice emoji = oz_string_slice_from_cstr("😁");
    mu_assert(oz_utf8_is_valid(emoji), "emoji should be valid");
    mu_assert(oz_utf8_char_count(emoji) == 1, "emoji char count should be 1");

    const OzeroStringSlice mixed = oz_string_slice_from_cstr("J😁ы");
    mu_assert(oz_utf8_is_valid(mixed), "mixed should be valid");
    mu_assert(oz_utf8_char_count(mixed) == 3, "mixed char count should be 3");
}

MU_TEST_SUITE(utf8_valid) {
    MU_RUN_TEST(utf8_valid_ascii);
    MU_RUN_TEST(utf8_valid_multibyte);
}

// invalid utf-8 suite

MU_TEST(utf8_invalid_truncated) {
    const char truncated2[] = {(char)0xC2};
    const OzeroStringSlice s_tr2 = oz_string_slice_from_bytes(truncated2, 1);
    mu_assert(!oz_utf8_is_valid(s_tr2),
              "truncated 2-byte sequence should be invalid");

    const char truncated3[] = {(char)0xE0, (char)0xA0};
    const OzeroStringSlice s_tr3 = oz_string_slice_from_bytes(truncated3, 2);
    mu_assert(!oz_utf8_is_valid(s_tr3),
              "truncated 3-byte sequence should be invalid");

    const char truncated4[] = {(char)0xF0, (char)0x90, (char)0x80};
    const OzeroStringSlice s_tr4 = oz_string_slice_from_bytes(truncated4, 3);
    mu_assert(!oz_utf8_is_valid(s_tr4),
              "truncated 4-byte sequence should be invalid");
}

MU_TEST(utf8_invalid_overlong) {
    // overlong encoding of '/' (0x2F): 0xC0 0xAF (should be 0x2F)
    const char overlong2[] = {(char)0xC0, (char)0xAF};
    const OzeroStringSlice s_ol2 = oz_string_slice_from_bytes(overlong2, 2);
    mu_assert(!oz_utf8_is_valid(s_ol2), "overlong 2-byte should be invalid");

    const char overlong3[] = {(char)0xE0, (char)0x80, (char)0xAF};
    const OzeroStringSlice s_ol3 = oz_string_slice_from_bytes(overlong3, 3);
    mu_assert(!oz_utf8_is_valid(s_ol3), "overlong 3-byte should be invalid");
}

MU_TEST(utf8_invalid_surrogate) {
    // high surrogate U+D800: 0xED 0xA0 0x80
    const char surr[] = {(char)0xED, (char)0xA0, (char)0x80};
    const OzeroStringSlice s_surr = oz_string_slice_from_bytes(surr, 3);
    mu_assert(!oz_utf8_is_valid(s_surr),
              "UTF-16 surrogate should be invalid in UTF-8");
}

MU_TEST(utf8_invalid_out_of_bounds) {
    // codepoint > U+10FFFF: 0xF4 0x90 0x80 0x80
    const char oob[] = {(char)0xF4, (char)0x90, (char)0x80, (char)0x80};
    const OzeroStringSlice s_oob = oz_string_slice_from_bytes(oob, 4);
    mu_assert(!oz_utf8_is_valid(s_oob),
              "codepoint > U+10FFFF should be invalid in Unicode 17");

    // invalid lead byte, promising 5-byte sequence. Disallowed in UTF-8
    const char lead5[] = {(char)0xF8, (char)0x80, (char)0x80, (char)0x80,
                          (char)0x80};
    const OzeroStringSlice str5 = oz_string_slice_from_bytes(lead5, 5);
    mu_assert(!oz_utf8_is_valid(str5), "5-byte sequence should be invalid");
}

MU_TEST(utf8_invalid_stray_continuation) {
    // continuation byte (10xxxxxx) without lead byte
    const char stray_cont_byte[] = {(char)0x80};
    const OzeroStringSlice str_stray =
        oz_string_slice_from_bytes(stray_cont_byte, 1);
    mu_assert(
        !oz_utf8_is_valid(str_stray),
        "stray continuation byte should be invalid as a start of a sequence");

    const char stray_multi[] = {(char)0xBF, (char)0xBF};
    const OzeroStringSlice str_stray_multi =
        oz_string_slice_from_bytes(stray_multi, 2);
    mu_assert(!oz_utf8_is_valid(str_stray_multi),
              "multiple stray continuation bytes should be invalid as a start "
              "of a sequence");
}

MU_TEST_SUITE(utf8_invalid) {
    MU_RUN_TEST(utf8_invalid_truncated);
    MU_RUN_TEST(utf8_invalid_overlong);
    MU_RUN_TEST(utf8_invalid_surrogate);
    MU_RUN_TEST(utf8_invalid_out_of_bounds);
    MU_RUN_TEST(utf8_invalid_stray_continuation);
}

// Original text by Markus Kuhn
// https://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html

MU_TEST(kuhn_valid_1_to_2) {
    // This is just one test, though, it could be separated into multiple

    const OzeroStringSlice test_text = oz_string_slice_from_cstr(
        "1  Some correct UTF-8 text"
        "You should see the Greek word 'kosme':       \"κόσμε\""
        "2.1  First possible sequence of a certain length"
        "2.1.1  1 byte  (U-00000000):        \"\"\n2.1.2  2 bytes "
        "(U-00000080):        \"\"\n2.1.3  3 bytes (U-00000800):        "
        "\"ࠀ\"\n2.1.4  4 bytes (U-00010000):        \"𐀀\"\n2.1.5  5 bytes "
        "(U-00200000):        \"�����\"\n2.1.6  6 bytes (U-04000000):        "
        "\"������\"\n"
        "2.2  Last possible sequence of a certain length"
        "2.2.1  1 byte  (U-0000007F):        \"\"\n2.2.2  2 bytes "
        "(U-000007FF):        \"߿\"\n2.2.3  3 bytes (U-0000FFFF):        "
        "\"￿\"\n2.2.4  4 bytes (U-001FFFFF):        \"����\"\n2.2.5  "
        "5 "
        "bytes "
        "(U-03FFFFFF):        \"�����\"\n2.2.6  6 bytes (U-7FFFFFFF):        "
        "\"������\"\n        "
        "2.3  Other boundary conditions"
        "2.3.1  U-0000D7FF = ed 9f bf = \"퟿\"\n2.3.2  U-0000E000 = ee 80 80 "
        "= \"\"\n2.3.3  U-0000FFFD = ef bf bd = \"�\"\n2.3.4  U-0010FFFF "
        "= "
        "f4 8f bf bf = \"􏿿\"\n2.3.5  U-00110000 = f4 90 80 80 = "
        "\"����\"\n");
    mu_assert(oz_utf8_is_valid(test_text), "This should all be valid UTF-8");
}

// *some* invalid utf-8

MU_TEST(kuhn_3_1_unexpected_continuation) {
    // 3.1.1 First continuation byte 0x80
    char bytes1[] = {(char)0x80};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "3.1.1: 0x80 should be invalid as a start of a sequence");

    // 3.1.2 Last continuation byte 0xbf
    char bytes2[] = {(char)0xbf};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "3.1.2: 0xbf should be invalid as a start of a sequence");

    // 3.1.9 Sequence of all 64 possible continuation bytes (0x80-0xbf)
    char b_all[64];
    for (int i = 0; i < 64; i++) {
        b_all[i] = (char)(0x80 + i);
    }
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(b_all, sizeof(b_all))),
        "3.1.9: 64 continuation bytes should be invalid as a start of a "
        "sequence");
}

MU_TEST(kuhn_3_2_lonely_start) {
    // 3.2.1 Lonely start characters (lowest 0xc0)
    char bytes1[] = {(char)0xc0, ' '};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "3.2.1: lonely 0xC0 should be invalid");

    // 3.2.2 Lonely start of 3-byte sequence (lowest 0xe0)
    char bytes2[] = {(char)0xe0, ' '};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "3.2.2: lonely 0xE0 should be invalid");
}

MU_TEST(kuhn_3_3_missing_last_byte) {
    // 3.3.1 2-byte sequence with last byte missing (U+0000)
    char bytes1[] = {(char)0xc0};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "3.3.1: truncated 2-byte sequence should be invalid");

    // 3.3.8 4-byte sequence with last byte missing
    char bytes2[] = {(char)0xf0, (char)0x80, (char)0x80};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "3.3.8: truncated 4-byte sequence should be invalid");
}

MU_TEST(kuhn_3_5_impossible_bytes) {
    // 3.5.1 fe
    char bytes1[] = {(char)0xfe};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "3.5.1: 0xFE is impossible in UTF-8");

    // 3.5.2 ff
    char bytes2[] = {(char)0xff};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "3.5.2: 0xFF is impossible in UTF-8");
}

MU_TEST(kuhn_4_overlong_sequences) {
    // 4.1.1 Overlong ASCII slash '/' (U+002F): c0 af
    char bytes1[] = {(char)0xc0, (char)0xaf};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "4.1.1: overlong slash (2-byte)");

    // 4.1.2 Overlong slash: e0 80 af
    char bytes2[] = {(char)0xe0, (char)0x80, (char)0xaf};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "4.1.2: overlong slash (3-byte)");

    // 4.2.2 Maximum overlong 2-byte: c1 bf (U+007F)
    char bytes3[] = {(char)0xc1, (char)0xbf};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes3, sizeof(bytes3))),
        "4.2.2: max overlong 2-byte");

    // 4.3.1 Overlong NUL: c0 80
    char bytes4[] = {(char)0xc0, (char)0x80};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes4, sizeof(bytes4))),
        "4.3.1: overlong NUL");
}

MU_TEST(kuhn_5_illegal_code_positions) {
    // 5.1.1 Single UTF-16 surrogate U+D800: ed a0 80
    char bytes1[] = {(char)0xed, (char)0xa0, (char)0x80};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes1, sizeof(bytes1))),
        "5.1.1: High surrogate U+D800");

    // 5.1.7 Single UTF-16 surrogate U+DFFF: ed bf bf
    char bytes2[] = {(char)0xed, (char)0xbf, (char)0xbf};
    mu_assert(
        !oz_utf8_is_valid(oz_string_slice_from_bytes(bytes2, sizeof(bytes2))),
        "5.1.7: Low surrogate U+DFFF");

    // 5.3.1 Noncharacter U+FFFE: ef bf be
    // NOTE: these are noncharacters in Unicode, but, according to the UTF-8
    // spec, these should still be well-formed UTF-8 sequences
    char bytes3[] = {(char)0xef, (char)0xbf, (char)0xbe};
    mu_assert(
        oz_utf8_is_valid(oz_string_slice_from_bytes(bytes3, sizeof(bytes3))),
        "5.3.1: U+FFFE is well-formed UTF-8 (but noncharacter)");
    char bytes4[] = {(char)0xef, (char)0xbf, (char)0xbf};
    mu_assert(
        oz_utf8_is_valid(oz_string_slice_from_bytes(bytes4, sizeof(bytes4))),
        "5.3.2: U+FFFF is well-formed UTF-8 (but noncharacter)");
}

MU_TEST_SUITE(kuhn_suite) {
    MU_RUN_TEST(kuhn_valid_1_to_2);
    MU_RUN_TEST(kuhn_3_1_unexpected_continuation);
    MU_RUN_TEST(kuhn_3_2_lonely_start);
    MU_RUN_TEST(kuhn_3_3_missing_last_byte);
    MU_RUN_TEST(kuhn_3_5_impossible_bytes);
    MU_RUN_TEST(kuhn_4_overlong_sequences);
    MU_RUN_TEST(kuhn_5_illegal_code_positions);
}

int main() {
    MU_RUN_SUITE(utf8_valid);
    MU_REPORT();
    MU_RUN_SUITE(utf8_invalid);
    MU_REPORT();
    return MU_EXIT_CODE;
}
