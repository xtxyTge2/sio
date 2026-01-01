#include "unity.h"
#include <string.h>
#include <sio/sio.h>

void setUp(void)
{
}
void tearDown(void)
{
}

static bool write_test_file(const char *path, const char *content)
{
	FILE *f = fopen(path, "w");
	if (!f) {
		perror("fopen");
		return false;
	}

	const size_t length = strlen(content);
	const size_t nitems = fwrite(content, sizeof(char), length, f);
	if (nitems != length) {
		fclose(f);
		fprintf(stderr, "fwrite only wrote: %ld items, expected: %ld\n",
			nitems, length);
		return false;
	}

	fclose(f);
	return true;
}

/* SIO_STRING */
void test_string_new(void)
{
	struct sio_string *s = sio_string_new();
	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 0);
	TEST_ASSERT_NULL(s->chars);
	sio_string_free(s);
}

void test_string_copy_from_chars(void)
{
	struct sio_string *s = sio_string_new();
	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 0);
	TEST_ASSERT_NULL(s->chars);

	char const *data = "chars";
	sio_string_copy_from_chars(s, data);

	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 5);
	TEST_ASSERT_NOT_NULL(s->chars);

	TEST_ASSERT_EQUAL(s->chars[0], 'c');
	TEST_ASSERT_EQUAL(s->chars[1], 'h');
	TEST_ASSERT_EQUAL(s->chars[2], 'a');
	TEST_ASSERT_EQUAL(s->chars[3], 'r');
	TEST_ASSERT_EQUAL(s->chars[4], 's');
	TEST_ASSERT_EQUAL(s->chars[5], '\0');

	sio_string_free(s);
}

void test_string_copy_empty(void)
{
	struct sio_string *s = sio_string_new();
	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 0);
	TEST_ASSERT_NULL(s->chars);

	const char *empty = nullptr;
	sio_string_copy_from_chars(s, empty);

	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 0);
	TEST_ASSERT_NULL(s->chars);

	sio_string_free(s);
}

void test_string_copy_from_chars_with_length(void)
{
	struct sio_string *s = sio_string_new();
	TEST_ASSERT_NOT_NULL(s);
	TEST_ASSERT_EQUAL(s->length, 0);
	TEST_ASSERT_NULL(s->chars);

	const char *data = "hello";
	const size_t len = 3;
	sio_string_copy_from_chars_with_length(s, data, len);

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

	TEST_ASSERT_NOT_EQUAL(s->chars, data);

	sio_string_free(s);
}

/* SIO_PATH */
void test_sio_path_new(void)
{
	struct sio_path *path = sio_path_new();

	TEST_ASSERT_NOT_NULL(path);
	TEST_ASSERT_NULL(path->path_str.chars);
	TEST_ASSERT_EQUAL(path->path_str.length, 0);

	sio_path_free(path);
}

void test_sio_path_from_c_str(void)
{
	const char *data = "/this/is/a//path";
	struct sio_path *path = sio_path_from_c_str(data);

	TEST_ASSERT_NOT_NULL(path);
	TEST_ASSERT_NOT_NULL(path->path_str.chars);
	TEST_ASSERT_EQUAL(path->path_str.length, 16);
	TEST_ASSERT_EQUAL(path->path_str.length, strlen(data));
	for (size_t i = 0; i < path->path_str.length; i++) {
		TEST_ASSERT_EQUAL(path->path_str.chars[i], data[i]);
	}
	TEST_ASSERT_EQUAL(path->path_str.chars[path->path_str.length], '\0');
	sio_path_free(path);
}

/* SIO_FILE */
void test_open_non_existent(void)
{
	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);

	struct sio_path *path = sio_path_from_c_str("/path/that/does/not/exist");
	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NULL(file);

	sio_path_free(path);
	sio_context_destroy(ctx);
}

void test_read_small(void)
{
	const char *test_path = "test_sio_linux.txt";
	remove(test_path);

	const char *content = "this is a test\nwith two lines";
	TEST_ASSERT_TRUE(write_test_file(test_path, content));

	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);

	struct sio_path *path = sio_path_from_c_str(test_path);
	TEST_ASSERT_NOT_NULL(path);

	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NOT_NULL(file);
	TEST_ASSERT_NOT_NULL(file->file);

	struct sio_string *read_content = sio_read_file(ctx, file);
	TEST_ASSERT_NOT_NULL(read_content);
	TEST_ASSERT_NOT_NULL(read_content->chars);
	TEST_ASSERT_EQUAL(read_content->length, strlen(content));
	TEST_ASSERT_EQUAL_STRING(read_content->chars, content);
	TEST_ASSERT_EQUAL_STRING_LEN(read_content->chars, content,
				     strlen(content));

	sio_close(ctx, file);
	sio_string_free(read_content);
	sio_path_free(path);
	sio_context_destroy(ctx);

	TEST_ASSERT_EQUAL(remove(test_path), 0);
}

void test_close_nullptr(void)
{
	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);

	sio_close(ctx, nullptr);
	sio_context_destroy(ctx);
}

void test_open_close(void)
{
	const char *test_path = "test_sio_linux.txt";
	remove(test_path);
	const char *content = "123";
	TEST_ASSERT_TRUE(write_test_file(test_path, content));

	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);

	struct sio_path *path = sio_path_from_c_str(test_path);
	TEST_ASSERT_NOT_NULL(path);

	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NOT_NULL(file);
	TEST_ASSERT_NOT_NULL(file->file);

	sio_close(ctx, file);
	sio_path_free(path);
	sio_context_destroy(ctx);
	TEST_ASSERT_EQUAL(remove(test_path), 0);
}

void test_open_empty(void)
{
	const char *test_path = "test_sio_linux.txt";

	remove(test_path);
	FILE *f = fopen(test_path, "w");
	TEST_ASSERT_TRUE(f);
	TEST_ASSERT_EQUAL(fclose(f), 0);

	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);
	struct sio_path *path = sio_path_from_c_str(test_path);
	TEST_ASSERT_NOT_NULL(path);
	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NOT_NULL(file);
	struct sio_string *file_content = sio_read_file(ctx, file);
	TEST_ASSERT_NOT_NULL(file_content);
	TEST_ASSERT_EQUAL(file_content->length, 0);
	TEST_ASSERT_NULL(file_content->chars);

	sio_close(ctx, file);
	sio_path_free(path);
	sio_string_free(file_content);
	sio_context_destroy(ctx);
}

void test_read_4096_bytes(void)
{
	const char *test_path = "test_sio_linux.txt";

	remove(test_path);
	FILE *f = fopen(test_path, "w");
	TEST_ASSERT_TRUE(f);

	const size_t len = 4096;
	char content[len];
	for (size_t i = 0; i < len; i++) {
		content[i] = 'a';
		TEST_ASSERT_EQUAL(fputc(content[i], f), (int)content[i]);
	}
	TEST_ASSERT_EQUAL(fclose(f), 0);

	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);
	struct sio_path *path = sio_path_from_c_str(test_path);
	TEST_ASSERT_NOT_NULL(path);
	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NOT_NULL(file);
	struct sio_string *file_content = sio_read_file(ctx, file);
	TEST_ASSERT_NOT_NULL(file_content);
	TEST_ASSERT_EQUAL(file_content->length, len);
	TEST_ASSERT_NOT_NULL(file_content->chars);
	TEST_ASSERT_EQUAL_STRING_LEN(file_content->chars, content, len);

	sio_close(ctx, file);
	sio_path_free(path);
	sio_string_free(file_content);
	sio_context_destroy(ctx);
}

void test_read_null_bytes(void)
{
	const char *test_path = "test_sio_linux.txt";

	const char data[] = {'h', 'e', 'l', 'l', '\0', 'o', 'w', 'w'};
	const size_t data_len = 8;
	remove(test_path);
	FILE *f = fopen(test_path, "wb");
	TEST_ASSERT_TRUE(f);
	TEST_ASSERT_EQUAL(fwrite(data, sizeof(char), sizeof(data), f),
			  sizeof(data) / sizeof(char));
	TEST_ASSERT_EQUAL(fclose(f), 0);

	struct sio_context *ctx = sio_context_init();
	TEST_ASSERT_NOT_NULL(ctx);
	struct sio_path *path = sio_path_from_c_str(test_path);
	TEST_ASSERT_NOT_NULL(path);
	struct sio_file *file = sio_open(ctx, path, "r");
	TEST_ASSERT_NOT_NULL(file);
	struct sio_string *file_content = sio_read_file(ctx, file);
	TEST_ASSERT_NOT_NULL(file_content);
	TEST_ASSERT_EQUAL(file_content->length, data_len);
	TEST_ASSERT_NOT_NULL(file_content->chars);
	TEST_ASSERT_EQUAL_STRING_LEN(file_content->chars, data, data_len);

	sio_close(ctx, file);
	sio_path_free(path);
	sio_string_free(file_content);
	sio_context_destroy(ctx);
}

int main(void)
{
	UNITY_BEGIN();

	/* SIO_STRING */
	RUN_TEST(test_string_new);
	RUN_TEST(test_string_copy_from_chars);
	RUN_TEST(test_string_copy_empty);
	RUN_TEST(test_string_copy_from_chars_with_length);

	/* SIO_PATH */
	RUN_TEST(test_sio_path_new);
	RUN_TEST(test_sio_path_from_c_str);

	/* SIO_FILE */
	RUN_TEST(test_open_non_existent);
	RUN_TEST(test_open_close);
	RUN_TEST(test_read_small);
	RUN_TEST(test_read_4096_bytes);
	RUN_TEST(test_open_empty);
	RUN_TEST(test_close_nullptr);
	RUN_TEST(test_read_null_bytes);

	/* SIO_CONTEXT */
	RUN_TEST(test_open_non_existent);

	return UNITY_END();
}
