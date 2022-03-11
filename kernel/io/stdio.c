#include <io/stdio.h>


struct io_device *stdio = NULL;

void stdio_init(struct io_device *_stdio) {
	stdio = _stdio;
}
