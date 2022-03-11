#pragma once

#include <common.h>


void pic_remap(void);
void pic_set_mask(u8 isr);
void pic_clear_mask(u8 isr);
void pic_signal_eoi(u64 isr);
