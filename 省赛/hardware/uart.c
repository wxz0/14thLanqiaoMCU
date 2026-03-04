#include "uart.h"


void uart_send_byte(uint8_t byte)
{
  SBUF = byte;
	while(TI == 0);
	TI = 0;
}
