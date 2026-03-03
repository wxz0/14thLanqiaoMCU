#include "74hc138.h"

void hc138_select(uint8_t n)
{
  switch(n)
	{
		case 4:
			P2 = (P2&0x1f) | 0x80;
			break;
		case 5:
			P2 = (P2&0x1f) | 0xa0;
			break;
		case 6:
			P2 = (P2&0x1f) | 0xc0;
			break;
		case 7:
			P2 = (P2&0x1f) | 0xe0;
			break;
		default:
			P2 = (P2&0x1f);
			break;
	}

}

void sys_init(void)
{
	P0 = 0xff;
  hc138_select(4);
	
	P0 = 0xaf;
	hc138_select(5);
	
	P0 = 0xff;
	hc138_select(6);

	P0 = 0xff;
	hc138_select(7);	
}
