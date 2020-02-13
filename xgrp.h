#include <stdint.h>

#ifndef __XGRP_H__
#define __XGRP_H__

typedef struct {
	int w,h;
	uint8_t *p;
} xgrp_bitmap;

void xgrp_set_framebuffer(uint8_t *, int, int);

void xgrp_enable_bitmap_dither(int);

void xgrp_bitmap_load(xgrp_bitmap *, char *);
void xgrp_bitmap_draw(xgrp_bitmap *, int, int);
void xgrp_bitmap_draw_ps(xgrp_bitmap *, int, int, void (*)(int, int, int));

#endif
