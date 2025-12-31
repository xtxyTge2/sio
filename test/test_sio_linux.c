#include "unity.h"
#include <sio/sio.h>

void setUp (void) {}
void tearDown (void) {}

void test_string_new (void) {
    sio_string *s = sio_string_new ();
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 0);
    TEST_ASSERT_NULL(s->chars);
    sio_string_free (s);
}

void test_string_copy_from_chars (void) {
    sio_string *s = sio_string_new ();
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 0);
    TEST_ASSERT_NULL(s->chars);

    char const *data = "chars";
    sio_string_copy_from_chars (s, data);

    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 5);
    TEST_ASSERT_NOT_NULL(s->chars);

    TEST_ASSERT_EQUAL(s->chars[0], 'c');
    TEST_ASSERT_EQUAL(s->chars[1], 'h');
    TEST_ASSERT_EQUAL(s->chars[2], 'a');
    TEST_ASSERT_EQUAL(s->chars[3], 'r');
    TEST_ASSERT_EQUAL(s->chars[4], 's');
    TEST_ASSERT_EQUAL(s->chars[5], '\0');

    sio_string_free (s);
}

void test_string_copy_empty (void) {
    sio_string *s = sio_string_new ();
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 0);
    TEST_ASSERT_NULL(s->chars);

    const char *empty = nullptr;
    sio_string_copy_from_chars (s, empty);

    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 0);
    TEST_ASSERT_NULL(s->chars);

    sio_string_free (s);
}

void test_string_copy_from_chars_with_length (void) {
    sio_string *s = sio_string_new ();
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 0);
    TEST_ASSERT_NULL(s->chars);

    const char *data = "hello";
    const size_t len = 3;
    sio_string_copy_from_chars_with_length (s, data, len);

    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(s->length, 3);
    TEST_ASSERT_NOT_NULL(s->chars);
    TEST_ASSERT_EQUAL(s->chars[0], 'h');
    TEST_ASSERT_EQUAL(s->chars[1], 'e');
    TEST_ASSERT_EQUAL(s->chars[2], 'l');
    TEST_ASSERT_EQUAL(s->chars[3], '\0');

    TEST_ASSERT_EQUAL(data[0], 'h');
    TEST_ASSERT_EQUAL(data[1], 'e');
    TEST_ASSERT_EQUAL(data[2], 'l');
    TEST_ASSERT_EQUAL(data[3], 'l');
    TEST_ASSERT_EQUAL(data[4], 'o');
    TEST_ASSERT_EQUAL(data[5], '\0');

    TEST_ASSERT_NOT_EQUAL (s->chars, data);

    sio_string_free (s);
}

int main (void) {
    UNITY_BEGIN();

    RUN_TEST(test_string_new);
    RUN_TEST(test_string_copy_from_chars);
    RUN_TEST(test_string_copy_empty);
    RUN_TEST(test_string_copy_from_chars_with_length);

    return UNITY_END();
}
