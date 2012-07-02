/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ARP Packet Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file contains the functions required for
               ARP Packet handling.
*/

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
unsigned int SendArpRequest(unsigned char* targetIP){

    /*Lets declare a packet of structure type ARP*/
    ARP arpPacket;
    
    /*----Setup EtherNetII Header----*/
    /*The source of the packet will be the ENC28J60 mac address.*/
    memcpy( arpPacket.eth.SrcAddrs, deviceMAC,6 );
    
    /*The destination is broadcast,so thats a MAC Address of 
    0xFF:0xFF:0xFF:0xFF:0xFF:0xFF:0xFF */
    memset( arpPacket.eth.DestAddrs,0xFF, 6 );
    
    /*The type of packet being sent is an ARP*/
    arpPacket.eth.type = (ARPPACKET);
  
    /*----Setup ARP Header----*/
    arpPacket.hardware = (ETHERNET);
    
    /*We want an IP address resolved.*/
    arpPacket.protocol = (IPPACKET);
    arpPacket.hardwareSize = 0x06;
    arpPacket.protocolSize = 0x04;
    arpPacket.opCode = (ARPREQUEST);
  
    /*Target MAC is set to 0 as it is unknown.*/
    memset( arpPacket.targetMAC, 0, 6 );
    
    /*Sender MAC is the ENC28J60's MAC address.*/
    memcpy( arpPacket.senderMAC, deviceMAC, 6 );
    
    /*The target IP is the IP address we want resolved.*/
    memcpy( arpPacket.targetIP, targetIP, 4);
    
    /*Sender IP will be the device IP*/
    memcpy( arpPacket.senderIP, deviceIP, 4);
    
    /*Send the Packet*/
    return(MACWrite((unsigned char*)&arpPacket,sizeof(ARP)));
}

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
unsigned int ReplyArpRequest(ARP* arpPacket){
    /*
    Check if the ARP Request is directed at us,
    by checking the destination IP in the eth header.
    */
    
    if( !memcmp( arpPacket->targetIP, deviceIP, sizeof(deviceIP) ) ){
        /*Yes,the ARP Request is indeed directed at our device*/
    
        /*Swap the MAC Addresses in the ETH header*/
        memcpy( arpPacket->eth.DestAddrs, arpPacket->eth.SrcAddrs, sizeof(deviceMAC) );
        memcpy( arpPacket->eth.SrcAddrs, deviceMAC, sizeof(deviceMAC) );
        
        /*Swap the MAC Addresses in the ARP packet*/
        memcpy( arpPacket->targetMAC, arpPacket->senderMAC, sizeof(deviceMAC) );
        memcpy( arpPacket->senderMAC, deviceMAC, sizeof(deviceMAC) );
        
        /*Swap the IP Addresses in the ARP packet*/
        memcpy( arpPacket->targetIP, arpPacket->senderIP, sizeof(deviceIP) );
        memcpy( arpPacket->senderIP, deviceIP, sizeof(deviceIP));
        
        /*Set the opCode for an ARP Reply*/
        arpPacket->opCode = (ARPREPLY); 
        
        /*Send the Packet!*/
        return(MACWrite((unsigned char*) arpPacket, sizeof(ARP)));
    }else{
        /*The ARP Request is not for us,so we did nothing.*/
        return FALSE;
  }
  return FALSE;
}

/* [] END OF FILE */
