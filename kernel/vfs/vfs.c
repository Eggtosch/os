#include <vfs/vfs.h>

#include <memory/pmm.h>

#include <io/stdio.h>
#include <string.h>
#include <common.h>


struct vfs_entry {
	struct io_device *stream;
	char name[248];
};

static_assert(sizeof(struct vfs_entry) == 256, "");

#define ENTRIES_PER_PAGE (PAGE_SIZE / sizeof(struct vfs_entry))


static struct vfs_entry *_vfs;
static u64 _vfs_size;
static u64 _vfs_cap;


void vfs_init(void) {
	_vfs = (struct vfs_entry*) pmm_alloc(1);
	_vfs_size = 0;
	_vfs_cap  = ENTRIES_PER_PAGE;
}

int vfs_mount(const char *pathname, struct io_device *stream) {
	if (pathname == NULL || stream == NULL) {
		return -1;
	}

	if (_vfs_size >= _vfs_cap) {
		struct vfs_entry *new = (struct vfs_entry*) pmm_alloc(_vfs_cap * 2 / ENTRIES_PER_PAGE);
		for (u64 i = 0; i < _vfs_cap; i++) {
			new[i] = _vfs[i];
		}
		_vfs_cap *= 2;
		_vfs = new;
	}

	struct vfs_entry *entry = &_vfs[_vfs_size];
	entry->stream = stream;
	memcpy(entry->name, pathname, strlen(pathname) + 1);

	_vfs_size++;
	return _vfs_size - 1;
}

void vfs_unmount(int fd) {
	if (fd < 0 || (u64) fd >= _vfs_size) {
		return;
	}

	_vfs_size--;
	_vfs[fd] = _vfs[_vfs_size];
}

int vfs_open(const char *pathname) {
	for (u64 i = 0; i < _vfs_size; i++) {
		if (strcmp(pathname, _vfs[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

u64 vfs_read(const char *pathname, u8 *buf, u64 buflen) {
	struct io_device *dev = vfs_get(vfs_open(pathname));
	if (dev == NULL) {
		return 0;
	}

	return dev->read(dev, buf, buflen);
}

u64 vfs_write(const char *pathname, u8 *buf, u64 buflen) {
	struct io_device *dev = vfs_get(vfs_open(pathname));
	if (dev == NULL) {
		return 0;
	}

	return dev->write(dev, buf, buflen);
}

struct io_device *vfs_get(int fd) {
	if (fd < 0 || (u64) fd >= _vfs_size) {
		return NULL;
	}
	return _vfs[fd].stream;
}

void vfs_print_entries(void) {
	printf("vfs entries:\n");
	for (u64 i = 0; i < _vfs_size; i++) {
		printf("    %s\n", _vfs[i].name);
	}
}

