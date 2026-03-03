#include <STC15F2K60S2.H>
#include "seg.h"
#include "led.h"
#include "key.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"

pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t seg_index = 0;

idata bit humi_mode = 0;
idata uint16_t humi_3s_count = 0;
idata uint8_t show_mode = 0;
idata uint8_t fd_mode = 0;

idata uint8_t led_state[] ={0,0,0,0,0,0,0,0};

idata uint8_t time[] = {13,3,5};

idata uint16_t light = 0;
idata bit light_state_pre = 0;//0Ϊ����1Ϊ��
idata bit light_state_cur = 0;
idata bit cap_flag = 0;
idata bit get_data_flag = 0;

idata uint16_t temp = 0;

idata uint8_t seg_slow = 0;
idata uint8_t key_slow = 0;
idata uint8_t time_slow = 0;
idata uint8_t light_slow = 0;

void time_task(void)
{
  if(time_slow)return;
	time_slow = 1;
  readtime(time[]);
}

void light_task(void)
{
  if(light_slow)return;
	light_slow = 1;
	if(light_flag)return;
  light = readlight();
	light = light * 100 / 51.0;
	light_state_pre = light_state_cur;
	if(light >= 100)
	{
	  light_state_cur = 0;
	}
	else
	{
	  light_state_cur = 1;
	}
	if(light_state_cur== 1 && light_state_pre == 0)
	{
	  cap_flag = 1;
	}
}


void seg_task(void)
{
  if(seg_slow)return;
	seg_slow = 1;
	if(humi_mode)
	{
	  
	}
	else
	{
		switch(show_mode)
		{
		 case 0:
				seg_show[0] = time[0] / 10;
				seg_show[1] = time[0] % 10;
				seg_show[2] = 11;
				seg_show[3] = time[1] / 10;
				seg_show[4] = time[1] % 10;
				seg_show[5] = 11;
				seg_show[6] = time[2] / 10;
				seg_show[7] = time[2] % 10;
				break;
		 case 1:
				switch(fd_mode)
				{
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
				
				}
				seg_show[0] = 12;
				seg_show[1] = 10;
				seg_show[2] = 10;
				seg_show[3] = 10;
				seg_show[4] = 10;
				seg_show[5] = 10;
				seg_show[6] = 10;
				seg_show[7] = 10;
				break;
		 case 2:
				break;
		}
	}
	
}

void key_task(void)
{
	static uint8_t key_state = 0;
  uint8_t keynum = 0;
	if(key_slow)return;
	key_slow = 1;
	key_state = key_state << 4;
	key_state |= key_getstate();
	
	if((key_state & 0x11) == 0x10)keynum = 4;
	if((key_state & 0x22) == 0x20)keynum = 5;
	if((key_state & 0x44) == 0x40)keynum = 8;
	if((key_state & 0x88) == 0x80)keynum = 9;

  switch(keynum)
	{
	  case 4:
			show_mode++;
	    show_mode %= 3;
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
	}
}
void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;
  TMOD |= 0x04; 	//���ö�ʱ��ģʽ
	TL0 = 0x00;				//���ö�ʱ��ʼֵ
	TH0 = 0x00;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
}


void Timer1_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x40;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;			//���ö�ʱ��ģʽ
	TL1 = 0x20;				//���ö�ʱ��ʼֵ
	TH1 = 0xD1;				//���ö�ʱ��ʼֵ
	TF1 = 0;				//���TF1��־
	TR1 = 1;				//��ʱ��1��ʼ��ʱ
	ET1 = 1;				//ʹ�ܶ�ʱ��1�ж�
	EA = 1;
}

void Timer1_Isr(void) interrupt 3
{
	seg_slow++;
	seg_index++;
	key_slow++;
	
	if(seg_slow >= 90)seg_slow = 0;
	if(seg_index >= 8)seg_index = 0;
	if(key_slow >= 20)key_slow = 0;
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
}

void main(void)
{
  Timer1_Init();
  while(1)
	{
		seg_task();
	  key_task();
	}
}