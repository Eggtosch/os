#pragma once

#include <common.h>


void idt_init(void);
void idt_create_descriptor(u8 index, u8 type_and_attributes);
