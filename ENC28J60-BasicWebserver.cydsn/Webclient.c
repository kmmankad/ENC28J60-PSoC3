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


unsigned int WebClient_ProcessReply(TCPhdr* Rpacket){

return 1;
}

unsigned int WebClient_BrowseURL(unsigned char* query,TCPhdr* Tpacket){

    unsigned int datlen;
    
    /*We have finished the handshake,so lets send the Actual Query*/
    Tpacket->PSH=1;
    Tpacket->ip.ident=0xABBA;
    
    /*Length of Query String*/
    datlen=strlen(query);
    
    /*Copy in the Query*/
    memcpy((unsigned char*)Tpacket+sizeof(TCPhdr),query,datlen);
    
    /*Set IP Length field*/
    Tpacket->ip.len=(sizeof(TCPhdr)+datlen)-sizeof(EtherNetII);
    
    /*Zero out the checksums*/
    Tpacket->ip.chksum=0x00;
    Tpacket->chksum=0x00;
    
    /*Compute the checksums*/
    Tpacket->ip.chksum=checksum((unsigned char*)Tpacket + sizeof(EtherNetII),sizeof(IPhdr)-sizeof(EtherNetII),0);
    Tpacket->chksum = checksum((unsigned char*)Tpacket->ip.source,0x08+0x14+datlen,2);
    
    return(MACWrite((unsigned char*)Tpacket,sizeof(TCPhdr)+datlen));
}


unsigned int WebClient_Send(const char* Querydat){
    unsigned char packet[MAXPACKETLEN];
    static bit FuncCall;
    
    /*Its a TCP Packet.*/
    TCPhdr* TCPacket = (TCPhdr*)packet;
    
    /*Check if Link is Up*/
    if(IsLinkUp()==0){
        return FALSE;
    }
    
    /*Till Previous Query wasnt fully processed,
    dont send another one,if so just return*/
    if(!((WebClientDataRecd!=2)&&(FuncCall==0))){
        return FALSE;
    }
   
    /*Take the URL into the Global URL*/
    if(sizeof(Querydat)<sizeof(WebClientQuery))  {
        /*We can accomodate it into our global query dat buffer*/
        memcpy(WebClientQuery,Querydat,sizeof(Querydat));
    }else{
        /*Its too big.*/
        return FALSE;
    }
    
    
    /*We'll send a SYN to initiate the Handshake.*/
    SetupBasicIPPacket( packet, TCPPROTOCOL, serverIP );
    
    TCPacket->sourcePort=WClientPort;
    TCPacket->destPort=80;
    
   // TCPacket->seqNo=0x00;
   // TCPacket->ackNo=0x00;
    
    TCPacket->hdrLen = (sizeof(TCPhdr)-sizeof(IPhdr))/4;
    TCPacket->SYN=1;
    
    /*Set Window size*/
    TCPacket->wndSize = 200;
    
    /*Set IP Length*/
    TCPacket->ip.len =sizeof(TCPhdr)-sizeof(EtherNetII);
    
    /*Compute the checksums*/
    TCPacket->ip.chksum=checksum((unsigned char*)TCPacket + sizeof(EtherNetII),sizeof(IPhdr)-sizeof(EtherNetII),0);
    TCPacket->chksum = checksum((unsigned char*)TCPacket->ip.source,0x08+0x14,2);
    
    /*Send the SYN*/
    MACWrite((unsigned char*)TCPacket,sizeof(TCPhdr));
    
    FuncCall=1;
    
    return TRUE;
}

/* [] END OF FILE */
