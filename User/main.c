#include "stm32f10x.h" 
#include "sys.h"
#include "delay.h"
#include "motor.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"
#include <math.h>

extern u8   TIM4CH3_CAPTURE_STA;	//���벶��״̬		    				
extern u16	TIM4CH3_CAPTURE_VAL;	//���벶��ֵ
extern u8   TIM4CH4_CAPTURE_STA;	//���벶��״̬		    				
extern u16	TIM4CH4_CAPTURE_VAL;	//���벶��ֵ

int main (void)
{
	int b = 1;//��ʱus
  u32 temp3=0; 
	u32 temp4=0; 
	int speed=0;
	u32 stop=4600;//�����ת�ĵ�ֵ
	
	RCC_Configuration(); 								//ʱ������
	MOTOR_Init();												//LED��ʼ��
	TIM3_PWM_Init(7200-1,0); 						//PWMʱ��Ƶ��=72000/(7200)*(1) = 10kHZ 
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_Init(115200); 	 //���ڳ�ʼ��Ϊ115200
	TIM4_CH3_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� 	
	TIM4_CH4_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� 	
	
	delay_s(3);//�Ƚ��ջ���ʼ�����
	
	while(1)
	{		
	
		TIM_SetCompare3(TIM3,speed);					
		TIM_SetCompare4(TIM3,speed);	

		//����ң�����ź�
	  if(TIM4CH3_CAPTURE_STA&0X80)		//�ɹ�������һ��������
		{
      temp3=TIM4CH3_CAPTURE_STA&0X3F;
			temp3*=65536;		//���ʱ���ܺ�
			temp3+=TIM4CH3_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
      USART1_printf("CH1:%d		",temp3); 
      TIM4CH3_CAPTURE_STA = 0;        
    }
    if(TIM4CH4_CAPTURE_STA&0X80)		//�ɹ�������һ��������
		{
      temp4=TIM4CH4_CAPTURE_STA&0X3F;
			temp4*=65536;		//���ʱ���ܺ�
			temp4+=TIM4CH4_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
      USART1_printf("CH3:%d		",temp4); 
      TIM4CH4_CAPTURE_STA = 0;        
    }	
		
		//ң���źŴ���	
		if(temp4<850)//û����
		{
			Stop();
		}		
		else if(temp4>1550)//ǰ��
		{
			if(temp3<1490)//����
			{
				speed	= temp3 - 1500;
				speed *= -1;
				speed *= 5;			
				speed += stop;
				Turn_left();
				delay_us(b);
			}
			else if(temp3>1510)//����
			{
				speed	= temp3 - 1500;
				speed *= 5;			
				speed += stop;
				Turn_right();	
				delay_us(b);			
			}
			else if(1490<temp3<1510)//����˻���
			{
			speed	= temp4 - 1500;
			speed *= 5;
			speed += stop;				
			Go_Forward();
			delay_us(b);
			}
		}	
		else if(temp4<1450)//����
		{
			if(temp3<1490)//����
			{
				speed	= temp3 - 1500;
				speed *= -1;
				speed *= 5;			
				speed += stop;
				Turn_left();
				delay_us(b);
			}
			else if(temp3>1510)//����
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
		else if(temp3<1490)//����
		{
			speed	= temp3 - 1500;
			speed *= -1;
			speed *= 5;			
			speed += stop;
			Turn_left();
			delay_us(b);
		}		
		else if(temp3>1510)//����
		{
			speed	= temp3 - 1500;
			speed *= 5;			
			speed += stop;
			Turn_right();	
			delay_us(b);			
		}		
		else if(1490<temp3<1510&&temp4<1550)//�������Ż���
		{
			Stop();
		}
		
		USART1_printf("speed=%d\r\n",speed); 
	}
}
