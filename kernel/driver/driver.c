#include <driver/driver.h>
#include <vfs/vfs.h>

void driver_register(__unused const char *name, struct driver_file *files) {
	for (; files->filepath != NULL; files++) {
		vfs_mount(files->filepath, &files->stream);
	}
}

