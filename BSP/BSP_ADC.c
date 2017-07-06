/***********************************************************************
BSP_ADC.C
***********************************************************************/
#include "main.h"

//uint16_t ADC_REV_Instant[ADC_CHANNLE_NUM] ;   //
uint16_t ADC_REV_Instant[ADC_CHANNLE_NUM] ;   //
uint32_t ADC_REV_SUM[ADC_CHANNLE_NUM] ;   //
unsigned char ADC_dma_ready_flag;
uint32_t dma_trig_cnt=0;
/***********************************************************************
name: void ADC_Configuration(void)
function:
input:
output:
time:
developer:
note:
***********************************************************************/
void ADC_Configuration(void)
{
	
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	/* enable clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DMA2 Stream0 channel0 configuration----------------------------------------- */
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_0;//ADC1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)(&ADC_REV_Instant);
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = 16;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);  

	NVIC_EnableIRQ(DMA_Channel_0);     
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC , ENABLE);	  
	/* DMA2_Stream0 enable */
    DMA_Cmd(DMA2_Stream0, ENABLE);  
    
    /****************************************************************************   
	  PCLK2 = HCLK / 2 
	 select frequency divider
	  ADCCLK = PCLK2 /8 = HCLK / 8 = 168 / 8 = 21M
      ADC sample frenquency: Sampling Time + Conversion Time = 480 + 12 cycles = 492cyc
                    Conversion Time = 21MHz / 492cyc = 42.6ksps. 
	*****************************************************************************/
    
	/* ADC Common configuartion ----------------------------------------------------------*/
     ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 regular channel 12 configuration ************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 16;
    ADC_Init(ADC1, &ADC_InitStructure);
    
     /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* ADC1 regular channel18 (VBAT) configuration ******************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_3Cycles);	  	//PA0 0-5V battery voltage
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_3Cycles);	   	//PA1 5V output

    /* Enable VBAT channel */
    ADC_VBATCmd(ENABLE); 
    
    ADC_TempSensorVrefintCmd(ENABLE); 

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC1 **************************************************************/
    ADC_Cmd(ADC1, ENABLE);
    
        /* Start ADC1 Software Conversion */
   ADC_SoftwareStartConv(ADC1);
}

void DMA2_Stream0_IRQHandler(void)
{
	
	unsigned int i = 0;
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		ADC_dma_ready_flag = 1;										//允许再次发送
	//	DMA_Cmd(DMA2_Stream0, DISABLE); 
		for(i=0;i<ADC_CHANNLE_NUM;i++){
			ADC_REV_SUM[i]+=(uint32_t)ADC_REV_Instant[i];
		}
		dma_trig_cnt++;
	}
}

