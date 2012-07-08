/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Ping Handling functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive a ping are defined 
               in this header file.
*/

#ifndef PING_H
#define PING_H

/*******************************************************************************
* Function Name: PingReply
********************************************************************************
* Summary:
*   Generate and send a Ping reply from a received request.
*
* Parameters:
*   ping - A pointer of type ICMPhdr,to the received request's packet.
*   len - length of the recd. Ping request.          
* Returns:
*   TRUE(0)- if the Ping Reply was successfully sent.
*   FALSE(1) - if the Ping Reply was not successful in transmission.
*******************************************************************************/
unsigned int PingReply(ICMPhdr* ping,unsigned int len);

/*******************************************************************************
* Function Name: SendPing
********************************************************************************
* Summary:
*   Generate and send a Ping request to an IP specified by targetIP
*
* Parameters:
*   targetIP - IP Address to which the ping request will be directed.
*   example: unsigned char LabPC_IP[]={192,168,1,15}; then
*               ..
*              SendPing(LabPC_IP);
* Returns:
*   TRUE(0)- if the Ping request was successfully sent.
*   FALSE(1) - if the Ping request was not successful in transmission.
*******************************************************************************/
unsigned int SendPing( unsigned char* targetIP );

#endif

/* [] END OF FILE */
