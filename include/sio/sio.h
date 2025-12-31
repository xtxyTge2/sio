#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct sio_string sio_string;
struct sio_string {
    size_t length;
    char *chars;
};

typedef struct sio_path sio_path;
struct sio_path {
    sio_string path_str;
};

typedef struct sio_file sio_file;
struct sio_file {
    int ret_p;
    FILE* file;
};

typedef struct sio_context sio_context;
struct sio_context {
    bool ok;
};


/* SIO_FILE */
sio_file* sio_file_new (void);
void sio_file_free (sio_file* p);
sio_string* sio_read_file (sio_context* ctx, sio_file *file);

/* SIO_PATH */
sio_path* sio_path_new (void);
void sio_path_free (sio_path* p);
sio_path* sio_path_from_c_str (const char* s);

/* SIO_STRING */
sio_string* sio_string_new (void);
void sio_string_free (sio_string* s);
void sio_string_copy_from_chars (sio_string* s, char const *d);
void sio_string_copy_from_chars_with_length (sio_string* s, char const *d, size_t length);
void sio_string_take_from_chars (sio_string* s, char *data);

/* SIO_CONTEXT */
sio_context* sio_context_init (void);
void sio_context_destroy (sio_context* ctx);
sio_file* sio_open (sio_context* ctx, sio_path* path, const char* mode);
void sio_close (sio_context* ctx, sio_file* file);
