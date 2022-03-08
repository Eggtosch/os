#include <syscall.h>


#define SYSCALL "int $0x80"


int sys_open(const char *name, u64 flags);

u64 sys_read(int fd, void *buffer, u64 count) {
	u64 bytes;
	asm volatile(SYSCALL : "=a"(bytes) : "a"(0x1), "b"(fd), "c"(buffer), "d"(count) : "memory");
	return bytes;
}

u64 sys_write(int fd, void *buffer, u64 count);

int sys_fb_init(struct fb_buffer *fb_buffer) {
	u64 success;
	asm volatile(SYSCALL : "=a"(success) : "a"(0x3), "b"(fb_buffer) : "memory");
	return success;
}

int sys_fb_deinit(struct fb_buffer *fb_buffer) {
	u64 success;
	asm volatile(SYSCALL : "=a"(success) : "a"(0x4), "b"(fb_buffer) : "memory");
	return success;
}

int sys_fb_present(struct fb_buffer *fb_buffer) {
	u64 success;
	asm volatile(SYSCALL : "=a"(success) : "a"(0x5), "b"(fb_buffer) : "memory");
	return success;
}

int sys_mmap(void *addr, u64 length, int fd, u64 flags);
int sys_munmap(void *addr, u64 length);

void sys_exit(void) {
	asm volatile(SYSCALL :: "a"(0x8) : "memory");
}
