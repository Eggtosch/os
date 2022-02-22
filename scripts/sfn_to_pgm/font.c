#include <stdint.h>
#include <stdio.h>
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_NOIMPLEMENTATION
#include "ssfn.h"

extern uint8_t _binary_unifont_sfn_start;

#define W (8 * ('~' - ' ' + 1))
#define H (16)
uint8_t buf[W * H * sizeof(uint32_t)];
uint8_t buf2[W * H];

int main(void) {
	ssfn_src		= (ssfn_font_t *)&_binary_unifont_sfn_start;
	ssfn_dst.ptr	= buf;
	ssfn_dst.w		= W;
	ssfn_dst.h		= H;
	ssfn_dst.p		= W * 4;
	ssfn_dst.x		= 0;
	ssfn_dst.y		= 0;
	ssfn_dst.fg     = 0xffffffff;
	for (uint32_t c = ' '; c <= '~'; c++) {
		ssfn_putc(c);
	}

	uint32_t *old = (uint32_t*) buf;
	for (int i = 0; i < W * H; i++) {
		buf2[i] = old[i] == 0 ? 0 : 0xff;
	}

	printf("P5\n%d %d\n%d\n", W, H, 255);
	fwrite(buf2, 1, sizeof(buf2), stdout);
}
