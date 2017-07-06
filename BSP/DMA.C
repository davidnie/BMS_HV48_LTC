/***********************************************************************
文件名称：DMA.C
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

#include <stm32f4xx.h>
unsigned char RS232_send_data[RS232_REC_BUFF_SIZE];
volatile unsigned char RS232_dma_send_flag = 0;
unsigned char U4_send_data[RS232_REC_BUFF_SIZE];
volatile unsigned char U4_dma_send_flag = 0;
 volatile unsigned char RS232_REC_Flag ;
 volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE] ;//用于接收数据
 volatile unsigned int RS232_rec_counter ;//用于RS232接收计数
 unsigned short RS232_send_data_length;
 void SPI2_DMA_Send(unsigned char *send_buff,unsigned int length);


/***********************************************************************
函数名称：void RS232_DMA_Init(void) 
功    能：
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：RS232用的是串口1 对应的DMA通道是DMA2_Channel4
***********************************************************************/
void RS232_DMA_Init(void)
{
	
#if 1
	DMA_InitTypeDef DMA_InitStructure;
	//enable DMA 1 clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  

    DMA_DeInit(DMA1_Stream3);					//receive
    DMA_InitStructure.DMA_Channel            = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI2->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)(&RS232_buff);
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = RS232_REC_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);                      
    //NVIC_EnableIRQ(DMA_Channe3_0);     
    DMA_Cmd(DMA1_Stream3, ENABLE);  


    DMA_DeInit(DMA1_Stream4);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_0;//send
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI1->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)(&RS232_send_data);
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = RS232_REC_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);  
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC , ENABLE);	  
    DMA_Cmd(DMA1_Stream4, DISABLE);  
#endif 
}

//UART2 serial output DMA interrupt

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
		//RS232_dma_send_flag = 0;										//允许再次发送
		DMA_Cmd(DMA2_Stream7, DISABLE); 
	}
}
void UART4_DMA_Init(void)
{
	
#if 0
	DMA_InitTypeDef DMA_InitStructure;
	//启动DMA时钟  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  

    DMA_DeInit(DMA2_Stream4);					//接收管道
    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)(&U4_buff);
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = RS232_REC_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream4, &DMA_InitStructure);                      
    NVIC_EnableIRQ(DMA_Channel_4);     
    DMA_Cmd(DMA2_Stream4, ENABLE);  


    DMA_DeInit(DMA2_Stream6);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;//发送管道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)(&U4_send_data);
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = RS232_REC_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream6, &DMA_InitStructure);  
	DMA_ITConfig(DMA2_Stream6, DMA_IT_TC , ENABLE);	  
    DMA_Cmd(DMA2_Stream6, DISABLE);  
	#endif

}

//UART 2 DMA send interrupt
void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF7))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF7);
		//RS232_dma_send_flag = 0;										//allow send again
		DMA_Cmd(DMA2_Stream2, DISABLE); 
	}
}

void DMA1_Stream4_IRQHandler(void)
{
	//if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF7))
	{
		RS232_dma_send_flag = 0;
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF7);
		//RS232_dma_send_flag = 0;										//allow send again
		DMA_Cmd(DMA1_Stream4, DISABLE); 
	}
}
void SPI2_DMA_Send(unsigned char *send_buff,unsigned int length)
{
	
	unsigned int counter_232 = 0;
	while(RS232_dma_send_flag != 0)					//等待上次的DMA传输完成
	{
		counter_232 ++;
		if(counter_232 >= 0xffff)
		{
			break;
		}
	}						 
	RS232_dma_send_flag = 1;
	DMA1_Stream4 -> M0AR = (u32)send_buff;
	//DMA2_Stream3 -> M0AR = (u32)RS232_send_data;
	DMA_SetCurrDataCounter(DMA1_Stream4,length);  	//设置传输长度
	DMA_Cmd(DMA1_Stream4,ENABLE);					//启动DMA传输
}

