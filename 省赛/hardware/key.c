#include "key.h"

sbit ROW3 = P3^2;
sbit ROW4 = P3^3;
sbit COL1 = P4^4;
sbit COL2 = P4^2;

uint8_t key_getstate(void)
{
 uint8_t state = 0;

 ROW3 = ROW4 = COL1 = COL2 = 1;

 COL1 = 0;
 if(ROW4 == 0)state = 0x01 << 0;
 if(ROW3 == 0)state = 0x01 << 1;
 COL1 = 1;

 COL2 = 0;
 if(ROW4 == 0)state = 0x01 << 2;
 if(ROW3 == 0)state = 0x01 << 3;
 COL2 = 1;

 return state;	
}