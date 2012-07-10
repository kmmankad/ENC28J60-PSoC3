/*******************************************************************************
* File Name: SPIM_INT.c
* Version 2.30
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "SPIM.h"

#if (SPIM_RXBUFFERSIZE > 4u)

    extern volatile uint8 SPIM_RXBUFFER[];
    extern volatile uint8 SPIM_rxBufferRead;
    extern volatile uint8 SPIM_rxBufferWrite;
    extern volatile uint8 SPIM_rxBufferFull;
    
#endif /* SPIM_RXBUFFERSIZE > 4u */

#if (SPIM_TXBUFFERSIZE > 4u)

    extern volatile uint8 SPIM_TXBUFFER[];
    extern volatile uint8 SPIM_txBufferRead;
    extern volatile uint8 SPIM_txBufferWrite;
    extern volatile uint8 SPIM_txBufferFull;

#endif /* SPIM_TXBUFFERSIZE > 4u */

volatile uint8 SPIM_swStatusTx = 0u;
volatile uint8 SPIM_swStatusRx = 0u;

/* User code required at start of ISR */
/* `#START SPIM_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: SPIM_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to 
*  sending and FIFO Not Full.
*  SPIM_TXBUFFER[SPIM_TXBUFFERSIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SPIM_TX_ISR)
{     
    /* User code required at start of ISR */
    /* `#START SPIM_TX_ISR_START` */

    /* `#END` */
    
    #if((SPIM_InternalTxInterruptEnabled) && (SPIM_TXBUFFERSIZE > 4u))
                         
        /* See if TX data buffer is not empty and there is space in TX FIFO */
        while(SPIM_txBufferRead != SPIM_txBufferWrite)
        {
            SPIM_swStatusTx = SPIM_GET_STATUS_TX(SPIM_swStatusTx);
            
            if ((SPIM_swStatusTx & SPIM_STS_TX_FIFO_NOT_FULL) != 0u)
            {            
                if(SPIM_txBufferFull == 0u)
                {
                   SPIM_txBufferRead++;

                    if(SPIM_txBufferRead >= SPIM_TXBUFFERSIZE)
                    {
                        SPIM_txBufferRead = 0u;
                    }
                }
                else
                {
                    SPIM_txBufferFull = 0u;
                }
            
                /* Move data from the Buffer to the FIFO */
                CY_SET_REG8(SPIM_TXDATA_PTR,
                    SPIM_TXBUFFER[SPIM_txBufferRead]);
            }
            else
            {
                break;
            }            
        }
            
        /* Disable Interrupt on TX_fifo_not_empty if BUFFER is empty */
        if(SPIM_txBufferRead == SPIM_txBufferWrite)
        {
            SPIM_TX_STATUS_MASK_REG  &= ~SPIM_STS_TX_FIFO_NOT_FULL; 
        }                       
        
	#endif /* SPIM_InternalTxInterruptEnabled && (SPIM_TXBUFFERSIZE > 4u) */
    
    /* User code required at end of ISR (Optional) */
    /* `#START SPIM_TX_ISR_END` */

    /* `#END` */
   
}


/*******************************************************************************
* Function Name: SPIM_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  SPIM_RXBUFFER[SPIM_RXBUFFERSIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SPIM_RX_ISR)
{     
    #if((SPIM_InternalRxInterruptEnabled) && (SPIM_RXBUFFERSIZE > 4u))
        uint8 rxData = 0u; 
    #endif /* SPIM_InternalRxInterruptEnabled */  
    
    /* User code required at start of ISR */
    /* `#START SPIM_RX_ISR_START` */

    /* `#END` */
    
    #if((SPIM_InternalRxInterruptEnabled) && (SPIM_RXBUFFERSIZE > 4u))
         
        SPIM_swStatusRx = SPIM_GET_STATUS_RX(SPIM_swStatusRx);          
        
        /* See if RX data FIFO has some data and if it can be moved to the RX Buffer */
        while((SPIM_swStatusRx & SPIM_STS_RX_FIFO_NOT_EMPTY) == 
                                                                                SPIM_STS_RX_FIFO_NOT_EMPTY)
        {
            rxData = CY_GET_REG8(SPIM_RXDATA_PTR);
            
            /* Set next pointer. */
            SPIM_rxBufferWrite++;
            if(SPIM_rxBufferWrite >= SPIM_RXBUFFERSIZE)
            {
                SPIM_rxBufferWrite = 0u;
            }
            
            if(SPIM_rxBufferWrite == SPIM_rxBufferRead)
            {
                SPIM_rxBufferRead++;
                if(SPIM_rxBufferRead >= SPIM_RXBUFFERSIZE)
                {
                    SPIM_rxBufferRead = 0u;
                }
                SPIM_rxBufferFull = 1u;
            }
            
            /* Move data from the FIFO to the Buffer */
            SPIM_RXBUFFER[SPIM_rxBufferWrite] = rxData;
                
            SPIM_swStatusRx = SPIM_GET_STATUS_RX(SPIM_swStatusRx);
        }                    
        
	#endif /* SPIM_InternalRxInterruptEnabled  && (SPIM_RXBUFFERSIZE > 4u) */        
    
    /* User code required at end of ISR (Optional) */
    /* `#START SPIM_RX_ISR_END` */

    /* `#END` */
    
}

/* [] END OF FILE */
