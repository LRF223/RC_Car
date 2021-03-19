
#include "sys.h"
#include "usart.h"
	  	 
//使UASRT串口可用printf函数发送
//在usart.h文件里可更换使用printf函数的串口号	  
#if 1
#pragma import(__use_no_semihosting)            //标准库需要的支持函数   
              
struct __FILE {int handle; }; 

FILE __stdout;																	//定义_sys_exit()以避免使用半主机模式   

_sys_exit(int x){x = x;}												//重定义fputc函数 

int fputc(int ch, FILE *f)
	{      
	while((USART_n->SR&0X40)==0);									//循环发送,直到发送完毕   
  USART_n->DR = (u8) ch;      
	return ch;
	}
#endif 

/*****************USART1串口相关程序*************************/
 
#if EN_USART1																		//USART1使用与屏蔽选择
u8 USART1_RX_BUF[USART1_REC_LEN];    						//接收缓冲,最大USART_REC_LEN个字节.
//接收状态	
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       											//接收状态标记	  

/*
USART1专用的printf函数
当同时开启2个以上串口时，printf函数只能用于其中之一，其他串口要自创独立的printf函数
调用方法：USART1_printf("123"); //向USART2发送字符123
*/
void USART1_printf (char *fmt, ...){ 
	char buffer[USART1_REC_LEN+1]; 							 // 数据长度
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

void USART1_Init(u32 bound){ 																										//串口1初始化并启动
	
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure; 			
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);			//使能USART1，GPIOA时钟
	
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;																//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;													//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;												//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;															//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);																									//根据指定的参数初始化VIC寄存器 
	
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;																			//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式
  USART_Init(USART1, &USART_InitStructure); 																			//初始化串口
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);																	//开启ENABLE/关闭DISABLE中断
  USART_Cmd(USART1, ENABLE);																											//使能串口 
}

void USART1_IRQHandler(void){ 																										//串口1中断服务程序（固定的函数名不能修改）	
	u8 a;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  												//接收中断(接收到的数据必须是0x0d 0x0a结尾)		
		a =USART_ReceiveData(USART1);																									//读取接收到的数据
		printf("%c",a); 																															//把收到的数据发送回电脑		  
	} 
} 
#endif	
