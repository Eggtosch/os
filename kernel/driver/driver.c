#include <driver/driver.h>
#include <vfs/vfs.h>

void driver_register(const char *name, struct driver_file *files) {
	(void) name;
	for (; files->filepath != NULL; files++) {
		vfs_mount(files->filepath, &files->stream);
	}
}

