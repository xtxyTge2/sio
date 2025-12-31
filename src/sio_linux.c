#include <sio/sio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SIO_MALLOC(ptr, count) do {                        \
    typeof(ptr) *sio_ptr_ = &(ptr);                        \
    size_t sio_count_ = (count);                           \
    *sio_ptr_ = malloc (sio_count_ * sizeof (**sio_ptr_)); \
    if (!*sio_ptr_ && sio_count_ != 0) {                   \
        assert (false && "SIO_MALLOC failed");             \
        abort ();                                          \
    }                                                      \
} while (0)

#define SIO_CALLOC(ptr, count) do {                       \
    typeof(ptr) *sio_ptr_ = &(ptr);                       \
    size_t sio_count_ = (count);                          \
    *sio_ptr_ = calloc (sio_count_, sizeof (**sio_ptr_)); \
    if (!*sio_ptr_ && sio_count_ != 0) {                  \
        assert (false && "SIO_CALLOC failed");            \
        abort ();                                         \
    }                                                     \
} while (0)

#define SIO_REALLOC(ptr, count) do {                             \
    typeof(ptr) *sio_ptr_ = &(ptr);                              \
    size_t sio_count_ = (count);                                 \
    size_t sio_size_ = sio_count_ * sizeof (**sio_ptr_);         \
    typeof(*sio_ptr_) sio_tmp_ = realloc (*sio_ptr_, sio_size_); \
    if (! sio_tmp_ && sio_count_ != 0) {                         \
        assert (false && "SIO_REALLOC failed");                  \
        abort ();                                                \
    }                                                            \
    *sio_ptr_ = sio_tmp_;                                        \
} while (0)

#define SIO_REALLOCARRAY(ptr, count) do {                                                   \
    typeof(ptr) *sio_ptr_ = &(ptr);                                                         \
    size_t sio_count_ = (count);                                                            \
    typeof(*sio_ptr_) sio_tmp_ = reallocarray (*sio_ptr_, sio_count_, sizeof (**sio_ptr_)); \
    if (! sio_tmp_ && sio_count_ != 0) {                                                    \
        assert (false && "SIO_REALLOC failed");                                             \
        abort ();                                                                           \
    }                                                                                       \
    *sio_ptr_ = sio_tmp_;                                                                   \
} while (0)

#define SIO_FREE(ptr) do { \
    typeof(ptr) *sio_ptr_ = &(ptr); \
    free(*sio_ptr_);                \
    *sio_ptr_ = nullptr;            \
} while (0)

/* SIO_PATH */
sio_path* sio_path_new (void) {
    sio_path* p = nullptr;
    SIO_MALLOC(p, 1);
    p->path_str.length = 0;
    p->path_str.chars = nullptr;
    return p;
}

sio_path* sio_path_from_c_str (const char* s) {
    sio_path* path = sio_path_new ();
    assert (path);
    assert (path->path_str.length == 0);
    assert (path->path_str.chars == nullptr);
    sio_string_copy_from_chars (&path->path_str, s);
    return path;
}

void sio_path_free (sio_path* p) {
    assert(p);

    SIO_FREE(p->path_str.chars);
    p->path_str.length = 0;
    SIO_FREE(p);
}

/* SIO_STRING */
sio_string* sio_string_new (void) {
    sio_string* s = nullptr;
    SIO_MALLOC(s, 1);

    s->length = 0;
    s->chars = nullptr;
    return s;
}

void sio_string_free (sio_string* s) {
    assert (s);
    s->length = 0;
    SIO_FREE(s->chars);
    SIO_FREE(s);
}

void sio_string_take_from_chars (sio_string* s, char *data) {
    assert (s);
    assert (s->length == 0);
    assert (s->chars == nullptr);
    assert (data);

    s->length = strlen (data);
    s->chars = data;
}

void sio_string_copy_from_chars_with_length (sio_string* s, char const *data, size_t length) {
    assert (s);
    assert (data);
    assert (s->length == 0);
    assert (s->chars == nullptr);

    SIO_MALLOC(s->chars, length + 1); /* + 1 for null terminator */
    memmove (s->chars, data, length);
    s->chars[length] = '\0';
    s->length = length; /* length does NOT include null terminator */

    assert (s->chars != nullptr);
    assert (s->chars[s->length] == '\0');
}

void sio_string_copy_from_chars (sio_string* s, char const *data) {
    assert (s);
    assert (s->length == 0);
    assert (s->chars == nullptr);

    if (! data) return;

    s->length = strlen (data); /* does NOT include null terminator */
    SIO_MALLOC(s->chars, s->length + 1); /* + 1 for null terminator */
    memmove (s->chars, data, s->length + 1);

    assert (s->chars != nullptr);
    assert (s->chars[s->length] == '\0');
}

/* SIO_FILE */
sio_file* sio_file_new (void) {
    sio_file* f = nullptr;
    SIO_MALLOC(f, 1);
    f->ret_p = 0;
    f->file = nullptr;
    return f;
}

void sio_file_free (sio_file* p) {
    if (! p) return;

    if (p->file != nullptr) {
        fclose (p->file);
        p->file = nullptr;
    }

    SIO_FREE(p);
}

/* SIO_CONTEXT */
sio_context* sio_context_init (void) {
    sio_context *ctx = nullptr;
    SIO_MALLOC(ctx, 1);

    assert (ctx != nullptr);
    return ctx;
}

void sio_context_destroy (sio_context* ctx) {
    assert (ctx);
    SIO_FREE(ctx);
}

#ifdef SIO_USE_URING
sio_string* sio_read_file (sio_context *ctx, sio_file *file) {
    assert (ctx);

    if (! file || ! file->file) return nullptr;
    return nullptr;
}
#else // SIO_USE_URING
sio_string* sio_read_file (sio_context *ctx, sio_file *file) {
    assert (ctx);

    if (! file || ! file->file) return nullptr;

    const int fd = fileno (file->file);
    if (fd == -1) {
        perror ("fileno");
        return nullptr;
    }

    struct stat st;
    if (fstat (fd, &st) == -1) {
        perror ("fstat");
        fprintf (stderr, "Failed fstat for fd: %d\n", fd);
        return nullptr;
    }
    const size_t len = st.st_size;

    /* empty file */
    if (len == 0) {
        sio_string *s = sio_string_new ();
        return s;
    }

    char *buf = mmap (0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
        perror ("mmap");
        fprintf (stderr, "Failed mmap for fd: %d\n", fd);
        return nullptr;
    }

    sio_string *file_contents = sio_string_new ();
    sio_string_copy_from_chars_with_length (file_contents, buf, len);

    assert (file_contents->length == len);
    assert (file_contents->chars[file_contents->length] == '\0');
    assert (file_contents->chars != buf);

    munmap (buf, len);
    return file_contents;
}
#endif // SIO_USE_URING

sio_file* sio_open (sio_context* ctx, sio_path* path, const char* mode) {
    assert (ctx);
    assert (path);
    assert (path->path_str.length != 0);
    assert (path->path_str.chars != nullptr);
    assert (path->path_str.chars[path->path_str.length] == '\0');

    sio_file* file = sio_file_new ();
    FILE* f = fopen (path->path_str.chars, mode);
    if (! f) {
        sio_file_free (file);
        fprintf (stderr, "fopen failed for path: %s\n", path->path_str.chars);
        return nullptr;
    }

    file->file = f;
    return file;
}

void sio_close (sio_context* ctx, sio_file* file) {
    assert (ctx);
    sio_file_free (file);
}
