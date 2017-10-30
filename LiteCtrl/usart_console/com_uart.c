#include "stm32l0xx_hal.h"
#include "ring_buffer.h"

static HAL_StatusTypeDef uart_end_transmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable the UART Transmit Complete Interrupt */    
  __HAL_UART_DISABLE_IT(huart, UART_IT_TC);

  huart->gState = HAL_UART_STATE_READY;
  
  HAL_UART_TxCpltCallback(huart);
  
  return HAL_OK;
}

//HAL_UART_IRQHandler
static void uart_other_handle(UART_HandleTypeDef *huart)
{
	if((__HAL_UART_GET_IT(huart, UART_IT_PE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE) != RESET))
  { 
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);
    
    huart->ErrorCode |= HAL_UART_ERROR_PE;
    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }
  
  /* UART frame error interrupt occured --------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_FE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  { 
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);
    
    huart->ErrorCode |= HAL_UART_ERROR_FE;
    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }
  
  /* UART noise error interrupt occured --------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_NE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  { 
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);
    
    huart->ErrorCode |= HAL_UART_ERROR_NE;
    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }
  
  /* UART Over-Run interrupt occurred -----------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_ORE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  { 
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);
    
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }

  /* Call UART Error Call back function if need be --------------------------*/
  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }

  /* UART Wake Up interrupt occured ------------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_WUF) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_WUF) != RESET))
  { 
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_WUF);

    /* Set the UART state ready to be able to start again the process */
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;

  }
	
	  /* UART in mode Transmitter -- TC ------------------------------------------*/
	if((__HAL_UART_GET_IT(huart, UART_IT_TC) != RESET) &&(__HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC) != RESET))
	{
		uart_end_transmit_IT(huart);
	}
}

void handl_uart_send(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf)
{
	int len;
	char ch;
	uart_other_handle(pUartHand);
	if((__HAL_UART_GET_IT(pUartHand, UART_IT_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(pUartHand, UART_IT_TXE) != RESET))
	{
		len = pop_buf(pRbf,&ch,1);
		if(len>0)
		{
			pUartHand->Instance->TDR = (uint8_t)(ch & (uint8_t)0xFFU);
		}
		else
		{
			__HAL_UART_DISABLE_IT(pUartHand, UART_IT_TXE);
		}
	}
}

void handl_uart_recv(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf)
{
	char recv_ch;
	if((__HAL_UART_GET_IT(pUartHand, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(pUartHand, UART_IT_RXNE) != RESET))
  { 
		recv_ch = (char)(pUartHand->Instance->RDR);
    	push_buf(pRbf,&recv_ch,1);
		__HAL_UART_SEND_REQ(pUartHand, UART_RXDATA_FLUSH_REQUEST);
  }
}


int uart_printf(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf,uint8_t *buf,int len)
{
	int ret;
	ret = push_buf(pRbf,(char*)buf,len);
	if(ret>0)
		__HAL_UART_ENABLE_IT(pUartHand, UART_IT_TXE);
	return ret;
}


