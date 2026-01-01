#include <stdio.h>
#include <stdlib.h>
#include <sio/sio.h>
#include <assert.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage %s: <input file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct sio_context *sio = sio_context_init();
	struct sio_path *path = sio_path_from_c_str(argv[1]);
	struct sio_file *file = sio_open(sio, path, "r");
	struct sio_string *file_contents = sio_read_file(sio, file);

	assert(file_contents);
	assert(file_contents->chars != nullptr);
	assert(file_contents->length > 0);
	assert(file_contents->chars[file_contents->length] == '\0');
	printf("File contents: '%s'\n", file_contents->chars);
	sio_string_free(file_contents);

	sio_close(sio, file);
	sio_path_free(path);

	sio_context_destroy(sio);
	return EXIT_SUCCESS;
}
