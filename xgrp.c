#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xgrp.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stbimg.h"

uint8_t *__xgrp_fb = NULL;
int __xgrp_fb_w,__xgrp_fb_h;
int __xgrp_enable_dither = 1;

const int bayerpattern8[64] = {
	 1, 33,  9, 41,  3, 35, 11, 43,
	49, 17, 57, 25, 51, 19, 59, 27,
	13, 45,  5, 37, 15, 47,  7, 39,
	61, 29, 53, 21, 63, 31, 55, 23,
	 4, 36, 12, 44,  2, 34, 10, 42,
	52, 20, 60, 28, 50, 18, 58, 26,
	16, 48,  8, 40, 14, 46,  6, 38,
	64, 32, 56, 24, 62, 30, 54, 22,
};

#define CUTOFF(__n) ((__n) > 255 ? 255 : ((__n) < 0 ? 0 : (__n)))

uint8_t xgrp_rgb2pal(int r, int g, int b, int x, int y)
{
	int bias = 0;
	int r1,g1,b1;
	
	if(__xgrp_enable_dither) bias = bayerpattern8[(y%8) * 8 + (x%8)];
	
	r1 = CUTOFF(r + bias / 2) >> 5;
	g1 = CUTOFF(g + bias / 2) >> 5;
	b1 = CUTOFF(b + bias) >> 6;
	
	return (r1 << 5) | (g1 << 2) | b1;
}

void xgrp_set_framebuffer(uint8_t *fb, int w, int h)
{
	__xgrp_fb = fb;
	__xgrp_fb_w = w;
	__xgrp_fb_h = h;
}

void xgrp_enable_bitmap_dither(int en)
{
	__xgrp_enable_dither = en;
}

void xgrp_bitmap_load(xgrp_bitmap *p, char *f)
{
	int w,h,dmy;
	int x,y;
	stbi_uc *org;
	
	if(!p) return;
	
	org = stbi_load(f, &w, &h, &dmy, 4);
	
	p->w = w;
	p->h = h;
	p->p = (unsigned char *)malloc(w * h);
	
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			int n = (y*w+x)*4;
			p->p[y*w+x] = xgrp_rgb2pal(org[n+2],org[n+1],org[n+0],x,y);
		}
	}
	
	stbi_image_free(org);
}

void xgrp_bitmap_draw(xgrp_bitmap *p, int x, int y)
{
	int i,dw,ofs=0,ofy=0;
	
	if(!__xgrp_fb) return;
	
	dw = p->w;
	
	if(x+p->w >= __xgrp_fb_w) dw = __xgrp_fb_w - x - 1;
	if(x < 0) {
		dw += x;
		ofs = -x;
	}
	
	if(y < 0) ofy = -y;
	
	for(i = ofy; i < p->h; i++) {
		if(y+i >= __xgrp_fb_h) break;
		memcpy(__xgrp_fb+(y+i)*__xgrp_fb_w+(x+ofs),p->p+i*p->w+ofs,dw);
	}
}

void xgrp_bitmap_draw_ps(xgrp_bitmap *p, int x, int y, void (*ps)(int x, int y, int c))
{
	int i,j;
	
	for(i = 0; i < p->h; i++) {
		for(j = 0; j < p->w; j++) {
			ps(x+j,y+i,p->p[i*p->w+j]);
		}
	}
	
	if(!__xgrp_fb) return;
}
