#include <memory/pmm.h>
#include <panic.h>
#include <process/pipe.h>
#include <string.h>

static u64 pipe_read(struct io_device *dev, u8 *buf, u64 bufsize, __unused u64 offset) {
	struct pipe *pipe = (struct pipe *) dev;
	u64 bytes         = (u64) pipe->size <= bufsize ? (u64) pipe->size : bufsize;

	for (u64 i = 0; i < bytes; i++) {
		buf[i] = (pipe->position + i) % PAGE_SIZE;
	}

	pipe->position = (pipe->position + bytes) % PAGE_SIZE;
	pipe->size -= bytes;
	return bytes;
}

static u64 pipe_write(struct io_device *dev, u8 *buf, u64 bufsize, __unused u64 offset) {
	struct pipe *pipe = (struct pipe *) dev;
	u64 bytes_free    = PAGE_SIZE - pipe->size;
	u64 bytes         = bytes_free <= bufsize ? bytes_free : bufsize;

	for (u64 i = 0; i < bytes; i++) {
		int bufindex           = (pipe->position + i) % PAGE_SIZE;
		pipe->buffer[bufindex] = buf[i];
	}

	pipe->size += bytes;
	return bytes;
}

static void pipe_close(struct io_device *dev) {
	struct pipe *pipe = (struct pipe *) dev;
	pmm_free(pipe->buffer, 1);
	pipe->position = 0;
	pipe->size     = 0;
	pipe->buffer   = NULL;
}

struct pipe pipe_new(void) {
	struct pipe pipe;
	pipe.dev.read     = pipe_read;
	pipe.dev.write    = pipe_write;
	pipe.dev.close    = pipe_close;
	pipe.dev.userdata = NULL;
	pipe.position     = 0;
	pipe.size         = 0;
	pipe.buffer       = pmm_alloc(1);
	return pipe;
}
