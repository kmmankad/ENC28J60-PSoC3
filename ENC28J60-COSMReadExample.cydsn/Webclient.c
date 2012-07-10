/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Webclient functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Webserver client functionality that allows for basic requests/reply processing.
*/
#include "IPStackMain.h"
#include <string.h>
#include <stdlib.h>
/*******************************************************************************
* Function Name: WebClient_ProcessReply
********************************************************************************
* Summary:
*   Process the data sent as a reply from the webserver.
*   **Add all your reply parsing code in this function.**
* Parameters:
*   Rpacket - pointer to a TCP Packet that has the data.
*              
* Returns:
*   customize as required.
*******************************************************************************/
unsigned int WebClient_ProcessReply(TCPhdr* Rpacket){
	/*replydatptr points to the start of the Data in the reply
	Points to H in HTTP 1.1/.... 
	*/
	unsigned char* replydatptr=(unsigned char*)Rpacket+sizeof(TCPhdr);
    unsigned char* temppointer;
    unsigned int currentTemp;
	/*Is it an 'HTTP 200 OK' ? */
	if(strncmp(replydatptr+9,"200",3)==0){
		/*Turn off the status LED to indicate a successful reply*/
		StatusLED_Write(0);
		/*Start scanning for the end of the header(start of data)*/
		temppointer=strstr(replydatptr+9,"\r\n\r\n");
		/*Data is like 2012-07-10T13:39:36.891523Z,32
		so we skip ahead of the date,time and the comma to get our temperature.*/
		/*Note that if youre logging,or using the RTC,this is a good place
		to get Time and Date.*/
		currentTemp=atoi(temppointer+32);
		/*Print it to the LCD*/
		LCD_PrintNumber(currentTemp);
		LCD_PutChar((char)223); /*Print the degree symbol*/
	    LCD_PutChar('C'); /*Print the C*/
		LCD_Position(1,0);
		return TRUE;
	}
	
	return FALSE;
}

/*******************************************************************************
* Function Name: WebClient_Send
********************************************************************************
* Summary:
*   Sends the query stored in the WebClientQuery global variable.
*	(Actually just commands the SYN Packet to initiate the HTTP Request with the server.)
*   Ensure that you have stored the appropriate query in,before calling this.
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the SYN for the Query was successfully signalled for a send.
*   FALSE(1) - if the SYN for the Query was not successful in signalled for a send.
*******************************************************************************/
unsigned int WebClient_Send(){
	
	/*Check if Link is Up*/
    if(IsLinkUp()==0){
        return FALSE;
    }
	
	/*To avoid false re-triggers*/
	if(WebClientStatus>=2){
		return FALSE;
	}
	
	/*Send a SYN*/
    WebClientStatus=1;
	
	/*Allocate a New Port*/
	WClientPort+=1;
	
	/*Send the Query*/
	return TRUE;
}

/*******************************************************************************
* Function Name: WebClient_BrowseURL
********************************************************************************
* Summary:
*   Sends the query stored in the WebClientQuery global variable.
*   Do not call this function in usercode,its part of the state machine in IPStack.C
*	It is called by the state machine after the SYN-SYNACK-ACK handshake is over.
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the Query was successfully sent.
*   FALSE(1) - if the Query was not successful in transmission.
*******************************************************************************/
unsigned int WebClient_BrowseURL(TCPhdr* Tpacket){

    unsigned int datlen;
	
    /*We have finished the handshake,so lets send the Actual Query*/
    Tpacket->PSH=1;
	
	/*Dont Fragment*/
	Tpacket->ip.flags=0x4000;
	Tpacket->ip.ttl=128;

    /*Length of Query String*/
    datlen=strlen(WebClientQuery);
    
    /*Copy in the Query*/
    memcpy((unsigned char*)Tpacket+sizeof(TCPhdr),WebClientQuery,datlen);
    
    /*Set IP Length field*/
    Tpacket->ip.len=(sizeof(TCPhdr)+datlen)-sizeof(EtherNetII);
    
    /*Zero out the checksums*/
    Tpacket->ip.chksum=0x00;
    Tpacket->chksum=0x00;
    
    /*Compute the checksums*/
    Tpacket->ip.chksum=checksum((unsigned char*)Tpacket + sizeof(EtherNetII),sizeof(IPhdr)-sizeof(EtherNetII),0);
    Tpacket->chksum = checksum((unsigned char*)Tpacket->ip.source,0x08+0x14+datlen,2);
	
	/*Send the Query TCP Packet*/
	return(MACWrite((unsigned char*)Tpacket,sizeof(TCPhdr)+datlen));
}

/*******************************************************************************
* Function Name: WebClient_SendSYN
********************************************************************************
* Summary:
*   Sends a SYN packet to the server to initiate the 3 way TCP handshake.
*
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the SYN Packet was successfully sent.
*   FALSE(1) - if the SYN Packet was not successful in transmission.
*******************************************************************************/
unsigned int WebClient_SendSYN(void){
    unsigned char packet[MAXPACKETLEN];
	static unsigned char iden=0x1991;
    unsigned char* optptr=packet+sizeof(TCPhdr);
    /*Its a TCP Packet.*/
    TCPhdr* TCPacket = (TCPhdr*)packet;
    
    /*Check if Link is Up*/
    if(IsLinkUp()==0){
        return FALSE;
    }
    
    /*We'll send a SYN to initiate the Handshake.*/
    SetupBasicIPPacket( packet, TCPPROTOCOL, serverIP );
	
	/*New Identifier*/
	TCPacket->ip.ident=iden++;
	
	/*Source Port to be our Client Port*/
    TCPacket->sourcePort=WClientPort;
	
	/*Its to be sent to the server at port 80*/
    TCPacket->destPort=80;
	
	/*Set Sequence number to be 1*/
    TCPacket->seqNo[0]=0x01;
	
	/*Header length 6(measured in chunks of 4 bytes.) 20 is TCP header length,4 is MSS option*/
    TCPacket->hdrLen = 0x06;
	
	/*Set the SYN Flag*/
    TCPacket->SYN=1;
    
    /*Set Window size*/
    TCPacket->wndSize = 0x320;
    
    /*Set IP Length*/
    TCPacket->ip.len=44;
    
	/*Set Max Segment Size(MSS) Option as 0x200*/
    *(optptr)++ =0x02;
    *(optptr)++ =0x04;
    *(optptr)++ =0x02;
    *(optptr)++ =0x00;
	
    /*Zero out the checksums*/
    TCPacket->ip.chksum=0x00;
    TCPacket->chksum=0x00;
	
    /*Compute the checksums*/
    TCPacket->ip.chksum=checksum((unsigned char*)TCPacket + sizeof(EtherNetII),sizeof(IPhdr)-sizeof(EtherNetII),0);
    TCPacket->chksum = checksum((unsigned char*)TCPacket->ip.source,0x08+0x14+4,2);

    /*Send the SYN*/
    return(MACWrite((unsigned char*)TCPacket,sizeof(TCPhdr)+4));
}

/* [] END OF FILE */
