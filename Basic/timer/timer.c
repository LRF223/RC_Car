#include "timer.h"
#include "usart.h"

TIM_ICInitTypeDef  TIM4_ICInitStructure;

void TIM4_CH3_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;              //PA2 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //PA2 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);						//PA2 ����
	
	//��ʼ����ʱ��2 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC3S=03 	ѡ������� IC3ӳ�䵽TI3��
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
 	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI3��
 	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
 	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
 	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC3IE�����ж�	
	
  TIM_Cmd(TIM4,ENABLE ); 	

}



u8  TIM4CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH3_CAPTURE_VAL;	//���벶��ֵ
u8  TIM4CH4_CAPTURE_STA = 0;	//���벶��״̬		    				
u16	TIM4CH4_CAPTURE_VAL;	//���벶��ֵ

void TIM4_IRQHandler(void)
{ 

 	if((TIM4CH3_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM4CH3_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM4CH3_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM4CH3_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIM4CH3_CAPTURE_VAL=0XFFFF;
				}
				else TIM4CH3_CAPTURE_STA++;
			}	 
		}
		if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//����1���������¼�
		{	
			if(TIM4CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM4CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4);
		   	TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								//��δ��ʼ,��һ�β���������
			{
				TIM4CH3_CAPTURE_STA=0;			//���
				TIM4CH3_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH3_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}			     	    					   
 	}
  TIM_ClearITPendingBit(TIM4, TIM_IT_CC3|TIM_IT_Update); //����жϱ�־λ
	
	if((TIM4CH4_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM4CH4_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM4CH4_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM4CH4_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIM4CH4_CAPTURE_VAL=0XFFFF;
				}
				else TIM4CH4_CAPTURE_STA++;
			}	 
		}
		if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//����1���������¼�
		{	
			if(TIM4CH4_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM4CH4_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
		   	TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								//��δ��ʼ,��һ�β���������
			{
				TIM4CH4_CAPTURE_STA=0;			//���
				TIM4CH4_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH4_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC4|TIM_IT_Update); //����жϱ�־λ
 
}


//TIM4CH4
void TIM4_CH4_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;              //PA2 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //PA2 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);						//PA2 ����
	
	//��ʼ����ʱ�� TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC4S=04 	ѡ������� IC4ӳ�䵽TI4��
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
 	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI4��
 	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
 	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
 	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�	
	
  TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4

}

