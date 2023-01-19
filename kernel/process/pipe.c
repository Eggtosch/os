#include <process/pipe.h>
#include <string.h>
#include <memory/pmm.h>
#include <panic.h>

#define ENTRIES_PER_PAGE (PAGE_SIZE / sizeof(struct pipe))

struct pipe {
	int position;
	int size;
	u8 *buffer;
};

static_assert(sizeof(struct pipe) == 16, "");

static struct pipe *_pipes = NULL;
static int _pipes_cap = 0;
static int _pipes_size = 0;

static int first_free_pipe(void) {
	if (_pipes_size >= _pipes_cap) {
		if (_pipes_cap == 0) {
			_pipes_cap = ENTRIES_PER_PAGE;
			_pipes = (struct pipe*) pmm_alloc(1);
		} else {
			_pipes_cap *= 2;
			struct pipe *newpipes = (struct pipe*) pmm_alloc(_pipes_cap * ENTRIES_PER_PAGE);
			memcpy(newpipes, _pipes, _pipes_size * sizeof(struct pipe));
			pmm_free((u64) _pipes, _pipes_cap / ENTRIES_PER_PAGE);
			_pipes = newpipes;
		}
	}

	for (int i = 0; i < _pipes_cap; i++) {
		if (_pipes[i].buffer == NULL) {
			return i;
		}
	}

	panic("No pipes left, this should not be possible\n");
}

static u64 pipe_read(struct io_device *dev, u8 *buf, u64 bufsize, __unused u64 offset) {
	int index = (int)(u64) dev->userdata;
	struct pipe *pipe = &_pipes[index];
	u64 bytes = (u64) pipe->size <= bufsize ? (u64) pipe->size : bufsize;

	for (u64 i = 0; i < bytes; i++) {
		buf[i] = (pipe->position + i) % PAGE_SIZE;
	}

	pipe->position = (pipe->position + bytes) % PAGE_SIZE;
	pipe->size -= bytes;
	return bytes;
}

static u64 pipe_write(struct io_device *dev, u8 *buf, u64 bufsize, __unused u64 offset) {
	int index = (int)(u64) dev->userdata;
	struct pipe *pipe = &_pipes[index];
	u64 bytes_free = PAGE_SIZE - pipe->size;
	u64 bytes = bytes_free <= bufsize ? bytes_free : bufsize;

	for (u64 i = 0; i < bytes; i++) {
		int bufindex = (pipe->position + i) % PAGE_SIZE;
		pipe->buffer[bufindex] = buf[i];
	}

	pipe->size += bytes;
	return bytes;
}

static void pipe_close(struct io_device *dev) {
	int index = (int)(u64) dev->userdata;
	pmm_free((u64) _pipes[index].buffer, 1);
	_pipes[index].position = 0;
	_pipes[index].size = 0;
	_pipes[index].buffer = NULL;
}

struct io_device pipe_new(void) {
	int index = first_free_pipe();
	_pipes[index].position = 0;
	_pipes[index].size = 0;
	_pipes[index].buffer = (u8*) pmm_alloc(1);

	struct io_device dev;
	dev.read = pipe_read;
	dev.write = pipe_write;
	dev.close = pipe_close;
	dev.userdata = (void*)(u64) index;
	return dev;
}

