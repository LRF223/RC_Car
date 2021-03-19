#include "stm32f10x.h" 
#include "sys.h"
#include "delay.h"
#include "motor.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"
#include <math.h>

extern u8   TIM4CH3_CAPTURE_STA;	//输入捕获状态		    				
extern u16	TIM4CH3_CAPTURE_VAL;	//输入捕获值
extern u8   TIM4CH4_CAPTURE_STA;	//输入捕获状态		    				
extern u16	TIM4CH4_CAPTURE_VAL;	//输入捕获值

int main (void)
{
	int b = 1;//延时us
  u32 temp3=0; 
	u32 temp4=0; 
	int speed=0;
	u32 stop=4600;//电机不转的底值
	
	RCC_Configuration(); 								//时钟设置
	MOTOR_Init();												//LED初始化
	TIM3_PWM_Init(7200-1,0); 						//PWM时钟频率=72000/(7200)*(1) = 10kHZ 
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	USART1_Init(115200); 	 //串口初始化为115200
	TIM4_CH3_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数 	
	TIM4_CH4_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数 	
	
	delay_s(3);//等接收机初始化完成
	
	while(1)
	{		
	
		TIM_SetCompare3(TIM3,speed);					
		TIM_SetCompare4(TIM3,speed);	

		//接收遥控器信号
	  if(TIM4CH3_CAPTURE_STA&0X80)		//成功捕获到了一次上升沿
		{
      temp3=TIM4CH3_CAPTURE_STA&0X3F;
			temp3*=65536;		//溢出时间总和
			temp3+=TIM4CH3_CAPTURE_VAL;		//得到总的高电平时间
      USART1_printf("CH1:%d		",temp3); 
      TIM4CH3_CAPTURE_STA = 0;        
    }
    if(TIM4CH4_CAPTURE_STA&0X80)		//成功捕获到了一次上升沿
		{
      temp4=TIM4CH4_CAPTURE_STA&0X3F;
			temp4*=65536;		//溢出时间总和
			temp4+=TIM4CH4_CAPTURE_VAL;		//得到总的高电平时间
      USART1_printf("CH3:%d		",temp4); 
      TIM4CH4_CAPTURE_STA = 0;        
    }	
		
		//遥控信号处理	
		if(temp4<850)//没开控
		{
			Stop();
		}		
		else if(temp4>1550)//前进
		{
			if(temp3<1490)//向左
			{
				speed	= temp3 - 1500;
				speed *= -1;
				speed *= 5;			
				speed += stop;
				Turn_left();
				delay_us(b);
			}
			else if(temp3>1510)//向右
			{
				speed	= temp3 - 1500;
				speed *= 5;			
				speed += stop;
				Turn_right();	
				delay_us(b);			
			}
			else if(1490<temp3<1510)//方向杆回中
			{
			speed	= temp4 - 1500;
			speed *= 5;
			speed += stop;				
			Go_Forward();
			delay_us(b);
			}
		}	
		else if(temp4<1450)//后退
		{
			if(temp3<1490)//向左
			{
				speed	= temp3 - 1500;
				speed *= -1;
				speed *= 5;			
				speed += stop;
				Turn_left();
				delay_us(b);
			}
			else if(temp3>1510)//向右
			{
				speed	= temp3 - 1500;
				speed *= 5;			
				speed += stop;
				Turn_right();	
				delay_us(b);			
			}
			else if(1490<temp3<1510)
			{
			speed	= temp4 - 1500;
			speed *= -1;
			speed *= 5;			
			speed += stop;	
			Draw_Back();
			delay_us(b);
			}
		}		
		else if(temp3<1490)//向左
		{
			speed	= temp3 - 1500;
			speed *= -1;
			speed *= 5;			
			speed += stop;
			Turn_left();
			delay_us(b);
		}		
		else if(temp3>1510)//向右
		{
			speed	= temp3 - 1500;
			speed *= 5;			
			speed += stop;
			Turn_right();	
			delay_us(b);			
		}		
		else if(1490<temp3<1510&&temp4<1550)//方向、油门回中
		{
			Stop();
		}
		
		USART1_printf("speed=%d\r\n",speed); 
	}
}
