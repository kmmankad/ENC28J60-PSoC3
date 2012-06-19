/* Basic Hardware Test Code for ENC28J60
 Author : Kartik Mankad
 Date : 18-06-2012
 This code is licensed as CC-BY-SA 3.0
 Description : This file tests the connected ENC28J60
               hardware,by 
               1.Turning on both Magjack LEDs.
               2.Reading the Silicon Revision.
               
 This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from www.embeddedmarket.com
  Software: PSoC Creator 2.1 with cy_boot 3.0
  
Based heavily on the work by Duncan Frost for the MSP430.               
*/

#include <device.h>
#include "spi.h"

void main(void){
    unsigned char op = 0xFF; 
    //0xFF is the SPI command for Soft Reset.
       
    //Initialize SPIM Module on the PSoC.
    spiInit(); 
       
    //Start the LCD component.
    LCD_Start();
    LCD_PrintString("SPI Init'd.");
    CyDelay(700);
    LCD_ClearDisplay();
       
    //Send that Soft Reset Command.
    SPI_SEL(0);
        spiTxBuffer(&op,1);
    SPI_SEL(1);
       
    //Wait for the ENC chip to come back.
    CyDelay(1);
    
    LCD_PrintString("ENC Reset.");
    CyDelay(700);
    LCD_ClearDisplay();
       
    /**************************************
    We will now try to turn on the LEDs
    on the Magjack.
    Steps:
    1.Write to the PHLCON register,and turn on the LEDs.
        1.1 Since its a PHY register,Write the address of the PHLCON register to
            into the MIREGADR register.(Which is why we select Bank 2 first
            by writing to ECON1.)
        1.2 Write the lower 8 bits of data to write into the
            MIWRL register.
        1.3 Write the upper 8 bits of data to write into the
            MIWRH register. Writing to this register auto-
            matically begins the MII transaction, so it must
            be written to after MIWRL.
    2.Check the LEDs to see if they light up :-P
    3.Say so on the LCD.
    ************************************************/
       
    //Write Control Register Command for ECON1
    // 010 11111 = 0x5F
    op = 0x5F; 
    SPI_SEL(0);
        spiTxBuffer(&op,1);
            
       //Write 0x02 as we want to select bank 2.            
        op = 0x02; 
        spiTxBuffer(&op,1);
    SPI_SEL(1);
       
    //Write Control Register Command for MIREGADR Register.
    // 010 10100 = 0x54
    op = 0x54;
    SPI_SEL(0);
        spiTxBuffer(&op,1);

        //Select PHLCON Register.
        op = 0x14; 
        spiTxBuffer(&op,1);
    SPI_SEL(1);
       
    //Write Control Register Command for MIWRL Register.
    // 010 10110 = 0x56
    op = 0x56; 
    SPI_SEL(0);
    spiTxBuffer(&op,1);
       
    //Turn on LEDB.
    op = 0x80; 
    spiTxBuffer(&op,1);
    SPI_SEL(1);
    
    LCD_PrintString("LEDB ON.");
    CyDelay(700);
    LCD_ClearDisplay();
    
    //Write Control Register Command for MIWRL Register.
    // 010 10111 = 0x57
    op = 0x57; //Write to MIWRH Reg
    SPI_SEL(0);
    spiTxBuffer(&op,1);
       
    //Turn on LEDA.
    op = 0x38; 
    spiTxBuffer(&op,1);
    SPI_SEL(1);

    LCD_PrintString("LEDA ON.");
    CyDelay(700);
    LCD_ClearDisplay();

    /**************************************
    We will now try to read the Silicon Revision.
    Steps:
    1.Read from EREVID Register
        1.1 Write to ECON 1 to Select Bank 3
        1.2 Read the EREVID Register.
        (Mine gave 0x04)
    **************************************/
        op = 0x5F; //Write to ECON1 Reg
        SPI_SEL(0);
            spiTxBuffer(&op,1);
        op = 0x03; //Write 3 as we want to select bank 3.
            spiTxBuffer(&op,1);
        SPI_SEL(1);
        
        op = 0x12; //Read EREVID Register
        SPI_SEL(0);
            spiTxBuffer(&op,1);
            spiRxBuffer(&op,1);
        SPI_SEL(1);
        
        LCD_PrintString("Silicon Revision");
        LCD_Position(1,0);
        LCD_PrintInt8(op);
    for(;;);
}
