/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : UDP Transceiver functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive generic UDP packets.
 
*/
#include "IPStackMain.h"

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
unsigned int UDPReply(UDPPacket* udppkt,unsigned char* datapayload,unsigned int payloadlen){
    
    uint16 port;
    
    /*Swap the MAC Addresses in the ETH header*/
    memcpy( udppkt->udp.ip.eth.DestAddrs, udppkt->udp.ip.eth.SrcAddrs, 6);
    memcpy( udppkt->udp.ip.eth.SrcAddrs, deviceMAC,6 );
    
    /*Swap the IP Addresses in the IP header*/
    memcpy( udppkt->udp.ip.dest, udppkt->udp.ip.source,4);
    memcpy( udppkt->udp.ip.source, deviceIP,4);
    
    /*Swap the ports*/
    port=udppkt->udp.sourcePort;
    udppkt->udp.sourcePort=udppkt->udp.destPort;
    udppkt->udp.destPort=port;
    
    /*zero the checksums*/
    udppkt->udp.chksum=0x00;
    udppkt->udp.ip.chksum=0x00;
    
    /*write in the correct lengths*/
    udppkt->udp.len=(sizeof(UDPhdr)-sizeof(IPhdr))+payloadlen;
    udppkt->udp.ip.len=(sizeof(UDPhdr)+payloadlen)-sizeof(EtherNetII);
    
    /*clear the old payload*/
    memset(udppkt->Payload,0x00, (udppkt->udp.len)-8 );
    
    /*copy in the payload*/
    memcpy(udppkt->Payload,datapayload,payloadlen);
    
    /*do the checksums.*/
    udppkt->udp.ip.chksum=checksum(((unsigned char*) udppkt) + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    udppkt->udp.chksum=checksum((unsigned char*)udppkt->udp.ip.source,16+payloadlen,1);
    
    /*Send the packet.*/
    return(MACWrite((unsigned char*)udppkt, sizeof(UDPhdr)+payloadlen));
}

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
unsigned int UDPSend(unsigned char* targetIP,unsigned int targetPort,unsigned char* datapayload,unsigned int payloadlen){
    UDPPacket udppkt;
    
    /*Setup the IP part*/
    SetupBasicIPPacket( (unsigned char*)&udppkt, UDPPROTOCOL, targetIP );
    udppkt.udp.ip.flags = 0x0;
    
    /*Setup the ports*/
    udppkt.udp.sourcePort=UDPPort;
    udppkt.udp.destPort=targetPort;
    
    /*Zero the checksums*/
    udppkt.udp.chksum=0x00;
    udppkt.udp.ip.chksum=0x00;
    
    /*Write in the correct lengths*/
    udppkt.udp.len=(sizeof(UDPhdr)-sizeof(IPhdr))+payloadlen;
    udppkt.udp.ip.len=(sizeof(UDPhdr)+payloadlen)-sizeof(EtherNetII);
    
    /*Copy in the payload*/
    memcpy(udppkt.Payload,datapayload,payloadlen);
    
    /*Do the checksums.*/
    udppkt.udp.ip.chksum=checksum((unsigned char*)&udppkt + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    udppkt.udp.chksum=checksum((unsigned char*)&udppkt.udp.ip.source,16+payloadlen,1);
    
    /*Send the packet!*/
    return(MACWrite((unsigned char*)&udppkt, sizeof(UDPhdr)+payloadlen));
}

/*******************************************************************************
* Function Name: UDP_ProcessIncoming
********************************************************************************
* Summary:
*   This will be called in GetPacket,when a UDP packet is recd.You may process the data
*   carried by that incoming UDP packet in this function,and then call UDPReply to respond.
*
* Parameters:
*   incomingpacket - packet whose data payload is to be processed and replied to.
* Returns:
*   Nothing.
*******************************************************************************/
void UDP_ProcessIncoming(UDPPacket* incomingpacket){
	unsigned char* datapointer=(unsigned char*)incomingpacket+sizeof(UDPhdr);

	if(strncmp(datapointer,"Invoke.",sizeof("Invoke."))==0){
		UDPReply(incomingpacket,"Hello World",sizeof("Hello World"));
	}else{
		UDPReply(incomingpacket,"Access Denied.",sizeof("Access Denied."));
	}

}

/* [] END OF FILE */
