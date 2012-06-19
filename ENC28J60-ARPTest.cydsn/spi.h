/* Basic SPI Driver Code for PSoC3 SPIM
 Author : Kartik Mankad
 Date : 12-05-11
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the commonly used functions
               to deal with SPI devices,namely reading and writing of data.
 Changelog:
 6-05-11 : Added the Wait for Existing Data to go out in spiTxByte.
 18-6-12 : Added Buffer Read-Write functions and changed function
           names to make them more meaningful.            
*/
#include <device.h>
#define DummyByte 0x00 //0x00 or 0xFF.

//Expects the Slave Select Pin to be called "SS".Hence the macro.
#define SPI_SEL(x)  SS_Write(x)

//Function defines.

/*******************************************************************************
* Function Name: spiInit
********************************************************************************
* Summary:
*   Starts the SPI Master Module
*
* Parameters:
*   none.
*
* Returns:
*   nothing.
*
*******************************************************************************/
void spiInit();

/*******************************************************************************
* Function Name: spiRxByte
********************************************************************************
* Summary:
*   Receives a byte of data from the active SPI slave device.
*
* Parameters:
*   none.
*
* Returns:
*   The byte recd. over SPI from the slave.
*   Note: You need to control the CS line externally.
*         This function assumes CS is active.
*
*******************************************************************************/
uint8 spiRxByte();

/*******************************************************************************
* Function Name: spiTxByte
********************************************************************************
* Summary:
*   Transmits a byte of data over SPI to the active slave device.
*
* Parameters:
*   bDataSend - the byte that is to be transmitted.
*
* Returns:
*   The byte recd. over SPI from the slave as an echo.
*   Note: You need to control the CS line externally.
*         This function assumes CS is active.
*
*******************************************************************************/
uint8 spiTxByte(uint8 bDataSend);

/*******************************************************************************
* Function Name: spiTxBuffer
********************************************************************************
* Summary:
*   Transmits an array of bytes of data over SPI to the active slave device.
*
* Parameters:
*   ptrBuffer - A pointer to the buffer which has to be transmitted.
*   Len - The number of bytes from the buffer passed,which have to be
*         transmitted.
*
* Returns:
*   The number of bytes actually transmitted.
*
*******************************************************************************/
unsigned int spiTxBuffer(unsigned char * ptrBuffer, unsigned int Len);

/*******************************************************************************
* Function Name: spiRxBuffer
********************************************************************************
* Summary:
*   Receives a number of bytes of data over SPI from the active slave device.
*
* Parameters:
*   ptrBuffer - A pointer to the buffer which has to hold incoming data.
*   Len - The number of bytes which have to be received.
*
* Returns:
*   The number of bytes actually received.
*
*******************************************************************************/
unsigned int spiRxBuffer(unsigned char * ptrBuffer, unsigned int Len);

//Functions:

void spiInit(){//Start the SPIM Module
	SPIM_Start();
}


uint8 spiTxByte(uint8 bDataSend)//Send a Byte.
{	
	while (!( SPIM_ReadTxStatus() & SPIM_STS_TX_FIFO_EMPTY )); // Wait for existing data to go out.
	SPIM_WriteTxData(bDataSend); //Write the Byte onto the TX FIFO
	while (!( SPIM_ReadTxStatus() & SPIM_STS_BYTE_COMPLETE  ));//Wait till completely sent.
 	while (!( SPIM_ReadRxStatus() & SPIM_STS_RX_FIFO_NOT_EMPTY ));//Wait for a byte to come in
	return(SPIM_ReadRxData());
}

uint8 spiRxByte(){//Receive a Byte.
	CyDelay(1);
	return(spiTxByte(DummyByte));
}

unsigned int spiTxBuffer(unsigned char * ptrBuffer, unsigned int Len)
{
    unsigned int i;

    if (Len == 0) // no data no send
        return 0;
    for (i=0;i<Len;i++){
        spiTxByte(*ptrBuffer++);
    }
    
    return i;
}

unsigned int spiRxBuffer(unsigned char * ptrBuffer, unsigned int Len)
{
    unsigned int i;
    
    for ( i=0;i<Len;i++){
        *ptrBuffer++ = spiRxByte();
    }
    
    return i;
}