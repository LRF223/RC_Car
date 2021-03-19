#include "motor.h"

void MOTOR_Init(void){ 																																							//MOTOR接口初始化
		GPIO_InitTypeDef  GPIO_InitStructure; 	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);       
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;  							//选择端口号（0~15或all）                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 																								//选择IO接口工作方式       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 																							//设置IO接口速度（2/10/50MHz）    
		GPIO_Init(MOTORPORT, &GPIO_InitStructure);			
}

void Go_Forward(void){
		GPIO_WriteBit(MOTORPORT,Motor_L1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_L2,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_R1,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_R2,(BitAction)(0)); 
		}

void Draw_Back(void){
		GPIO_WriteBit(MOTORPORT,Motor_L1,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_L2,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R2,(BitAction)(1)); 
		}
		
void Turn_left(void){
		GPIO_WriteBit(MOTORPORT,Motor_L1,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_L2,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R1,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_R2,(BitAction)(0)); 
		}

void Turn_right(void){
		GPIO_WriteBit(MOTORPORT,Motor_L1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_L2,(BitAction)(1)); 
		GPIO_WriteBit(MOTORPORT,Motor_R1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R2,(BitAction)(1)); 
		}

void Stop(void){
		GPIO_WriteBit(MOTORPORT,Motor_L1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_L2,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R1,(BitAction)(0)); 
		GPIO_WriteBit(MOTORPORT,Motor_R2,(BitAction)(0)); 
		}		
		
