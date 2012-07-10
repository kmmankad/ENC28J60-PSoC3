/*******************************************************************************
* File Name: SPIM.c
* Version 2.30
*
* Description:
*  This file provides all API functionality of the SPI Master component.
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

#include "CyLib.h"
#include "SPIM.h"

#if(SPIM_InternalClockUsed)
    #include "SPIM_IntClock.h"   
#endif /* SPIM_InternalClockUsed */

#if (SPIM_TXBUFFERSIZE > 4u)

    volatile uint8 SPIM_TXBUFFER[SPIM_TXBUFFERSIZE] = {0u};
    volatile uint8 SPIM_txBufferRead = 0u;
    volatile uint8 SPIM_txBufferWrite = 0u;
    volatile uint8 SPIM_txBufferFull = 0u;
    
#endif /* SPIM_TXBUFFERSIZE > 4u */

#if (SPIM_RXBUFFERSIZE > 4u)

    volatile uint8 SPIM_RXBUFFER[SPIM_RXBUFFERSIZE] = {0u};
    volatile uint8 SPIM_rxBufferRead = 0u;
    volatile uint8 SPIM_rxBufferWrite = 0u;
    volatile uint8 SPIM_rxBufferFull = 0u;
    
#endif /* SPIM_RXBUFFERSIZE > 4u */

uint8 SPIM_initVar = 0u;

extern volatile uint8 SPIM_swStatusTx;
extern volatile uint8 SPIM_swStatusRx;


/*******************************************************************************
* Function Name: SPIM_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the 
*  interrupt service routine, configuring the bit-counter parameters and 
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_Init(void) 
{    
    /* Initialize the Bit counter */
    SPIM_COUNTER_PERIOD_REG = SPIM_BITCTR_INIT;
    
    /* ISR initialization  */  
    #if(SPIM_InternalTxInterruptEnabled)
    
        CyIntDisable(SPIM_TX_ISR_NUMBER);

        /* Set the ISR to point to the SPIM_isr Interrupt. */
        CyIntSetVector(SPIM_TX_ISR_NUMBER, SPIM_TX_ISR);

        /* Set the priority. */
        CyIntSetPriority(SPIM_TX_ISR_NUMBER, SPIM_TX_ISR_PRIORITY);
        
    #endif /* SPIM_InternalTxInterruptEnabled */                                
    
    #if(SPIM_InternalRxInterruptEnabled)
    
        CyIntDisable(SPIM_RX_ISR_NUMBER);

        /* Set the ISR to point to the SPIM_isr Interrupt. */
        CyIntSetVector(SPIM_RX_ISR_NUMBER, SPIM_RX_ISR);

        /* Set the priority. */
        CyIntSetPriority(SPIM_RX_ISR_NUMBER, SPIM_RX_ISR_PRIORITY);
        
    #endif /* SPIM_InternalRxInterruptEnabled */
    
    /* Clear any stray data from the RX and TX FIFO */    
	SPIM_ClearFIFO();
	
	#if(SPIM_RXBUFFERSIZE > 4u)
    
        SPIM_rxBufferRead = 0u;
        SPIM_rxBufferWrite = 0u;

    #endif /* SPIM_RXBUFFERSIZE > 4u */
	
    #if(SPIM_TXBUFFERSIZE > 4u)
    
        SPIM_txBufferRead = 0u;
        SPIM_txBufferWrite = 0u;

    #endif /* SPIM_TXBUFFERSIZE > 4u */
    
    (void) SPIM_ReadTxStatus(); /* Clear any pending status bits */
    (void) SPIM_ReadRxStatus(); /* Clear any pending status bits */
	
	/* Configure the Initial interrupt mask */
    #if (SPIM_TXBUFFERSIZE > 4u)
        SPIM_TX_STATUS_MASK_REG  = SPIM_TX_INIT_INTERRUPTS_MASK & 
                                                ~SPIM_STS_TX_FIFO_NOT_FULL;                    
	#else /* SPIM_TXBUFFERSIZE < 4u */    
        SPIM_TX_STATUS_MASK_REG  = SPIM_TX_INIT_INTERRUPTS_MASK;       
	#endif /* SPIM_TXBUFFERSIZE > 4u */
    
    SPIM_RX_STATUS_MASK_REG  = SPIM_RX_INIT_INTERRUPTS_MASK; 
}
  
    
/*******************************************************************************
* Function Name: SPIM_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPIM_Enable(void) 
{             
    uint8 enableInterrupts = 0u;    
    
    enableInterrupts = CyEnterCriticalSection();
    
    SPIM_COUNTER_CONTROL_REG |= SPIM_CNTR_ENABLE;
    SPIM_TX_STATUS_ACTL_REG |= SPIM_INT_ENABLE;
    SPIM_RX_STATUS_ACTL_REG |= SPIM_INT_ENABLE;
    
    CyExitCriticalSection(enableInterrupts);
    
    #if(SPIM_InternalClockUsed)    
        SPIM_IntClock_Enable();        
    #endif /* SPIM_InternalClockUsed */
    
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntEnable(SPIM_TX_ISR_NUMBER);        
    #endif /* SPIM_InternalTxInterruptEnabled */
    
    #if(SPIM_InternalRxInterruptEnabled)    
        CyIntEnable(SPIM_RX_ISR_NUMBER);        
    #endif /* SPIM_InternalRxInterruptEnabled */
}


/*******************************************************************************
* Function Name: SPIM_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_Start(void) 
{       
    if(SPIM_initVar == 0u)
    {               
        SPIM_Init();
        SPIM_initVar = 1u; 
    }                       
        
    SPIM_Enable();        
}


/*******************************************************************************
* Function Name: SPIM_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void SPIM_Stop(void) 
{
    uint8 enableInterrupts = 0u;    
    
    enableInterrupts = CyEnterCriticalSection();
    
    SPIM_TX_STATUS_ACTL_REG &= ~SPIM_INT_ENABLE;
    SPIM_RX_STATUS_ACTL_REG &= ~SPIM_INT_ENABLE;
    
    CyExitCriticalSection(enableInterrupts);
    
    #if(SPIM_InternalClockUsed)    
        SPIM_IntClock_Disable();        
    #endif /* SPIM_InternalClockUsed */
    
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntDisable(SPIM_TX_ISR_NUMBER);        
    #endif /* SPIM_InternalTxInterruptEnabled */
    
    #if(SPIM_InternalRxInterruptEnabled)    
        CyIntDisable(SPIM_RX_ISR_NUMBER);        
    #endif /* SPIM_InternalRxInterruptEnabled */
}


/*******************************************************************************
* Function Name: SPIM_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_EnableTxInt(void) 
{       
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntEnable(SPIM_TX_ISR_NUMBER);
    #endif /* SPIM_InternalTxInterruptEnabled */     
}


/*******************************************************************************
* Function Name: SPIM_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_EnableRxInt(void) 
{       
    #if(SPIM_InternalRxInterruptEnabled)            
        CyIntEnable(SPIM_RX_ISR_NUMBER);
    #endif /* SPIM_InternalRxInterruptEnabled */     
}


/*******************************************************************************
* Function Name: SPIM_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_DisableTxInt(void) 
{
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntDisable(SPIM_TX_ISR_NUMBER);        
    #endif /* SPIM_InternalTxInterruptEnabled */
}


/*******************************************************************************
* Function Name: SPIM_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_DisableRxInt(void) 
{
    #if(SPIM_InternalRxInterruptEnabled)    
        CyIntDisable(SPIM_RX_ISR_NUMBER);        
    #endif /* SPIM_InternalRxInterruptEnabled */
}


/*******************************************************************************
* Function Name: SPIM_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the 
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPIM_SetTxInterruptMode(uint8 intSrc) 
{
    SPIM_TX_STATUS_MASK_REG  = intSrc;    
}


/*******************************************************************************
* Function Name: SPIM_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the 
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPIM_SetRxInterruptMode(uint8 intSrc) 
{
    SPIM_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: SPIM_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  SPIM_swStatusTx - used to store in software status register, 
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPIM_ReadTxStatus(void) 
{
    uint8 tmpStatus = 0u;
        
    #if (SPIM_TXBUFFERSIZE > 4u)
    
        SPIM_DisableTxInt();
        
        tmpStatus = SPIM_GET_STATUS_TX(SPIM_swStatusTx);                    
        
        SPIM_swStatusTx = 0u;        
        
        /* Enable Interrupts */
        SPIM_EnableTxInt();
        
    #else /* (SPIM_TXBUFFERSIZE < 4u) */
    
        tmpStatus = SPIM_TX_STATUS_REG;
        
    #endif /* (SPIM_TXBUFFERSIZE > 4u) */
    
    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SPIM_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  SPIM_swStatusRx - used to store in software Rx status register, 
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error 
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPIM_ReadRxStatus(void) 
{
    uint8 tmpStatus = 0u;
        
    #if (SPIM_RXBUFFERSIZE > 4u)
    
        SPIM_DisableRxInt();
        
        tmpStatus = SPIM_GET_STATUS_RX(SPIM_swStatusRx);
               
        SPIM_swStatusRx = 0u;
        
        /* Enable Interrupts */
        SPIM_EnableRxInt();
        
    #else /* (SPIM_RXBUFFERSIZE < 4u) */
    
        tmpStatus = SPIM_RX_STATUS_REG;
        
    #endif /* (SPIM_RXBUFFERSIZE > 4u) */
    
    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SPIM_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  SPIM_TXBUFFER[SPIM_TXBUFFERSIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_WriteTxData(uint8 txData) 
{    
    #if(SPIM_TXBUFFERSIZE > 4u)

        int16 tmpTxBufferRead = 0u;
                
        /* Block if buffer is full, so we don't overwrite. */
        do
        {
            tmpTxBufferRead = SPIM_txBufferRead - 1u;
            if (tmpTxBufferRead < 0u)
            {
                tmpTxBufferRead = SPIM_TXBUFFERSIZE - 1u;        
            }    
        } while(tmpTxBufferRead == SPIM_txBufferWrite);               
                   
        /* Disable Interrupt to protect variables that could change on interrupt. */
        SPIM_DisableTxInt();
        
        SPIM_swStatusTx = SPIM_GET_STATUS_TX(SPIM_swStatusTx);
        
        if((SPIM_txBufferRead == SPIM_txBufferWrite) &&
            ((SPIM_swStatusTx & SPIM_STS_TX_FIFO_NOT_FULL) != 0u))
        {
            /* Add directly to the FIFO. */
            CY_SET_REG8(SPIM_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the software buffer. */
            SPIM_txBufferWrite++;
            if(SPIM_txBufferWrite >= SPIM_TXBUFFERSIZE)
            {
                SPIM_txBufferWrite = 0u;
            }   
                      
            if(SPIM_txBufferWrite == SPIM_txBufferRead)
            {
                SPIM_txBufferRead++;
                if(SPIM_txBufferRead >= SPIM_RXBUFFERSIZE)
                {
                    SPIM_txBufferRead = 0u;
                }
                SPIM_txBufferFull = 1u;
            }
            
            SPIM_TXBUFFER[SPIM_txBufferWrite] = txData;
            
            SPIM_TX_STATUS_MASK_REG |= SPIM_STS_TX_FIFO_NOT_FULL;            
        }                         
        
        /* Enable Interrupt. */
        SPIM_EnableTxInt();                        

    #else /* SPIM_TXBUFFERSIZE <= 4u */

        /* Block while FIFO is full */
        while((SPIM_TX_STATUS_REG & SPIM_STS_TX_FIFO_NOT_FULL) == 0u);
        
        /* Then write the byte */
        CY_SET_REG8(SPIM_TXDATA_PTR, txData);

    #endif /* SPIM_TXBUFFERSIZE > 4u */
}


/*******************************************************************************
* Function Name: SPIM_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  SPIM_RXBUFFER[SPIM_RXBUFFERSIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call 
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call 
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPIM_ReadRxData(void) 
{
    uint8 rxData = 0u;

    #if(SPIM_RXBUFFERSIZE > 4u)
    
        /* Disable Interrupt to protect variables that could change on interrupt. */
        SPIM_DisableRxInt();
        
        if(SPIM_rxBufferRead != SPIM_rxBufferWrite)
        {      
            if(SPIM_rxBufferFull == 0u)
            {
                SPIM_rxBufferRead++;
                if(SPIM_rxBufferRead >= SPIM_RXBUFFERSIZE)
                {
                    SPIM_rxBufferRead = 0u;
                }
            }
            else
            {
                SPIM_rxBufferFull = 0u;
            }
        }    
        
        rxData = SPIM_RXBUFFER[SPIM_rxBufferRead];
                           
        /* Enable Interrupt. */
        SPIM_EnableRxInt();
    
    #else /* SPIM_RXBUFFERSIZE <= 4u */
    
        rxData = CY_GET_REG8(SPIM_RXDATA_PTR);
    
    #endif /* SPIM_RXBUFFERSIZE > 4u */

	return (rxData);
    
}


/*******************************************************************************
* Function Name: SPIM_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if 
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 SPIM_GetRxBufferSize(void) 
{
    uint8 size = 0u;

    #if(SPIM_RXBUFFERSIZE > 4u)
    
        /* Disable Interrupt to protect variables that could change on interrupt. */
        SPIM_DisableRxInt();
    
        if(SPIM_rxBufferRead == SPIM_rxBufferWrite)
        {
            size = 0u; /* No data in RX buffer */
        }
        else if(SPIM_rxBufferRead < SPIM_rxBufferWrite)
        {
            size = (SPIM_rxBufferWrite - SPIM_rxBufferRead);
        }
        else
        {
            size = (SPIM_RXBUFFERSIZE - SPIM_rxBufferRead) + SPIM_rxBufferWrite;
        }
    
        /* Enable interrupt. */
        SPIM_EnableRxInt();
    
    #else /* SPIM_RXBUFFERSIZE <= 4u */
    
        /* We can only know if there is data in the fifo. */
        size = ((SPIM_RX_STATUS_REG & SPIM_STS_RX_FIFO_NOT_EMPTY) == 
                 SPIM_STS_RX_FIFO_NOT_EMPTY) ? 1u : 0u;
    
    #endif /* SPIM_RXBUFFERSIZE < 4u */

    return (size);
}


/*******************************************************************************
* Function Name: SPIM_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if 
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  SPIM_GetTxBufferSize(void) 
{
    uint8 size = 0u;

    #if(SPIM_TXBUFFERSIZE > 4u)
    
        /* Disable Interrupt to protect variables that could change on interrupt. */
        SPIM_DisableTxInt();
    
        if(SPIM_txBufferRead == SPIM_txBufferWrite)
        {
            size = 0u;
        }
        else if(SPIM_txBufferRead < SPIM_txBufferWrite)
        {
            size = (SPIM_txBufferWrite - SPIM_txBufferRead);
        }
        else
        {
            size = (SPIM_TXBUFFERSIZE - SPIM_txBufferRead) + SPIM_txBufferWrite;
        }
    
        /* Enable Interrupt. */
        SPIM_EnableTxInt();
    
    #else /* SPIM_TXBUFFERSIZE <= 4u */
    
        size = SPIM_TX_STATUS_REG;
    
        /* Is the fifo is full. */
        if((size & SPIM_STS_TX_FIFO_EMPTY) == SPIM_STS_TX_FIFO_EMPTY)
        {
            size = 0u;
        }
        else if((size & SPIM_STS_TX_FIFO_NOT_FULL) == SPIM_STS_TX_FIFO_NOT_FULL)
        {
            size = 1u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 4u;
        }
    
    #endif /* SPIM_TXBUFFERSIZE > 4u */

    return (size);
}


/*******************************************************************************
* Function Name: SPIM_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function 
*  call - resets to zero.
*  SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to 
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_ClearRxBuffer(void) 
{
	/* Clear Hardware RX FIFO */
    while((!(SPIM_RX_STATUS_REG & SPIM_STS_RX_FIFO_NOT_EMPTY)) == 0u)
    {
        CY_GET_REG8(SPIM_RXDATA_PTR);
    }
	
    #if(SPIM_RXBUFFERSIZE > 4u)
    
        /* Disable interrupt to protect variables that could change on interrupt. */        
        SPIM_DisableRxInt();
    
        SPIM_rxBufferRead = 0u;
        SPIM_rxBufferWrite = 0u;
    
        /* Enable Rx interrupt. */
        SPIM_EnableRxInt();
        
    #endif /* SPIM_RXBUFFERSIZE > 4u */
}


/*******************************************************************************
* Function Name: SPIM_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to 
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when 
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_ClearTxBuffer(void) 
{
    uint8 enableInterrupts = 0u;
    
    /* Clear Hardware TX FIFO */       
    enableInterrupts = CyEnterCriticalSection();
    
    #if(SPIM_DataWidth <= 8u)
    
        /* Clear TX FIFO */
        SPIM_AUX_CONTROL_DP0_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP0_REG &= ~SPIM_FIFO_CLR;
    
    #else
    
        /* Clear TX FIFO */
        SPIM_AUX_CONTROL_DP0_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP0_REG &= ~SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP1_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP1_REG &= ~SPIM_FIFO_CLR;
        
    #endif /* SPIM_DataWidth > 8u */
    
    CyExitCriticalSection(enableInterrupts);
	
    #if(SPIM_TXBUFFERSIZE > 4u)
    
        /* Disable Interrupt to protect variables that could change on interrupt. */
        SPIM_DisableTxInt();
    
        SPIM_txBufferRead = 0u;
        SPIM_txBufferWrite = 0u;
    
        /* If Buffer is empty then disable TX FIFO status interrupt */
        SPIM_TX_STATUS_MASK_REG &= ~SPIM_STS_TX_FIFO_NOT_FULL;

        /* Enable Interrupt. */
        SPIM_EnableTxInt();
    
    #endif /* SPIM_TXBUFFERSIZE > 4u */
}


#if (SPIM_BidirectionalMode == 1u)

    /*******************************************************************************
    * Function Name: SPIM_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SPIM_TxEnable(void) 
    {
        SPIM_CONTROL_REG |= SPIM_CTRL_TX_SIGNAL_EN;
    }
    
    
    /*******************************************************************************
    * Function Name: SPIM_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SPIM_TxDisable(void) 
    {
        SPIM_CONTROL_REG &= ~SPIM_CTRL_TX_SIGNAL_EN;
    }
    
#endif /* SPIM_BidirectionalMode == 1u */


/*******************************************************************************
* Function Name: SPIM_PutArray
********************************************************************************                       
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available 
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_PutArray(uint8 *buffer, uint8 byteCount) 
{
    while(byteCount > 0u)
    {
        SPIM_WriteTxData(*buffer++);
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: SPIM_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void SPIM_ClearFIFO(void) 
{
    uint8 enableInterrupts = 0u;
    
    while((!(SPIM_RX_STATUS_REG & SPIM_STS_RX_FIFO_NOT_EMPTY)) == 0u)
    {
        CY_GET_REG8(SPIM_RXDATA_PTR);
    }
    
    enableInterrupts = CyEnterCriticalSection();
    
    #if(SPIM_DataWidth <= 8u)
    
        /* Clear TX FIFO */
        SPIM_AUX_CONTROL_DP0_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP0_REG &= ~SPIM_FIFO_CLR;
    
    #else
    
        /* Clear TX FIFO */
        SPIM_AUX_CONTROL_DP0_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP0_REG &= ~SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP1_REG |= SPIM_FIFO_CLR;
        SPIM_AUX_CONTROL_DP1_REG &= ~SPIM_FIFO_CLR;
        
    #endif /* SPIM_DataWidth > 8u */
    
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/

/*******************************************************************************
* Function Name: SPIM_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_EnableInt(void) 
{       
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntEnable(SPIM_TX_ISR_NUMBER);
    #endif /* SPIM_InternalTxInterruptEnabled */                                
    
    #if(SPIM_InternalRxInterruptEnabled)           
        CyIntEnable(SPIM_RX_ISR_NUMBER);
    #endif /* SPIM_InternalRxInterruptEnabled */     
}


/*******************************************************************************
* Function Name: SPIM_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPIM_DisableInt(void) 
{
    #if(SPIM_InternalTxInterruptEnabled)    
        CyIntDisable(SPIM_TX_ISR_NUMBER);
    #endif /* SPIM_InternalTxInterruptEnabled */
    
    #if(SPIM_InternalRxInterruptEnabled)           
        CyIntDisable(SPIM_RX_ISR_NUMBER);
    #endif /* SPIM_InternalRxInterruptEnabled */
}


/*******************************************************************************
* Function Name: SPIM_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the 
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPIM_SetInterruptMode(uint8 intSrc) 
{
    SPIM_TX_STATUS_MASK_REG  = intSrc & ~(1u << SPIM_STS_SPI_IDLE_SHIFT);
    SPIM_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: SPIM_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  SPIM_swStatus - used to store in software status register, 
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPIM_ReadStatus(void) 
{
    uint8 tmpStatus;
        
    #if ((SPIM_TXBUFFERSIZE > 4u) || (SPIM_RXBUFFERSIZE > 4u))
    
        SPIM_DisableInt();
        
        tmpStatus = (SPIM_GET_STATUS_TX(SPIM_swStatusTx) & 
                      ~(1u << SPIM_STS_SPI_IDLE_SHIFT)) | 
                      SPIM_GET_STATUS_RX(SPIM_swStatusRx);
        
        SPIM_swStatusTx = 0u;
        SPIM_swStatusRx = 0u;
        
        /* Enable Interrupts */
        SPIM_EnableInt();
        
    #else /* (SPIM_TXBUFFERSIZE < 4u) && (SPIM_RXBUFFERSIZE < 4u) */
    
        tmpStatus = (SPIM_TX_STATUS_REG & ~(1u << SPIM_STS_SPI_IDLE_SHIFT)) |
                     SPIM_RX_STATUS_REG;
        
    #endif /* (SPIM_TXBUFFERSIZE > 4u) || (SPIM_RXBUFFERSIZE > 4u) */
    
    return(tmpStatus);
}


/* [] END OF FILE */
