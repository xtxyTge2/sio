#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef SIO_USE_URING
#include <liburing.h>
#endif // SIO_USE_URING

struct sio_string {
	size_t length;
	char *chars;
};

struct sio_path {
	struct sio_string path_str;
};

struct sio_file {
	int ret_p;
	FILE *file;
};

struct sio_context {
	bool ok;
#ifdef SIO_USE_URING
	struct io_uring ring;
	int flags;
#endif // SIO_USE_URING
};

/* SIO_FILE */
struct sio_file *sio_file_new(void);
void sio_file_free(struct sio_file *p);
struct sio_string *sio_read_file(struct sio_context *ctx,
				 struct sio_file *file);

/* SIO_PATH */
struct sio_path *sio_path_new(void);
void sio_path_free(struct sio_path *p);
struct sio_path *sio_path_from_c_str(const char *s);

/* SIO_STRING */
struct sio_string *sio_string_new(void);
void sio_string_free(struct sio_string *s);
void sio_string_copy_from_chars(struct sio_string *s, char const *d);
void sio_string_copy_from_chars_with_length(struct sio_string *s, char const *d,
					    size_t length);
void sio_string_take_from_chars(struct sio_string *s, char *data);

/* SIO_CONTEXT */
struct sio_context *sio_context_init(void);
void sio_context_destroy(struct sio_context *ctx);
struct sio_file *sio_open(struct sio_context *ctx, struct sio_path *path,
			  const char *mode);
void sio_close(struct sio_context *ctx, struct sio_file *file);
