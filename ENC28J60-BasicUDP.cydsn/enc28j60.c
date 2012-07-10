/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ENC28J60 driver function
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the underlying ENC28J60 driver.
*/

/* 
 Basic ENC28J60 Driver Code for PSoC3
 Author : Kartik Mankad
 Date : 17-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the functions,structures and registers
               essential while dealing with the ENC28J60 chip.
*/
#include "globals.h"
#include "enc28j60.h"
#include "spi.h"


/*
Declare structures of type TXSTATUS and RXSTATUS,which
  will hold the status vectors.
*/
TXSTATUS TxStatus;
RXSTATUS ptrRxStatus;

/*Define the Private Functions*/

static unsigned char ReadETHReg(unsigned char bytAddress);// read an ETH reg
static unsigned char ReadMacReg(unsigned char bytAddress);// read a MAC reg
static unsigned int ReadPhyReg(unsigned char);// read a PHY reg
static unsigned int ReadMacBuffer(unsigned char * ,unsigned int);//read the mac buffer (ptrBuffer, no. of bytes)
static unsigned char WriteCtrReg(unsigned char,unsigned char);// write to a Control reg
static unsigned char WritePhyReg(unsigned char,unsigned int);// write to a Phy reg
static unsigned int WriteMacBuffer(unsigned char *,unsigned int);// write to the MAC buffer
static void ResetMac(void);//Reset the MAC.
static unsigned char SetBitField(unsigned char, unsigned char);//Set Bit Field in the register.
static unsigned char ClrBitField(unsigned char, unsigned char);//Clear Bit Fir
static void BankSel(unsigned char);

/*Macro for Silicon Errata to do with Transmit Logic Reset.
Silicon Errata No.12 as per Latest Errata doc for ENC28J60
See http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf */
#define ERRATAFIX   SetBitField(ECON1, ECON1_TXRST);ClrBitField(ECON1, ECON1_TXRST);ClrBitField(EIR, EIR_TXERIF | EIR_TXIF)


unsigned char ReadChipRev(void){
    BankSel(3);
    return(ReadETHReg(EREVID));
}

void initMAC(unsigned char* deviceMAC){
    /*Initialize the SPI Module*/
    spiInit();        
    
    /*Execute a Soft Reset to the MAC*/
    ResetMac();
    
    /*Setup the 8kb Memory space on the ENC28J60
    by defining ERXST and ERXND*/
    BankSel(0);//Select Bank 0
    WriteCtrReg(ERXSTL,(unsigned char)( RXSTART & 0x00ff));    
    WriteCtrReg(ERXSTH,(unsigned char)((RXSTART & 0xff00)>> 8));
    WriteCtrReg(ERXNDL,(unsigned char)( RXEND   & 0x00ff));
    WriteCtrReg(ERXNDH,(unsigned char)((RXEND   & 0xff00)>>8));

    /*Set RX Read pointer to start of RX Buffer*/
    WriteCtrReg(ERXRDPTL, (unsigned char)( RXSTART & 0x00ff));
    WriteCtrReg(ERXRDPTH, (unsigned char)((RXSTART & 0xff00)>> 8));
	
	/*Setup Transmit Buffer*/
	WriteCtrReg(ETXSTL,(unsigned char)( TXSTART & 0x00ff));//Start of buffer
	WriteCtrReg(ETXSTH,(unsigned char)((TXSTART & 0xff00)>>8));
	/*End of buffer will depend on packets,so no point
	hardcoding it*/

    /*Set the RX Filters*/
    BankSel(1);//Select Bank 1
    /*REGISTER 8-1: ERXFCON: RECEIVE FILTER CONTROL REGISTER
    See Page 50 of the datasheet.*/
    WriteCtrReg(ERXFCON,( ERXFCON_UCEN + ERXFCON_CRCEN + ERXFCON_PMEN));
	
	//---Setup packet filter---
	//This part is taken from Guido Socher's AVR enc28j60 driver.Great Work,that.
	//For broadcast packets we allow only ARP packtets
	//All other packets should be unicast only for our mac (MAADR)
	//The pattern to match on is therefore
	//Type     ETH.DST
	//ARP      BROADCAST
	//06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	//in binary these poitions are:11 0000 0011 1111
	//This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	WriteCtrReg(EPMM0, 0x3f);
	WriteCtrReg(EPMM1, 0x30);
	WriteCtrReg(EPMCSL,0x39);
	WriteCtrReg(EPMCSH,0xf7);


    /*Initialize the MAC Registers*/
    BankSel(2); // select bank 2
    SetBitField(MACON1, MACON1_MARXEN);// Enable reception of frames
    WriteCtrReg(MACON3, MACON3_FRMLNEN +    // Type / len field will be checked
                        MACON3_TXCRCEN +    // MAC will append valid CRC
                        MACON3_PADCFG0);    // All small packets will be padded
     
    WriteCtrReg(MAMXFLL, (unsigned char)( MAXFRAMELEN & 0x00ff));// set max frame len
    WriteCtrReg(MAMXFLH, (unsigned char)((MAXFRAMELEN & 0xff00)>>8));
    WriteCtrReg(MABBIPG, 0x12);// back to back interpacket gap. set as per data sheet
    WriteCtrReg(MAIPGL , 0x12);// non back to back interpacket gap. set as per data sheet
    WriteCtrReg(MAIPGH , 0x0C);

  
    /*Assign the MAC Address to the chip.*/
    BankSel(3);//Select Bank 3.              
    WriteCtrReg(MAADR1,deviceMAC[0]);   
    WriteCtrReg(MAADR2,deviceMAC[1]);  
    WriteCtrReg(MAADR3,deviceMAC[2]);
    WriteCtrReg(MAADR4,deviceMAC[3]);
    WriteCtrReg(MAADR5,deviceMAC[4]);
    WriteCtrReg(MAADR6,deviceMAC[5]);

    /*Initialise the PHY registers
    REGISTER 11-3: PHCON1: PHY CONTROL REGISTER 1
    See Page 65 of the Datasheet.*/
    WritePhyReg(PHCON1, 0x000);
    /*
    "If using half duplex, the host controller may wish to set
    the PHCON2.HDLDIS bit to prevent automatic
    loopback of the data which is transmitted."
    See Section 6.6 on Page 40 */
    WritePhyReg(PHCON2, PHCON2_HDLDIS);
    
    /*Enable reception of packets*/
    WriteCtrReg(ECON1,  ECON1_RXEN);     
}

unsigned char MACWrite(unsigned char* packet, unsigned int len){

    unsigned char  bytControl=0x00;
  
    /*Configure TX Buffer Pointers*/
    BankSel(0);// select bank 0
    
    /*Buffer write ptr to start of Tx packet*/
    WriteCtrReg(ETXSTL,(unsigned char)( TXSTART & 0x00ff));        
    WriteCtrReg(ETXSTH,(unsigned char)((TXSTART & 0xff00)>>8));
    
	/*Set write buffer pointer to point to start of Tx Buffer*/
	WriteCtrReg(EWRPTL,(unsigned char)( TXSTART & 0x00ff));        
	WriteCtrReg(EWRPTH,(unsigned char)((TXSTART & 0xff00)>>8));
	
    
    /*Write the Per Packet Control Byte
    See FIGURE 7-1: FORMAT FOR PER PACKET CONTROL BYTES
    on Page 41 of the datasheet */
    WriteMacBuffer(&bytControl,1);
      
    /*Write the packet into the ENC's buffer*/
	WriteMacBuffer(packet, len);  
    
	/*Tell MAC when the end of the packet is*/
	WriteCtrReg(ETXNDL, (unsigned char)( (len+TXSTART+1) & 0x00ff));       
	WriteCtrReg(ETXNDH, (unsigned char)(((len+TXSTART+1) & 0xff00)>>8));

    /*We would like to enable Interrupts on Packet TX complete.*/
    ClrBitField(EIR,EIR_TXIF);
    SetBitField(EIE, EIE_TXIE |EIE_INTIE);
    
    /*Macro for Silicon Errata to do with Transmit Logic Reset.
    Silicon Errata No.12 as per Latest Errata doc for ENC28J60
    See http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf */
    ERRATAFIX;    
    
    /*Send that Packet!*/
    SetBitField(ECON1, ECON1_TXRTS);
    
    /*Wait for the Chip to finish the TX,and
      read the TX interrrupt bit to check the same.*/
    do{}while (!(ReadETHReg(EIR) & (EIR_TXIF)));             // kill some time. Note: Nice place to block?             // kill some time. Note: Nice place to block?

    /*Clear TXRTS,since the packet has been TX'd.*/
    ClrBitField(ECON1, ECON1_TXRTS);
  
    /*We will now attempt to read the TX Status Vectors.
    See TABLE 7-1: TRANSMIT STATUS VECTORS on Page 43 of the datasheet.*/
    BankSel(0);
    
    /*Because,that control byte.*/
    len++;
    
    /*Configure the buffer read ptr to read status structure*/
    WriteCtrReg(ERDPTL, (unsigned char)( len & 0x00ff));       
    WriteCtrReg(ERDPTH, (unsigned char)((len & 0xff00)>>8));
    
    /*Read In the TX Status Vectors*/
    /*Note: Use these for debugging.Really useful.*/
    ReadMacBuffer(&TxStatus.v[0],7);

    /*Read TX status vectors to see if TX was interrupted.*/
    if (ReadETHReg(ESTAT) & ESTAT_TXABRT){
        if (TxStatus.bits.LateCollision){
            ClrBitField(ECON1, ECON1_TXRTS);//Toggle the TXRTS
            SetBitField(ECON1, ECON1_TXRTS);
            ClrBitField(ESTAT,ESTAT_TXABRT | ESTAT_LATECOL);//Clear the Late Collision Bit.
        }
    ClrBitField(EIR, EIR_TXERIF | EIR_TXIF);//Clear the Interrupt Flags.
    ClrBitField(ESTAT,ESTAT_TXABRT);//Clear the Abort Flag.
    return FALSE;//Report a Failed Packet TX.
  }else{
    return TRUE;//Packet Sent Okay! :-)
  }
   return TRUE;
}

unsigned int MACRead(unsigned char* packet, unsigned int maxLen){
	volatile unsigned int pckLen;
	static unsigned int nextpckptr = RXSTART;
    
    /*Read EPKTCNT to see if we have any packets in.*/
    BankSel(1);//Select Bank 1.
    if(ReadETHReg(EPKTCNT) == 0){
        return 0;//Report that No Proper Packets RX'd.
    }
        
    /*Setup memory pointers to Read in this RX'd packet.*/
    BankSel(0);
    WriteCtrReg(ERDPTL,(unsigned char)( nextpckptr & 0x00ff));
    WriteCtrReg(ERDPTH,(unsigned char)((nextpckptr & 0xff00)>>8));
    
    /*Read in the Next Packet Pointer,and the following 32bit Status Vector.
    See FIGURE 7-3: SAMPLE RECEIVE PACKET LAYOUT on Page 45 of the datasheet.*/
    ReadMacBuffer((unsigned char*)&ptrRxStatus.v[0],6);
    
    /*Because,Little Endian.*/
    nextpckptr = CYSWAP_ENDIAN16(ptrRxStatus.bits.NextPacket);
    
    /*Compute actual length of the RX'd Packet.*/
    pckLen=CYSWAP_ENDIAN16(ptrRxStatus.bits.ByteCount) - 4; //We take away 4 as that is the CRC
    
	if( pckLen > (maxLen-1) ){
	pckLen = maxLen;
	}
	
    /*Read the packet only if it was RX'd Okay.
    We should be checking other flags too,like Length Out of Range,
    but that one doesnt seem reliable.
    We need more work and testing here.*/
    if(ptrRxStatus.bits.RxOk==0x01){
        ReadMacBuffer(packet,pckLen);//Read packet into buffer.
    }

    /*Ensure that ERXRDPT is Always ODD! Else Buffer gets corrupted.
    See No.5 in the Silicon Errata*/                                      
    BankSel(0);
    if ( ((nextpckptr - 1) < RXSTART) || ((nextpckptr-1) > RXEND) ) {
        /*Free up memory in that 8kb buffer by adjusting the RX Read pointer,
        since we are done with the packet.*/
        WriteCtrReg(ERXRDPTL, (RXEND & 0x00ff));
        WriteCtrReg(ERXRDPTH, ((RXEND & 0xff00) >> 8));
    }else{
        WriteCtrReg(ERXRDPTL, (( nextpckptr - 1 ) & 0x00ff ));
        WriteCtrReg(ERXRDPTH, ((( nextpckptr - 1 ) & 0xff00 ) >> 8 ));
    }
  /*To signal that we are done with the packet,decrement EPKTCNT*/
  SetBitField(ECON2, ECON2_PKTDEC);
  
  /*Return the length of the packet RX'd*/
  return pckLen;
}

/*------------------------Private Functions-----------------------------*/

/*******************************************************************************
* Function Name: ReadETHReg
********************************************************************************
* Summary:
*   Reads an ETH Register.Assumes that the correct bank is already selected.
*
* Parameters:
*   bytAddress - Address of the Register to be Read.
*
* Returns:
*   Data in the Register,as an unsigned char.
*******************************************************************************/
static unsigned char ReadETHReg(unsigned char bytAddress){
    /*Define the datahold for incoming Register Data.*/
    unsigned char bytData;

    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
    
    spiTxBuffer(&bytAddress,1);//Write the OpCode
    spiRxBuffer(&bytData, 1);//Read the Data
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
  
  /*Return the data fetched.*/
    return bytData;
}

/*******************************************************************************
* Function Name: ReadMacReg
********************************************************************************
* Summary:
*   Reads a MAC Register.Assumes that the correct bank is already selected.
*
* Parameters:
*   bytAddress - Address of the Register to be Read.
*
* Returns:
*   Data in the Register,as an unsigned char.
*******************************************************************************/
static unsigned char ReadMacReg(unsigned char bytAddress){
    /*Define the datahold for incoming Register Data.*/
    unsigned char bytData;
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
    
    spiTxBuffer(&bytAddress,1);//Write the OpCode.
    spiRxBuffer(&bytData, 1);//Read in the Dummy Byte.
    spiRxBuffer(&bytData, 1);//Read the Actual Value.
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)

    return bytData;
}

/*******************************************************************************
* Function Name: WritePhyReg
********************************************************************************
* Summary:
*   Writes to a PHY Reg,following the Indirect Writing sequence detailed in 
*   Section 3.3.2 on Page 21 of the datasheet.
*
* Parameters:
*   bytAddress - Address of the Register to be Read.
*
* Returns:
*   TRUE  - Write executed.
*   FALSE - Invalid Address.
*******************************************************************************/
static unsigned char WritePhyReg(unsigned char address, unsigned int datapayload){ 
    if (address > 0x14){
        return FALSE;
    }   
    BankSel(2);
    
    /*Write the address of the PHY register we wish to write to.*/
    WriteCtrReg(MIREGADR,address);
    
    /*Write the lower byte of the Payload to write.*/
    WriteCtrReg(MIWRL,(unsigned char)datapayload); 
    
    /*Write the higher byte of the Payload to write.*/
    WriteCtrReg(MIWRH,((unsigned char)(datapayload >>8)));
    
    return TRUE;
}

/*******************************************************************************
* Function Name: ReadPhyReg
********************************************************************************
* Summary:
*   Reads from a PHY Reg,following the indirect reading sequence detailed in 
*   Section 3.3.1 on Page 21 of the datasheet.
*
* Parameters:
*   address - Address of the Register to be Read.
*
* Returns:
*   TRUE  - Write executed.
*   FALSE - Invalid Address.
*******************************************************************************/
static unsigned int ReadPhyReg(unsigned char address){
    volatile unsigned int uiData;
    volatile unsigned char bytStat;

    BankSel(2);
    /*Write into MIREGADR the address of PHY register you want to read.*/
    WriteCtrReg(MIREGADR,address);
    
    /*Set the MIIRD Bit,to request a read.*/
    SetBitField(MICMD, MICMD_MIIRD);
    
    /*Wait and Check if the Read has finished execution*/
    do{bytStat = ReadMacReg(MISTAT);
    }while(bytStat & MISTAT_BUSY);
    
    /*Clear the Read Request bit.*/
    ClrBitField(MICMD,MICMD_MIIRD);
    
    /*Read the low,high data bytes,and assemble them*/
    uiData = (unsigned int)ReadMacReg(MIRDL);       
    uiData |=((unsigned int)ReadMacReg(MIRDH)<<8); // Read high data byte

    return uiData;
}

/*******************************************************************************
* Function Name: WriteCtrReg
********************************************************************************
* Summary:
*   Writes to a control Register.Assumes the correct bank is already selected.
*
* Parameters:
*   bytAddress - Address of the Register to be Read.
*   bytData - The data payload to write.
*
* Returns:
*   Data in the Register,as an unsigned char.
*******************************************************************************/
static unsigned char WriteCtrReg(unsigned char bytAddress,unsigned char bytData){
    
    /*Check Validity of Address*/
    if (bytAddress > 0x1f){
        return FALSE;
    }
    bytAddress |= WCR_OP;//Set the Opcode.
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
    spiTxBuffer(&bytAddress,1);//Send the OpCode and Address.
    spiTxBuffer(&bytData,1);//Send the data payload.
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
  
    return TRUE;
}

/*******************************************************************************
* Function Name: ReadMacBuffer
********************************************************************************
* Summary:
*   Reads the 8kb buffer of the ENC chip.
*
* Parameters:
*   bytBuffer - The buffer to store the read data.
*   byt_length - Number of bytes to read into bytBuffer.
*
* Returns:
*   Number of bytes read.
*******************************************************************************/
static unsigned int ReadMacBuffer(unsigned char * bytBuffer,unsigned int byt_length){
    unsigned char bytOpcode;
    volatile unsigned int len;

    bytOpcode = RBM_OP;//Set the Opcode.
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
    
    spiTxBuffer(&bytOpcode,1);//Send the OpCode.
    len = spiRxBuffer(bytBuffer, byt_length);//Read bytes into the buffer.
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
  
    return len;
}


/*******************************************************************************
* Function Name: WriteMacBuffer
********************************************************************************
* Summary:
*   Writes the 8kb buffer of the ENC chip.Assumes auto increment is ON.
*
* Parameters:
*   bytBuffer - The buffer that stores data to be written.
*   ui_len - Number of bytes to write into bytBuffer.
*
* Returns:
*   Number of bytes written.
*******************************************************************************/
static unsigned int WriteMacBuffer(unsigned char * bytBuffer,unsigned int ui_len){
    unsigned char bytOpcode;
    volatile unsigned int len;

    bytOpcode = WBM_OP;//Set the opcode.
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
      
    spiTxBuffer(&bytOpcode,1);//Send the opcode
    len = spiTxBuffer(bytBuffer, ui_len);//Send the bytes out
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
  
    return len;
}

/*******************************************************************************
* Function Name: SetBitField
********************************************************************************
* Summary:
*   Sets those bits in the register at location bytAddress,which are set in bytData.
*   Assumes that the correct bank is already selected.
* Parameters:
*   bytAddress - Address of the register
*   bytData - appropriate bitmask that needs to be set.
*
* Returns:
*   TRUE - Command executed.
*   FALSE - Invalid Address.
*******************************************************************************/
static unsigned char SetBitField(unsigned char bytAddress, unsigned char bytData){
    
    if (bytAddress > 0x1f){
        return FALSE;
    }

    bytAddress |= BFS_OP;//Set the opcode.
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
 
    spiTxBuffer(&bytAddress,1);//Send the opcode and address.
    spiTxBuffer(&bytData,1);//Send the data.
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
  
    return TRUE;
}

/*******************************************************************************
* Function Name: ClrBitField
********************************************************************************
* Summary:
*   Clears those bits in the register at location bytAddress,which are set in bytData.
*   Assumes that the correct bank is already selected.
* Parameters:
*   bytAddress - Address of the register
*   bytData - appropriate bitmask that needs to be cleared.
*
* Returns:
*   TRUE - Command executed.
*   FALSE - Invalid Address.
*******************************************************************************/
static unsigned char ClrBitField(unsigned char bytAddress, unsigned char bytData){
    
    if (bytAddress > 0x1f){
        return FALSE;
    }

    bytAddress |= BFC_OP;//Set the opcode.
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)

    spiTxBuffer(&bytAddress,1);//Send the opcode and address.
    spiTxBuffer(&bytData,1);//Send the data.
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled Low.)
  
    return TRUE;
}

/*******************************************************************************
* Function Name: BankSel
********************************************************************************
* Summary:
*   Sets the correct bank,for further register manipulation.
*   Note that you should call this before any register manipulation,to set the
*   right bank.
* Parameters:
*   bank - can be either of 0,1,2 or 3,depending on which bank you want to set.
*
* Returns:
*   Nothing.
*******************************************************************************/
static void BankSel(unsigned char bank){
    volatile unsigned char temp;
    if (bank >3)
        return;
        
    temp = ReadETHReg(ECON1);//Read ECON1 Register.
    temp &= ~ECON1_BSEL;//Mask off the BSEL<1:0> bits
    temp |= bank;//Set the new BSEL bits.
    WriteCtrReg(ECON1, temp);//Write the new ECON1 back.
}
/*******************************************************************************
* Function Name: ResetMac
********************************************************************************
* Summary:
*   Sends a Soft Reset command over SPI to the chip.
*   Gives a large delay of 1second before returning.
*   right bank.
* Parameters:
*   none.
* Returns:
*   Nothing.
*******************************************************************************/
static void ResetMac(void){
    unsigned char bytOpcode = RESET_OP;
    
    SPI_SEL(TRUE);//Activate CS(Pulled Low.)
    
    spiTxBuffer(&bytOpcode,1);//Send the Command.
    
    SPI_SEL(FALSE);//Deactivate CS(Pulled High.)
    
    /*Give it 1sec to come out of Reset.*/
    CyDelay(1000);
}


/*******************************************************************************
* Function Name: IsLinkUp
********************************************************************************
* Summary:
*   Returns the status of the PHY link,from PHSTAT2 register.
*   Do not call this before initMAC,since SPIM is started in InitMAC function.
*
* Parameters:
*   none.
*
* Returns:
*   0x01 - If link is up.
*   0x00 - If link is not up.
*
*******************************************************************************/
unsigned char IsLinkUp(void){
    return(unsigned char)(ReadPhyReg(PHSTAT2) && 0x0400);
}