/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ARP Packet Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines the functions required for
               ARP Packet handling.
*/

#ifndef ARP_H
#define ARP_H

#include "IPStackMain.h"

/*******************************************************************************
* Function Name: SendArpRequest
********************************************************************************
* Summary:
*   Sends an ARP Request for the IP Address specified by targetIP.
*
* Parameters:
*   targetIP - An array that holds the IP,whose hardware address is required.
*              example: unsigned char routerIP={192,168,1,10};
*
* Returns:
*   TRUE(0)- if the request was successfully transmitted.
*   FALSE(1) - if the request was not successfully transmitted.
*******************************************************************************/
unsigned int SendArpRequest(unsigned char* targetIP);

/*******************************************************************************
* Function Name: ReplyArpRequest
********************************************************************************
* Summary:
*   Replies to an ARP Request made to the ENC28J60's IP Address.
*   This function creates an ARP Answer from the ARP request packet recd.
*   This function must be called with an appropriate pointer to the same.
*
* Parameters:
*   arpPacket - A pointer to the received ARP Request packet.
*             
* Returns:
*   TRUE(0)- if the reply was successfully transmitted.
*   FALSE(1) - if the reply was not successfully transmitted.
*******************************************************************************/
unsigned int ReplyArpRequest(ARP* arpPacket);
 
#endif
 
/* [] END OF FILE */
