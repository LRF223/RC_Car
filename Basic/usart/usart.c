
#include "sys.h"
#include "usart.h"
	  	 
//ʹUASRT���ڿ���printf��������
//��usart.h�ļ���ɸ���ʹ��printf�����Ĵ��ں�	  
#if 1
#pragma import(__use_no_semihosting)            //��׼����Ҫ��֧�ֺ���   
              
struct __FILE {int handle; }; 

FILE __stdout;																	//����_sys_exit()�Ա���ʹ�ð�����ģʽ   

_sys_exit(int x){x = x;}												//�ض���fputc���� 

int fputc(int ch, FILE *f)
	{      
	while((USART_n->SR&0X40)==0);									//ѭ������,ֱ���������   
  USART_n->DR = (u8) ch;      
	return ch;
	}
#endif 

/*****************USART1������س���*************************/
 
#if EN_USART1																		//USART1ʹ��������ѡ��
u8 USART1_RX_BUF[USART1_REC_LEN];    						//���ջ���,���USART_REC_LEN���ֽ�.
//����״̬	
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       											//����״̬���	  

/*
USART1ר�õ�printf����
��ͬʱ����2�����ϴ���ʱ��printf����ֻ����������֮һ����������Ҫ�Դ�������printf����
���÷�����USART1_printf("123"); //��USART2�����ַ�123
*/
void USART1_printf (char *fmt, ...){ 
	char buffer[USART1_REC_LEN+1]; 							 // ���ݳ���
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART1_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART1_REC_LEN) && (i < strlen(buffer))){
        USART_SendData(USART1, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

void USART1_Init(u32 bound){ 																										//����1��ʼ��������
	
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure; 			
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);			//ʹ��USART1��GPIOAʱ��
	
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;																//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;													//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;												//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;															//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																									//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
	
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;																			//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;															//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); 																			//��ʼ������
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);																	//����ENABLE/�ر�DISABLE�ж�
  USART_Cmd(USART1, ENABLE);																											//ʹ�ܴ��� 
}

void USART1_IRQHandler(void){ 																										//����1�жϷ�����򣨹̶��ĺ����������޸ģ�	
	u8 a;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  												//�����ж�(���յ������ݱ�����0x0d 0x0a��β)		
		a =USART_ReceiveData(USART1);																									//��ȡ���յ�������
		printf("%c",a); 																															//���յ������ݷ��ͻص���		  
	} 
} 
#endif	


/******************USART2������س���**************************/

#if EN_USART2   																							//USART2ʹ��������ѡ��
u8 USART2_RX_BUF[USART2_REC_LEN];															//���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  

/*
USART2ר�õ�printf����
��ͬʱ����2�����ϴ���ʱ��printf����ֻ����������֮һ����������Ҫ�Դ�������printf����
���÷�����USART2_printf("123"); //��USART2�����ַ�123
*/
void USART2_printf (char *fmt, ...){ 
	char buffer[USART2_REC_LEN+1];																							// ���ݳ���
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART2_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART2_REC_LEN) && (i < strlen(buffer))){
        USART_SendData(USART2, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}


void USART2_Init(u32 bound){																									//����1��ʼ��������

	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
		 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 											//ʹ��UART2����GPIOA��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 											//ʹ�ܴ��ڵ�RCCʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;																	//����USART2��RX�ӿ���PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;											//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 																//����USART2��TX�ӿ���PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;														//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;																				//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;												//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;														//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;																//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;										//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure);																					//��ʼ������
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);																		//����ENABLE/�ر�DISABLE�ж�
  USART_Cmd(USART2, ENABLE);                   																			//ʹ�ܴ��� 
 
	//Usart2 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;													//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;																//���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																										//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
}

void USART2_IRQHandler(void){ 																											//����2�жϷ�����򣨹̶��ĺ����������޸ģ�	

} 
#endif	


