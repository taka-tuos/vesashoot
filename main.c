#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <graph2.h>
#include <conio.h>
#include <time.h>

#include "xgrp.h"

#define SCR_W 320
#define SCR_H 200
#define SCR_SIZ (SCR_W*SCR_H)

uint8_t *scr;
uint8_t *vga;

void init_pallete(void)
{
	int r,g,b;
	long rgb332_table[256];
	
	for(r = 0; r < 8; r++) {
		for(g = 0; g < 8; g++) {
			for(b = 0; b < 4; b++) {
				int r8, g8, b8;

				r8 = (r << 5) | (r << 2) | (r >> 1);
				g8 = (g << 5) | (g << 2) | (g >> 1);
				b8 = (b << 6) | (b << 4) | (b << 2) | b;
			
				if(r8 > 255) r8 = 255;
				if(g8 > 255) g8 = 255;
				if(b8 > 255) b8 = 255;

				rgb332_table[(r << 5) | (g << 2) | b] = (r8 << 16) | (g8 << 8) | b8;
			}
		}
	}
	
	_remapallpalette(rgb332_table);
}

void restore_pallete(void)
{
	int i;
	long vga_table[16];
	
	int tbl[2][2] = { {0x00,0xaa}, {0x55,0xff} };
	
	for(i = 0; i < 16; i++) {
		int a = (i >> 3) & 1;
		int r = (i >> 2) & 1;
		int g = (i >> 1) & 1;
		int b = (i >> 0) & 1;
		
		int r8 = tbl[a][r];
		int g8 = tbl[a][g];
		int b8 = tbl[a][b];
		
		vga_table[i] = (r8 << 16) | (g8 << 8) | b8;
	}
	
	_remapallpalette(vga_table);
}

void ps10(int x, int y, int c)
{
	if(x < 0 || x >= SCR_W || y < 0 || y >= SCR_H) return;
	if(c != 0xe3) scr[y*SCR_W+x] = c;
}

void ps20(int x, int y, int c)
{
	if(((x&1)+y)&1) ps10(x,y,c);
}

int main(int argc, char *argv[])
{
	//int x,y;
	xgrp_bitmap t;
	int x[50],y[50];
	int vx,vy;
	
	_setvideomode(_MRES256COLOR);
	
	vga = (uint8_t *)0xa0000;
	scr = (uint8_t *)malloc(SCR_SIZ);
	
	init_pallete();
	
	xgrp_set_framebuffer(scr,SCR_W,SCR_H);
	
	/*for(y=0;y<SCR_H;y++)
		for(x=0;x<SCR_W;x++)
			scr[y*SCR_W+x] = ((y*16/SCR_H)<<4)|(x*16/SCR_W);
	memcpy(vga,scr,SCR_SIZ);*/
	
	xgrp_enable_bitmap_dither(0);
	
	xgrp_bitmap_load(&t,"icon.png");
	
	memset(x,0,sizeof(x));
	memset(y,0,sizeof(y));
	
	vx = 16;
	vy = 18;
	
	while(1) {
		int nx,ny;
		int i;
		
		x[0] += vx;
		y[0] += vy;
		
		//x[0] = -1;
		//y[0] = -1;
		
		nx = x[0] >> 4;
		ny = y[0] >> 4;
		
		for(i=49;i>0;i--) {
			x[i] = x[i-1];
			y[i] = y[i-1];
		}
		
		if(nx < 0 || nx >= SCR_W-31) vx = -vx;
		if(ny < 0 || ny >= SCR_H-31) vy = -vy;
		
		for(i=49;i>0;i-=10) xgrp_bitmap_draw_ps(&t,x[i]>>4,y[i]>>4,ps20);
		
		//xgrp_bitmap_draw_ps(&t,x[20]>>4,y[20]>>4,ps10);
		//xgrp_bitmap_draw_ps(&t,x[10]>>4,y[10]>>4,ps10);
		xgrp_bitmap_draw_ps(&t,nx,ny,ps10);
		
		{
			int n=clock();
			memcpy(vga,scr,SCR_SIZ);
			memset(scr,0,SCR_SIZ);
			while(clock()-n < 20);
		}
		
		if(kbhit()) {
			if(getch() == 0x1b) break;
		}
	}
	
	_setvideomode(_DEFAULTMODE);
	
	restore_pallete();
	
	return 0;
}
