/*
 * This header was auto generated from the folder /home/oskar/programs/os/fonts
 * Date: 22-03-2022 01:00:12
 */

#pragma once

#include <common.h>

struct font {
	const char *font_name;
	u16 font_width;
	u16 font_height;
	u16 font_nglyphs;
	u8 *font_data;
};

static u8 font0[256][11] = {
[' '] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['!'] = {0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x10,0x00},
['"'] = {0x00,0x00,0x6c,0x6c,0x24,0x48,0x00,0x00,0x00,0x00,0x00},
['#'] = {0x00,0x00,0x12,0x12,0xff,0x24,0x24,0xff,0x48,0x48,0x00},
['$'] = {0x00,0x10,0x38,0x54,0x50,0x38,0x14,0x54,0x38,0x10,0x00},
['%'] = {0x00,0x00,0x61,0x92,0x94,0x68,0x16,0x29,0x49,0x86,0x00},
['&'] = {0x00,0x00,0x30,0x48,0x40,0x32,0x4c,0x4c,0x32,0x00,0x00},
['\''] = {0x00,0x00,0x18,0x18,0x08,0x10,0x00,0x00,0x00,0x00,0x00},
['('] = {0x00,0x04,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x04},
[')'] = {0x00,0x20,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x20},
['*'] = {0x00,0x00,0x54,0x38,0x38,0x54,0x00,0x00,0x00,0x00,0x00},
['+'] = {0x00,0x00,0x00,0x10,0x10,0x7c,0x10,0x10,0x00,0x00,0x00},
[','] = {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x08,0x10,0x00},
['-'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00},
['.'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},
['/'] = {0x00,0x00,0x04,0x04,0x08,0x08,0x10,0x10,0x20,0x20,0x00},
['0'] = {0x00,0x00,0x3c,0x46,0x4a,0x4a,0x52,0x52,0x62,0x3c,0x00},
['1'] = {0x00,0x00,0x18,0x28,0x08,0x08,0x08,0x08,0x08,0x1c,0x00},
['2'] = {0x00,0x00,0x3c,0x42,0x42,0x04,0x18,0x20,0x42,0x7e,0x00},
['3'] = {0x00,0x00,0x3c,0x42,0x02,0x1c,0x02,0x02,0x42,0x3c,0x00},
['4'] = {0x00,0x00,0x0c,0x14,0x24,0x44,0x44,0x7e,0x04,0x04,0x00},
['5'] = {0x00,0x00,0x7c,0x40,0x40,0x7c,0x02,0x02,0x02,0x7c,0x00},
['6'] = {0x00,0x00,0x3c,0x40,0x40,0x7c,0x42,0x42,0x42,0x3c,0x00},
['7'] = {0x00,0x00,0x7e,0x02,0x02,0x04,0x08,0x08,0x08,0x08,0x00},
['8'] = {0x00,0x00,0x3c,0x42,0x42,0x3c,0x42,0x42,0x42,0x3c,0x00},
['9'] = {0x00,0x00,0x3c,0x42,0x42,0x42,0x3e,0x02,0x42,0x3c,0x00},
[':'] = {0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00,0x00},
[';'] = {0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x08,0x10},
['<'] = {0x00,0x00,0x00,0x06,0x18,0x60,0x60,0x18,0x06,0x00,0x00},
['='] = {0x00,0x00,0x00,0x00,0x7e,0x00,0x7e,0x00,0x00,0x00,0x00},
['>'] = {0x00,0x00,0x00,0x60,0x18,0x06,0x06,0x18,0x60,0x00,0x00},
['?'] = {0x00,0x00,0x38,0x44,0x04,0x08,0x10,0x10,0x00,0x10,0x00},
['@'] = {0x00,0x00,0x3c,0x42,0x9d,0xa5,0xa5,0xba,0x40,0x3c,0x00},
['A'] = {0x00,0x00,0x18,0x24,0x42,0x42,0x7e,0x42,0x42,0x42,0x00},
['B'] = {0x00,0x00,0x7c,0x42,0x42,0x7c,0x42,0x42,0x42,0x7c,0x00},
['C'] = {0x00,0x00,0x3c,0x42,0x42,0x40,0x40,0x42,0x42,0x3c,0x00},
['D'] = {0x00,0x00,0x78,0x44,0x42,0x42,0x42,0x42,0x44,0x78,0x00},
['E'] = {0x00,0x00,0x7e,0x40,0x40,0x7c,0x40,0x40,0x40,0x7e,0x00},
['F'] = {0x00,0x00,0x7e,0x40,0x40,0x7c,0x40,0x40,0x40,0x40,0x00},
['G'] = {0x00,0x00,0x3c,0x46,0x40,0x40,0x4e,0x42,0x42,0x3c,0x00},
['H'] = {0x00,0x00,0x42,0x42,0x42,0x7e,0x42,0x42,0x42,0x42,0x00},
['I'] = {0x00,0x00,0x38,0x10,0x10,0x10,0x10,0x10,0x10,0x38,0x00},
['J'] = {0x00,0x00,0x1e,0x04,0x04,0x04,0x04,0x44,0x44,0x38,0x00},
['K'] = {0x00,0x00,0x42,0x42,0x44,0x48,0x78,0x44,0x42,0x42,0x00},
['L'] = {0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7e,0x00},
['M'] = {0x00,0x00,0x42,0x66,0x7e,0x5a,0x42,0x42,0x42,0x42,0x00},
['N'] = {0x00,0x00,0x42,0x62,0x52,0x52,0x4a,0x4a,0x46,0x42,0x00},
['O'] = {0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00},
['P'] = {0x00,0x00,0x7c,0x42,0x42,0x42,0x7c,0x40,0x40,0x40,0x00},
['Q'] = {0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x44,0x4c,0x36,0x00},
['R'] = {0x00,0x00,0x7c,0x42,0x42,0x42,0x7c,0x44,0x42,0x42,0x00},
['S'] = {0x00,0x00,0x3c,0x42,0x40,0x3c,0x02,0x02,0x42,0x3c,0x00},
['T'] = {0x00,0x00,0x7c,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00},
['U'] = {0x00,0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00},
['V'] = {0x00,0x00,0x42,0x42,0x42,0x42,0x24,0x24,0x18,0x18,0x00},
['W'] = {0x00,0x00,0x42,0x42,0x42,0x42,0x5a,0x7e,0x66,0x42,0x00},
['X'] = {0x00,0x00,0x42,0x42,0x24,0x18,0x18,0x24,0x42,0x42,0x00},
['Y'] = {0x00,0x00,0x42,0x42,0x24,0x18,0x18,0x18,0x18,0x18,0x00},
['Z'] = {0x00,0x00,0x7e,0x02,0x04,0x08,0x10,0x20,0x40,0x7e,0x00},
['['] = {0x1c,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1c},
['\\'] = {0x00,0x00,0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,0x00},
[']'] = {0x38,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x38},
['^'] = {0x00,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['_'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e},
['`'] = {0x00,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['a'] = {0x00,0x00,0x00,0x00,0x3c,0x02,0x3e,0x42,0x42,0x3e,0x00},
['b'] = {0x00,0x00,0x00,0x40,0x40,0x7c,0x42,0x42,0x42,0x7c,0x00},
['c'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x40,0x40,0x42,0x3c,0x00},
['d'] = {0x00,0x00,0x02,0x02,0x3e,0x42,0x42,0x42,0x42,0x3e,0x00},
['e'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x7e,0x40,0x3e,0x00},
['f'] = {0x00,0x00,0x1c,0x22,0x20,0x78,0x20,0x20,0x20,0x20,0x00},
['g'] = {0x00,0x00,0x00,0x3e,0x42,0x42,0x42,0x3e,0x02,0x02,0x3c},
['h'] = {0x00,0x00,0x40,0x40,0x40,0x5c,0x62,0x42,0x42,0x42,0x00},
['i'] = {0x00,0x00,0x10,0x10,0x00,0x30,0x10,0x10,0x10,0x38,0x00},
['j'] = {0x00,0x04,0x04,0x00,0x04,0x04,0x04,0x04,0x04,0x44,0x38},
['k'] = {0x00,0x00,0x40,0x40,0x42,0x44,0x78,0x44,0x42,0x42,0x00},
['l'] = {0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x0c,0x00},
['m'] = {0x00,0x00,0x00,0x00,0x64,0x5a,0x5a,0x42,0x42,0x42,0x00},
['n'] = {0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x42,0x42,0x42,0x00},
['o'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x3c,0x00},
['p'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x7c,0x40,0x40},
['q'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x3e,0x02,0x02},
['r'] = {0x00,0x00,0x00,0x00,0x5c,0x60,0x40,0x40,0x40,0x40,0x00},
['s'] = {0x00,0x00,0x00,0x3c,0x42,0x40,0x3c,0x02,0x42,0x3c,0x00},
['t'] = {0x00,0x00,0x00,0x20,0x20,0x7c,0x20,0x20,0x20,0x22,0x1c},
['u'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x42,0x3e,0x00},
['v'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x24,0x24,0x18,0x18,0x00},
['w'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x5a,0x5a,0x24,0x00},
['x'] = {0x00,0x00,0x00,0x00,0x42,0x24,0x18,0x18,0x24,0x42,0x00},
['y'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x3e,0x02,0x02,0x3c},
['z'] = {0x00,0x00,0x00,0x00,0x7e,0x04,0x08,0x10,0x20,0x7e,0x00},
['{'] = {0x0c,0x10,0x10,0x10,0x10,0x60,0x10,0x10,0x10,0x10,0x0c},
['|'] = {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
['}'] = {0x30,0x08,0x08,0x08,0x08,0x06,0x08,0x08,0x08,0x08,0x30},
['~'] = {0x00,0x00,0x00,0x00,0x20,0x5a,0x04,0x00,0x00,0x00,0x00}
};

static u8 font1[256][16] = {
[' '] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['!'] = {0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x08,0x08,0x00,0x00},
['"'] = {0x00,0x00,0x22,0x22,0x22,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['#'] = {0x00,0x00,0x00,0x00,0x12,0x12,0x12,0x7e,0x24,0x24,0x7e,0x48,0x48,0x48,0x00,0x00},
['$'] = {0x00,0x00,0x00,0x00,0x08,0x3e,0x49,0x48,0x38,0x0e,0x09,0x49,0x3e,0x08,0x00,0x00},
['%'] = {0x00,0x00,0x00,0x00,0x31,0x4a,0x4a,0x34,0x08,0x08,0x16,0x29,0x29,0x46,0x00,0x00},
['&'] = {0x00,0x00,0x00,0x00,0x1c,0x22,0x22,0x14,0x18,0x29,0x45,0x42,0x46,0x39,0x00,0x00},
['\''] = {0x00,0x00,0x08,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['('] = {0x00,0x00,0x00,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x00},
[')'] = {0x00,0x00,0x00,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x00},
['*'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x49,0x2a,0x1c,0x2a,0x49,0x08,0x00,0x00,0x00},
['+'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x7f,0x08,0x08,0x08,0x00,0x00,0x00},
[','] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x08,0x08,0x10},
['-'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00},
['.'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},
['/'] = {0x00,0x00,0x00,0x00,0x02,0x02,0x04,0x08,0x08,0x10,0x10,0x20,0x40,0x40,0x00,0x00},
['0'] = {0x00,0x00,0x00,0x00,0x18,0x24,0x42,0x46,0x4a,0x52,0x62,0x42,0x24,0x18,0x00,0x00},
['1'] = {0x00,0x00,0x00,0x00,0x08,0x18,0x28,0x08,0x08,0x08,0x08,0x08,0x08,0x3e,0x00,0x00},
['2'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x02,0x0c,0x10,0x20,0x40,0x40,0x7e,0x00,0x00},
['3'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x02,0x1c,0x02,0x02,0x42,0x42,0x3c,0x00,0x00},
['4'] = {0x00,0x00,0x00,0x00,0x04,0x0c,0x14,0x24,0x44,0x44,0x7e,0x04,0x04,0x04,0x00,0x00},
['5'] = {0x00,0x00,0x00,0x00,0x7e,0x40,0x40,0x40,0x7c,0x02,0x02,0x02,0x42,0x3c,0x00,0x00},
['6'] = {0x00,0x00,0x00,0x00,0x1c,0x20,0x40,0x40,0x7c,0x42,0x42,0x42,0x42,0x3c,0x00,0x00},
['7'] = {0x00,0x00,0x00,0x00,0x7e,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x08,0x00,0x00},
['8'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x3c,0x42,0x42,0x42,0x42,0x3c,0x00,0x00},
['9'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x3e,0x02,0x02,0x02,0x04,0x38,0x00,0x00},
[':'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00},
[';'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x08,0x08,0x10,0x00},
['<'] = {0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x10,0x08,0x04,0x02,0x00,0x00},
['='] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00},
['>'] = {0x00,0x00,0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x40,0x00,0x00},
['?'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x02,0x04,0x08,0x08,0x00,0x08,0x08,0x00,0x00},
['@'] = {0x00,0x00,0x00,0x00,0x1c,0x22,0x4a,0x56,0x52,0x52,0x52,0x4e,0x20,0x1e,0x00,0x00},
['A'] = {0x00,0x00,0x00,0x00,0x18,0x24,0x24,0x42,0x42,0x7e,0x42,0x42,0x42,0x42,0x00,0x00},
['B'] = {0x00,0x00,0x00,0x00,0x7c,0x42,0x42,0x42,0x7c,0x42,0x42,0x42,0x42,0x7c,0x00,0x00},
['C'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x40,0x40,0x40,0x40,0x42,0x42,0x3c,0x00,0x00},
['D'] = {0x00,0x00,0x00,0x00,0x78,0x44,0x42,0x42,0x42,0x42,0x42,0x42,0x44,0x78,0x00,0x00},
['E'] = {0x00,0x00,0x00,0x00,0x7e,0x40,0x40,0x40,0x7c,0x40,0x40,0x40,0x40,0x7e,0x00,0x00},
['F'] = {0x00,0x00,0x00,0x00,0x7e,0x40,0x40,0x40,0x7c,0x40,0x40,0x40,0x40,0x40,0x00,0x00},
['G'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x40,0x40,0x4e,0x42,0x42,0x46,0x3a,0x00,0x00},
['H'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x7e,0x42,0x42,0x42,0x42,0x42,0x00,0x00},
['I'] = {0x00,0x00,0x00,0x00,0x3e,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3e,0x00,0x00},
['J'] = {0x00,0x00,0x00,0x00,0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x44,0x38,0x00,0x00},
['K'] = {0x00,0x00,0x00,0x00,0x42,0x44,0x48,0x50,0x60,0x60,0x50,0x48,0x44,0x42,0x00,0x00},
['L'] = {0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7e,0x00,0x00},
['M'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x66,0x66,0x5a,0x5a,0x42,0x42,0x42,0x42,0x00,0x00},
['N'] = {0x00,0x00,0x00,0x00,0x42,0x62,0x62,0x52,0x52,0x4a,0x4a,0x46,0x46,0x42,0x00,0x00},
['O'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00},
['P'] = {0x00,0x00,0x00,0x00,0x7c,0x42,0x42,0x42,0x7c,0x40,0x40,0x40,0x40,0x40,0x00,0x00},
['Q'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x42,0x5a,0x66,0x3c,0x03,0x00},
['R'] = {0x00,0x00,0x00,0x00,0x7c,0x42,0x42,0x42,0x7c,0x48,0x44,0x44,0x42,0x42,0x00,0x00},
['S'] = {0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x40,0x30,0x0c,0x02,0x42,0x42,0x3c,0x00,0x00},
['T'] = {0x00,0x00,0x00,0x00,0x7f,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
['U'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00},
['V'] = {0x00,0x00,0x00,0x00,0x41,0x41,0x41,0x22,0x22,0x22,0x14,0x14,0x08,0x08,0x00,0x00},
['W'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x5a,0x5a,0x66,0x66,0x42,0x42,0x00,0x00},
['X'] = {0x00,0x00,0x00,0x00,0x42,0x42,0x24,0x24,0x18,0x18,0x24,0x24,0x42,0x42,0x00,0x00},
['Y'] = {0x00,0x00,0x00,0x00,0x41,0x41,0x22,0x22,0x14,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
['Z'] = {0x00,0x00,0x00,0x00,0x7e,0x02,0x02,0x04,0x08,0x10,0x20,0x40,0x40,0x7e,0x00,0x00},
['['] = {0x00,0x00,0x00,0x0e,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x0e,0x00},
['\\'] = {0x00,0x00,0x00,0x00,0x40,0x40,0x20,0x10,0x10,0x08,0x08,0x04,0x02,0x02,0x00,0x00},
[']'] = {0x00,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x70,0x00},
['^'] = {0x00,0x00,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['_'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x00},
['`'] = {0x00,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
['a'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x42,0x02,0x3e,0x42,0x42,0x46,0x3a,0x00,0x00},
['b'] = {0x00,0x00,0x00,0x40,0x40,0x40,0x5c,0x62,0x42,0x42,0x42,0x42,0x62,0x5c,0x00,0x00},
['c'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x42,0x40,0x40,0x40,0x40,0x42,0x3c,0x00,0x00},
['d'] = {0x00,0x00,0x00,0x02,0x02,0x02,0x3a,0x46,0x42,0x42,0x42,0x42,0x46,0x3a,0x00,0x00},
['e'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x7e,0x40,0x40,0x42,0x3c,0x00,0x00},
['f'] = {0x00,0x00,0x00,0x0c,0x10,0x10,0x10,0x7c,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00},
['g'] = {0x00,0x00,0x00,0x00,0x00,0x02,0x3a,0x44,0x44,0x44,0x38,0x20,0x3c,0x42,0x42,0x3c},
['h'] = {0x00,0x00,0x00,0x40,0x40,0x40,0x5c,0x62,0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00},
['i'] = {0x00,0x00,0x00,0x08,0x08,0x00,0x18,0x08,0x08,0x08,0x08,0x08,0x08,0x3e,0x00,0x00},
['j'] = {0x00,0x00,0x00,0x04,0x04,0x00,0x0c,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x48,0x30},
['k'] = {0x00,0x00,0x00,0x40,0x40,0x40,0x44,0x48,0x50,0x60,0x50,0x48,0x44,0x42,0x00,0x00},
['l'] = {0x00,0x00,0x00,0x18,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3e,0x00,0x00},
['m'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x49,0x49,0x49,0x49,0x49,0x49,0x49,0x00,0x00},
['n'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00},
['o'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00},
['p'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x42,0x42,0x42,0x62,0x5c,0x40,0x40},
['q'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3a,0x46,0x42,0x42,0x42,0x42,0x46,0x3a,0x02,0x02},
['r'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x62,0x42,0x40,0x40,0x40,0x40,0x40,0x00,0x00},
['s'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x42,0x40,0x30,0x0c,0x02,0x42,0x3c,0x00,0x00},
['t'] = {0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x7c,0x10,0x10,0x10,0x10,0x10,0x0c,0x00,0x00},
['u'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x46,0x3a,0x00,0x00},
['v'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x24,0x24,0x24,0x18,0x18,0x00,0x00},
['w'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x49,0x49,0x49,0x49,0x49,0x49,0x36,0x00,0x00},
['x'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x42,0x24,0x18,0x18,0x24,0x42,0x42,0x00,0x00},
['y'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x42,0x26,0x1a,0x02,0x02,0x3c},
['z'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x02,0x04,0x08,0x10,0x20,0x40,0x7e,0x00,0x00},
['{'] = {0x00,0x00,0x00,0x0c,0x10,0x10,0x08,0x08,0x10,0x20,0x10,0x08,0x08,0x10,0x10,0x0c},
['|'] = {0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},
['}'] = {0x00,0x00,0x00,0x30,0x08,0x08,0x10,0x10,0x08,0x04,0x08,0x10,0x10,0x08,0x08,0x30},
['~'] = {0x00,0x00,0x00,0x31,0x49,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

static struct font fonts[] = {
	{"8x11", 8, 11, 256, (u8*)font0},
	{"8x16", 8, 16, 256, (u8*)font1},
	{NULL, 0, 0, 0, NULL}
};
