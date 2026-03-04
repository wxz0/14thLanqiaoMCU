#include "seg.h"
#include "74hc138.h"
uint8_t arr[] = {0xc0,0xf9,0xa4,0xb0,//0 1 2 3
                 0x99,0x92,0x82,0xf8,//4 5 6 7
	               0x80,0x90,0xff,0xbf,//8 9 null -
	               0xc6,0x86,0x8e,0x8c,//c e f p
	               0x88//R 
                    };

void seg_proc(uint8_t n,uint8_t num,uint8_t point)
{
	P0 = 0xff;
	hc138_select(7);
	hc138_select(0);
	
  P0 = 0x01 << n;
	hc138_select(6);
	hc138_select(0);
	
	P0 = arr[num];
	if(point)P0 = P0 & ~0x80;
  hc138_select(7);
	hc138_select(0);
}