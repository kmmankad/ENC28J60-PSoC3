/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : UDP Transceiver functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive generic UDP packets.
*/
#ifndef UDP_H
#define UDP_H

/*******************************************************************************
* Function Name: UDPReply
********************************************************************************
* Summary:
*   Generate and send a reply to a UDP packet RX'd.
*
* Parameters:
*   udppkt - pointer to the UDP packet recd.
*   datapayload - data to be sent in the UDP Reply.
*   payloadlen - length of the data payload to be sent.
*              
* Returns:
*   TRUE(0)- if the UDP Reply was successfully sent.
*   FALSE(1) - if the UDP Reply was not successful in transmission.
*******************************************************************************/
unsigned int UDPReply(UDPPacket* udppkt,unsigned char* datapayload,unsigned int payloadlen);

/*******************************************************************************
* Function Name: UDPSend
********************************************************************************
* Summary:
*   Generate and send a UDP packet with data.
*   You may edit the UDP Port number(that will be used as the source port) 
*   in "globals.c".Default is 1200.
*
* Parameters:
*   targetIP - IP address to send the UDP packet to.
*   targetPort - Port to direct the UDP packet to.
*   datapayload - data to be sent in the UDP packet.
*   payloadlen - length of the data payload to be sent.
*              
* Returns:
*   TRUE(0)- if the UDP packet was successfully sent.
*   FALSE(1) - if the UDP packet was not successful in transmission.
*******************************************************************************/
unsigned int UDPSend(unsigned char* targetIP,unsigned int targetPort,unsigned char* datapayload,unsigned int payloadlen);

#endif

/* [] END OF FILE */
