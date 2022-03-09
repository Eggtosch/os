#include <vfs/vfs.h>
#include <common.h>
#include <io/stdio.h>
#include <memory/pmm.h>
#include <debug.h>
#include <string.h>


struct vfs_entry {
	struct io_device *stream;
	char name[248];
};

static_assert(sizeof(struct vfs_entry) == 256);

#define ENTRIES_PER_PAGE (PAGE_SIZE / sizeof(struct vfs_entry))


static struct vfs_entry *_vfs;
static u64 _vfs_size;
static u64 _vfs_cap;


void vfs_init(void) {
	_vfs = pmm_alloc(1);
	_vfs_size = 0;
	_vfs_cap  = ENTRIES_PER_PAGE;
	debug(DEBUG_INFO, "Initialized virtual file system (vfs)");
}

int vfs_mount(const char *pathname, struct io_device *stream) {
	if (pathname == NULL || stream == NULL) {
		return -1;
	}

	if (_vfs_size >= _vfs_cap) {
		struct vfs_entry *new = pmm_alloc(_vfs_cap * 2 / ENTRIES_PER_PAGE);
		for (u64 i = 0; i < _vfs_cap; i++) {
			new[i] = _vfs[i];
		}
		_vfs_cap *= 2;
		_vfs = new;
	}

	struct vfs_entry *entry = &_vfs[_vfs_size];
	entry->stream = stream;
	for (u64 i = 0;; i++) {
		entry->name[i] = pathname[i];
		if (pathname[i] == '\0') {
			break;
		}
	}

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

struct io_device *vfs_get(int fd) {
	if (fd < 0 || (u64) fd >= _vfs_size) {
		return NULL;
	}
	return _vfs[fd].stream;
}
