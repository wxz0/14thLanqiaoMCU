#include "led.h"
#include "74hc138.h"


void led_proc(uint8_t state[])
{
 static uint8_t state_old;
 uint8_t temp = 0;
 uint8_t i = 0;
 for(i = 0;i<8;i++)
 {
   temp = temp | (state[i] << i);
 }

 if(temp!=state_old)
 {
	 P0 = ~temp;
	 hc138_select(4);
	 hc138_select(0);
   state_old = temp;
 }
}
