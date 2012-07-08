/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Ping Handling functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive a ping
               reply and request respectively,are contained here.
*/
#include "IPStackMain.h"

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
unsigned int PingReply(ICMPhdr* ping,unsigned int len){

    if ( ping->type == ICMPREQUEST ){
   /*Yes,it is a Request,lets reply.*/
   /*Setup the packet as a reply*/
    ping->type = ICMPREPLY;
    ping->chksum = 0x0;
    ping->ip.chksum = 0x0;
    
    /*Swap the MAC Addresses in the ETH header*/
    memcpy( ping->ip.eth.DestAddrs, ping->ip.eth.SrcAddrs, 6);
    memcpy( ping->ip.eth.SrcAddrs, deviceMAC,6 );
    
    /*Swap the IP Addresses in the IP header*/
    memcpy( ping->ip.dest, ping->ip.source,4);
    memcpy( ping->ip.source, deviceIP,4);
    
    /*Compute the checksums*/
    ping->chksum=checksum(((unsigned char*) ping) + sizeof(IPhdr ),len - sizeof(IPhdr),0);
    ping->ip.chksum = checksum(((unsigned char*) ping) + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    
    /*Send it!*/
    return(MACWrite((unsigned char*) ping, len));
  }
  return FALSE;
}

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
unsigned int SendPing( unsigned char* targetIP ){
    unsigned int i;
    
    /*declare an ICMP header for our ping request packet*/
    ICMPhdr ping;
    
    /*Setup the IP header part of it*/
    SetupBasicIPPacket( (unsigned char*)&ping, ICMPPROTOCOL, targetIP );
    
    /*Setup the Ping flags*/
    ping.ip.flags = 0x0;
    ping.type = 0x8;
    ping.codex = 0x0;
    ping.chksum = 0x0;
    ping.iden = (0x1);
    ping.seqNum = (76);
    
    /*Fill in the dummy data*/
    for(i=0;i<18;i++){
        *((unsigned char*)&ping+sizeof(ICMPhdr)+i)='A'+i;
    }
    /*Write the length field*/
    ping.ip.len = (60-sizeof(EtherNetII));
    
    /*Compute the checksums*/
    ping.chksum=checksum(((unsigned char*)&ping) + sizeof(IPhdr ),(sizeof(ICMPhdr) - sizeof(IPhdr))+18,0);
    ping.ip.chksum = checksum(((unsigned char*)&ping) + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    
    /*Send it!*/
    return(MACWrite( (unsigned char*)&ping, sizeof(ICMPhdr)+18 ));  
}


/* [] END OF FILE */
