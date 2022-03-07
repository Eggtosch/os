#include <syscall.h>

int sys_open(const char *name, u64 flags);

u64 sys_read(int fd, void *buffer, u64 count) {
	u64 bytes;
	asm volatile("int $0x80" : "=a"(bytes) : "a"(0x1), "b"(fd), "c"(buffer), "d"(count) : "memory");
	return bytes;
}

u64 sys_write(int fd, void *buffer, u64 count);
int sys_fb_buffer_init(struct fb_buffer *buffer, u16 x, u16 y, u16 width, u16 height);
int sys_fb_buffer_deinit(struct fb_buffer *fb_buffer);
int sys_fb_buffer_present(struct fb_buffer *fb_buffer);
int sys_mmap(void *addr, u64 length, int fd, u64 flags);
int sys_munmap(void *addr, u64 length);
