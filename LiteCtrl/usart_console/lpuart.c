#include "stm32l0xx_hal.h"
#include "ring_buffer.h"
#include "com_uart.h"

static UART_HandleTypeDef lpuartStruct;
void init_lpuart_gpio()  
{    								
  GPIO_InitTypeDef  GPIO_InitStruct;  
    
  /*##-1- Enable peripherals and GPIO Clocks #################################*/  
  /* Enable GPIO TX/RX clock */  
  //__HAL_RCC_GPIOB_CLK_ENABLE();  
  /* Enable USART1 clock */  
  //__HAL_RCC_LPUART1_CLK_ENABLE();   
    
  /*##-2- Configure peripheral GPIO ##########################################*/    
  /* UART TX GPIO pin configuration  */  
  GPIO_InitStruct.Pin       = GPIO_PIN_10;  
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;  
  GPIO_InitStruct.Pull      = GPIO_PULLUP;  
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;  
  GPIO_InitStruct.Alternate = GPIO_AF4_LPUART1;  
    
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
      
  /* UART RX GPIO pin configuration  */  
  GPIO_InitStruct.Pin = GPIO_PIN_11;  
  GPIO_InitStruct.Alternate = GPIO_AF4_LPUART1;  
      
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  

}  

static void exit_lpuart_gpio() 
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = GPIO_PIN_10;  
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;  
	GPIO_InitStruct.Pull      = GPIO_PULLUP;  
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;   
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
	  
	/* UART RX GPIO pin configuration  */  
	GPIO_InitStruct.Pin = GPIO_PIN_11;  	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10 | GPIO_PIN_11,GPIO_PIN_SET);
}

static void enrty_lpuart_gpio()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = GPIO_PIN_10;  
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;  
	GPIO_InitStruct.Pull      = GPIO_PULLUP;  
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;  
	GPIO_InitStruct.Alternate = GPIO_AF4_LPUART1;  

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
	  
	/* UART RX GPIO pin configuration  */  
	GPIO_InitStruct.Pin = GPIO_PIN_11;  
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
}

static void init_lpuart_t(UART_HandleTypeDef *lpuart_handle,uint32_t BaudRate)
{
	//UART_HandleTypeDef  UartHandle;
    lpuart_handle->Instance        = LPUART1;  
    lpuart_handle->Init.BaudRate   = BaudRate;  
    lpuart_handle->Init.WordLength = UART_WORDLENGTH_8B;  
    lpuart_handle->Init.StopBits   = UART_STOPBITS_1;  
    lpuart_handle->Init.Parity     = UART_PARITY_NONE;  
    lpuart_handle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;  
    lpuart_handle->Init.Mode       = UART_MODE_TX_RX;  
//    if(HAL_UART_Init(lpuart_handle) != HAL_OK)  
//    {  
//       while(1);
//    }  	
}

#define LP_TX_BUF_AMX 1024
static uint8_t lpuar_send_buf[LP_TX_BUF_AMX];
static RING_BUF lp_ring_send_buf;

#define LP_RX_BUF_AMX 64
static uint8_t lpuart_recv_buf[LP_RX_BUF_AMX];
static RING_BUF lp_ring_recv_buf;

void init_lpuart(uint32_t BaudRate)
{
	init_lpuart_t(&lpuartStruct,BaudRate);
	init_ring_buf(&lp_ring_send_buf, (char *)lpuar_send_buf, LP_TX_BUF_AMX);
	init_ring_buf(&lp_ring_recv_buf, (char *)lpuart_recv_buf, LP_RX_BUF_AMX);
}
 


void LPUART1_IRQHandler(void)
{
	handl_uart_send(&lpuartStruct,&lp_ring_send_buf);
	handl_uart_recv(&lpuartStruct,&lp_ring_recv_buf);
}

int lpuart_read(uint8_t *buf,int len)
{
	return pop_buf(&lp_ring_recv_buf,(char*)buf,len);
}
int lpuart_printf(uint8_t *buf,int len)
{
	return uart_printf(&lpuartStruct,&lp_ring_send_buf,buf,len);
}

void stop_lpuart()
{
	__HAL_RCC_LPUART1_CLK_DISABLE();
	__HAL_UART_DISABLE(&lpuartStruct);
	HAL_NVIC_DisableIRQ(LPUART1_IRQn);
	exit_lpuart_gpio();
}

void start_lpuart()
{
	enrty_lpuart_gpio();
	__HAL_RCC_LPUART1_CLK_ENABLE(); 
	HAL_UART_Init(&lpuartStruct);
	__HAL_UART_ENABLE(&lpuartStruct);
	HAL_NVIC_SetPriority(LPUART1_IRQn, 5, 5);  
	HAL_NVIC_EnableIRQ(LPUART1_IRQn);
	__HAL_UART_ENABLE_IT(&lpuartStruct, UART_IT_RXNE);
}
