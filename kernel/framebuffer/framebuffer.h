#pragma once

#include <common.h>
#include <boot/boot_info.h>

void framebuffer_init(struct boot_info *boot_info);
i64 framebuffer_read(u16 x, u16 y, u16 width, u16 height, u32 *buffer);
i64 framebuffer_write(u16 x, u16 y, u16 width, u16 height, u32 *buffer);
