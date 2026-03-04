#include <STC15F2K60S2.H>
#include "seg.h"
#include "led.h"
#include "key.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include "uart.h"

pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t seg_index = 0;

idata bit humi_mode = 0;
idata uint16_t humi_3s_count = 0;
idata uint8_t show_mode = 0;
idata uint8_t fd_mode = 0;

idata uint8_t led_state[] ={0,0,0,0,0,0,0,0};

idata uint8_t time[] = {13,3,5};
idata uint8_t time_cap[] = {0,0,0};
idata uint8_t cap_count = 0;
idata uint16_t temp_aver = 0;
idata uint16_t humi_aver = 0;

idata uint16_t light = 0;
idata bit light_state_pre = 0;
idata bit light_state_cur = 0;
idata uint16_t light_3s_count = 0;

idata uint16_t temp = 0;
idata uint16_t temp_max = 0;
idata uint16_t temp_limit = 30;

idata uint32_t fre = 0;
idata uint16_t fre_1s_count = 0;
idata uint8_t humi = 0;
idata uint8_t humi_max = 0;
idata bit humi_useful_flag = 0;

idata bit cap_flag = 0;
idata bit get_temp_flag = 0;
idata bit get_humi_flag = 0;

idata uint16_t key_2s_count = 0;
idata bit key9_press = 0;

idata uint8_t seg_slow = 0;
idata uint8_t key_slow = 0;
idata uint8_t time_slow = 0;
idata uint8_t light_slow = 0;
idata uint8_t temp_slow = 0;

void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
}

void time_task(void)
{
  if(time_slow)return;
	time_slow = 1;
  readtime(time);
}

void temp_task(void)
{
  if(temp_slow)return;
	temp_slow = 1;
	if(cap_flag)
	{
		if(!get_humi_flag)
		{
		  if(fre >= 200 && fre <= 2000)
			{
				  cap_count++;
				  time_cap[0] = time[0];
				  time_cap[1] = time[1];
			    humi_useful_flag = 1;
					humi = 2 * fre / 45.0 + 10 / 9.0;
          if(humi > humi_max)
					{
					  humi_max = humi;
					}
					humi_aver = (humi_aver * (cap_count - 1) + humi * 10 ) / cap_count;
					get_humi_flag = 1;
			}
			else
			{
			  humi_useful_flag = 0;
			}
		}
		
		if(!get_temp_flag)
		{
		  temp = readtemp();
			if(humi_useful_flag)
			{
				if(temp > temp_max)
				{
					temp_max = temp;
				}
				temp_aver = (temp_aver * (cap_count - 1) + temp * 10 ) / cap_count; 
			}
		  get_temp_flag = 1;
		}
	  return;
	}
}

void light_task(void)
{
  if(light_slow)return;
	light_slow = 1;
	if(cap_flag)return;
	
  light = readlight();
	
	light = light * 100 / 51.0;
	
	light_state_pre = light_state_cur;
	if(light >= 200)
	{
	  light_state_cur = 0;//亮
	}
	else
	{
	  light_state_cur = 1;//暗
	}
	if(light_state_cur == 1 && light_state_pre == 0)
	{
	  cap_flag = 1;
	}
}


void seg_task(void)
{
  if(seg_slow)return;
	seg_slow = 1;
	if(cap_flag)
	{
		seg_point[6] = 0;
		seg_show[0] = 13;
	  seg_show[1] = 10;
	  seg_show[2] = 10;
		if(humi_useful_flag)
		{
		 seg_show[3] = temp / 10;
		 seg_show[4] = temp % 10;
		 seg_show[5] = 11;
		 seg_show[6] = humi / 10;
		 seg_show[7] = humi % 10;
		}
		else
		{
		 seg_show[3] = temp / 10;
		 seg_show[4] = temp % 10;
		 seg_show[5] = 11;
		 seg_show[6] = 16;
		 seg_show[7] = 16;
		}
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
						if(cap_count != 0)
						{
							seg_show[0] = 12;
							seg_show[1] = 10;
							seg_show[2] = temp_max / 10;
							seg_show[3] = temp_max % 10;
							seg_show[4] = 11;
							seg_show[5] = temp_aver / 100;
							seg_show[6] = (temp_aver / 10) % 10;
							seg_point[6] = 1;
							seg_show[7] = temp_aver % 10;
						}
						else
						{
						  seg_show[0] = 12;
							seg_show[1] = 10;
							seg_show[2] = 10;
							seg_show[3] = 10;
							seg_show[4] = 10;
							seg_show[5] = 10;
							seg_show[6] = 10;
							seg_point[6] = 0;
							seg_show[7] = 10;
						}
						break;
					case 1:
						seg_show[0] = 17;
						seg_show[1] = 10;
					  if(cap_count != 0)
						{
							seg_show[2] = humi_max / 10;
							seg_show[3] = humi_max % 10;
							seg_show[4] = 11;
							seg_show[5] = humi_aver / 100;
							seg_show[6] = (humi_aver / 10) % 10;
							seg_point[6] = 1;
							seg_show[7] = humi_aver % 10;
						}
						else
						{
						  seg_show[2] = 10;
							seg_show[3] = 10;
							seg_show[4] = 10;
							seg_show[5] = 10;
							seg_show[6] = 10;
							seg_point[6] = 0;
							seg_show[7] = 10;
						}
						
						break;
					case 2:
						seg_show[0] = 14;
						if(cap_count != 0)
						{
							seg_show[1] = cap_count / 10;
							seg_show[2] = cap_count % 10;
							seg_show[3] = time_cap[0]/10;
							seg_show[4] = time_cap[0] % 10;
							seg_show[5] = 11;
							seg_show[6] = time_cap[1] /10;
							seg_point[6] = 0;
							seg_show[7] = time_cap[1] % 10;
						}
						else
						{
							seg_show[1] = cap_count / 10;
							seg_show[2] = cap_count % 10;
							seg_show[3] = 10;
							seg_show[4] = 10;
							seg_show[5] = 10;
							seg_show[6] = 10;
							seg_point[6] = 0;
							seg_show[7] = 10;
						}
						break;
				}
        break;
		 case 2:
			  seg_show[0] = 15;
				seg_show[1] = 10;
				seg_show[2] = 10;
				seg_show[3] = 10;
				seg_show[4] = 10;
				seg_show[5] = 10;
				seg_show[6] = temp_limit / 10;
				seg_point[6] = 0;
				seg_show[7] = temp_limit % 10;
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

	if((key_state & 0x88) == 0x88)key9_press = 1;
  if(key_2s_count >= 2000)
	{
	  cap_count = 0;
		temp = 0;
		temp_max = 0;
		temp_aver = 0;
		humi = 0;
		humi_max = 0;
		humi_aver = 0;
		time_cap[0] = 0;
		time_cap[1] = 0;
	}
  switch(keynum)
	{
	  case 4:
			show_mode++;
	    show_mode %= 3;
			if(show_mode == 0)
			{
			  fd_mode ==  0;
			}
			break;
		case 5:
			if(show_mode == 1)
			{
			  fd_mode++;
			  fd_mode %= 3;
			}
			break;
		case 8:
			if(show_mode== 2)
			{
			  temp_limit++;
			}
			break;
		case 9:
			if(show_mode == 2)
			{
			  temp_limit--;
			}
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
	time_slow++;
	temp_slow++;
	light_slow++;
	
	if(cap_flag)
	{
	  light_3s_count++;
		if(light_3s_count >= 3000)
		{
		  cap_flag = 0;
			get_humi_flag = 0;
			get_temp_flag = 0;
			humi_useful_flag = 0;
			light_3s_count = 0;
		}
	}
	
	fre_1s_count++;
	if(fre_1s_count++ >= 1000)
	{
	  TR0 = 0;
		fre = (TH0 << 8) | TL0;

		TH0 = 0;
		TL0 = 0;
		TR0 = 1;
		fre_1s_count = 0;
	}
	
	if(key9_press)
	{
	  key_2s_count++;
	}
	else
	{
	  key_2s_count = 0;
	}
	if(seg_slow >= 90)seg_slow = 0;
	if(seg_index >= 8)seg_index = 0;
	if(key_slow >= 20)key_slow = 0;
	if(time_slow >= 160)time_slow = 0;
	if(temp_slow >= 160)temp_slow = 0;
	if(light_slow >= 100)light_slow=0;
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
}

void main(void)
{
  Timer1_Init();
	Timer0_Init();
	Uart1_Init();
	writetime(time);
	readtemp();
  while(1)
	{
		time_task();
		seg_task();
	  key_task();
		light_task();
		temp_task();
	}
}