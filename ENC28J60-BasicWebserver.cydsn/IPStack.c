/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Common IP Stack functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the common functions required
               by the remainder of the IP stack.
*/

#include "IPStackMain.h"
#include <string.h>
#include <device.h>

/*******************************************************************************
* Function Name: add32
********************************************************************************
* Summary:
*   Adds a 16 bit operand to a 32bit one.Used here to update 
*   Sequence and Ack numbers during an HTTP transaction.
*
* Parameters:
*   op32 - pointer to the 32bit operand.
*   op16 - the 16bit operand.
*             
* Returns:
*   none.
*******************************************************************************/
void add32(unsigned char *op32, unsigned int op16){
    op32[3] += (op16 & 0xff);
    op32[2] += (op16 >> 8);
  
    if(op32[2] < (op16 >> 8)) {
        ++op32[1];
        if(op32[1] == 0) {
            ++op32[0];
        }
    }
  
    if(op32[3] < (op16 & 0xff)) {
        ++op32[2];
        if(op32[2] == 0) {
            ++op32[1];
            if(op32[1] == 0) {
                ++op32[0];
            }
        }
    }
}

/*******************************************************************************
* Function Name: checksum
********************************************************************************
* Summary:
*   Computes the checksum for UDP,TCP or IP as specified by the type parameter.
*   Sequence and Ack numbers during an HTTP transaction.
*   **Clear the checksum fields in the appropriate packets,before calculating.
*
* Parameters:
*   buf - pointer to the data structure over which the checksum must be calc'd.
*   For ICMP or IP Checksums(type 0),pointer should point to the start of 
*   the IP header.
*   For TCP(type 2)/UDP(type 1),pointer should point to 
*   the sourceIP field in the IPheader.
*
*   len - length of the data structure over which checksum must be calc'd.
*   For ICMP or IP Checksums(type 0),it should be size of ONLY IP header part.
*   That can be computed by sizeof(IPhdr) - sizeof(EtherNetII).
*   For TCP(type 2)/UDP(type 1),it should be UDP or TCP header plus 8,because
*   the source IP and destination IP(which are part of the checksum's pseudoheader)
*   are 4bytes long each.
*   
*   type - The Type of checksum we require,
*           0 for IP/ICMP checksums
*           1 for UDP checksums
*           2 for TCP checksums
*            
* Returns:
*   16 bit checksum.
*   This function is taken from the TUXGraphics network stack,due to its elegance.
*******************************************************************************/
uint16 checksum(unsigned char *buf, unsigned int len,unsigned char type){

    uint32 sum = 0;
    /*Process that pseudoheader nonsense for UDP*/
    if(type==1){
        sum+=UDPPROTOCOL;//Add the protocol field of the pseudoheader to sum.
                //'len' should be the length of the UDP header+data.
                /*Since UDP length field is also part of the pseudoheader,
                that needs to be added too.We compute that UDP length 
                by subtracting 8 from the 'len'(passed to this function),since
                we include 8bytes extra when we called this function,due to the
                sourceIP and destIP,which are to be part of this pseudoheader,
                and hence checksum calculation.*/
        sum+=(len-8);//Add the UDP len.
    }
    
    if(type==2){//Add the protocol field of the pseudoheader to sum.
        sum+=TCPPROTOCOL; 
                //'len' should be the length of the UDP header+data.
                /*Since TCP length field is also part of the pseudoheader,
                that needs to be added too.We compute that TCP length 
                by subtracting 8 from the 'len'(passed to this function),since
                we include 8bytes extra when we called this function,due to the
                sourceIP and destIP,which are to be part of this pseudoheader,
                and hence checksum calculation.*/
        sum+=len-8;//Add the TCP len
    }
        
    /*Sum up 16bit words*/
    while(len >1){
        sum += 0xFFFF & (((uint32)*buf<<8)|*(buf+1));
        buf+=2;
        len-=2;
    }
        
    /*if there is a byte left(number of bytes was odd) then add it,but zero padded.*/
    if (len){
        sum += ((uint32)(0xFF & *buf))<<8;
    }
        
    /*Now calculate the sum over the bytes in the sum
    until the result is only 16bit long*/
    while (sum>>16){
        sum = (sum & 0xFFFF)+(sum >> 16);
    }
    
    /*build 1's complement*/
    return((uint16)(sum ^ 0xFFFF));
}

/*******************************************************************************
* Function Name: SetupBasicIPPacket
********************************************************************************
* Summary:
*   Swaps the Source,Destination MAC and IP Addresses,
*   Populates the various other fields of an IP header,
*   zero-ing the checksum field.It does not set length.
*
* Parameters:
*   packet - pointer to the packet data structure to be populated.
*   proto - value of the protocol field in the IP header(TCP/ICMP/UDP/IP)
*   destIP - IP address to be filled in the destination IP field.
*             
* Returns:
*   none.
*******************************************************************************/
void SetupBasicIPPacket( unsigned char* packet, unsigned char proto, unsigned char* destIP){
    /*Structure the data buffer(packet) as an IP header*/
    IPhdr* ip = (IPhdr*)packet;
    
    /*ETH type is an IP packet*/
    ip->eth.type = (IPPACKET);
    
    /*Set the MAC Addresses in the ETH header*/
    memcpy( ip->eth.DestAddrs, routerMAC, sizeof(routerMAC) );
    memcpy( ip->eth.SrcAddrs, deviceMAC, sizeof(deviceMAC) );
    
    /*Set the IP Addresses in the IP header*/
    memcpy( ip->source , deviceIP, sizeof(deviceIP) );
    memcpy( ip->dest, destIP, sizeof(deviceIP) );
    
    /*Fill up the rest of the fields*/
    ip->version = 0x4;
    ip->hdrlen = 0x5;
    ip->diffsf = 0;
    ip->ident = 2;/*Random*/ 
	ip->flags = 0x4000;
	  	//ip->fragmentOffset1 = 0x00;  	
  	//ip->fragmentOffset2 = 0x00;


    ip->ttl = 128;
    
    /*Set the type of IP Protocol*/
    ip->protocol = proto;
    
    /*Zero out the checksum*/
    ip->chksum = 0x00;
}

/*******************************************************************************
* Function Name: GetPacket
********************************************************************************
* Summary:
*   This function checks for packets recd,and has been designed to
*   automatically reply to Ping Requests and ARP Requests.It also handles WebServer
*	and WebClient state machines.
*   If you wish to process Ping Replies,edit the lines marked for the same.
*
*   It returns '1' if it finds a packet of type proto.(UDP/TCP/ICMP etc)
*
* Parameters:
*   proto -  Protocol type for the packets we want to retrive.
*            use TCPPROTOCOL,UDPPROTOCOL,ICMPPROTOCOL as they have been declared 
*            already in "IPStack.h"
*            with the appropriate values.
*   packet - pointer to a buffer that can store the RX'd packet of type proto.
*             
* Returns:
*   It returns '1' if it finds a packet of type proto.(UDP/TCP/ICMP etc)
*******************************************************************************/
unsigned int GetPacket( int proto, unsigned char* packet ){ 
    unsigned int len;
   
    /*Did we get any packets?*/
    if ( len = MACRead( packet, MAXPACKETLEN ) ){
    
        /*Lets check if its an ARP packet.*/
        EtherNetII* eth = (EtherNetII*)packet;
        
        if ( eth->type == (ARPPACKET) ){
            /*Its an ARP Packet.*/
            ARP* arpPacket = (ARP*)packet;
            if ( arpPacket->opCode == (ARPREQUEST)){
                /*We have recd. an ARP Request,and
                  we should reply.*/
                return(ReplyArpRequest(arpPacket));
            }
        } else if( eth->type == (IPPACKET) ){/*Is it an IP Packet?*/
        /*Its an IP Packet,so we should go ahead and check its
        protocol.(UDP/TCP/ICMP etc.) */
            
            IPhdr* ip = (IPhdr*)packet;
        
            /*PING PACKET HANDLER*/
            if( (ip->protocol == ICMPPROTOCOL) ){
                ICMPhdr* ping = (ICMPhdr*)packet;
                if(ping->type==ICMPREQUEST){
                    /*Someone has pinged us,lets reply.*/
                    return(PingReply((ICMPhdr*)packet, len));
                }else if(ping->type==ICMPREPLY){
                /*We have recd. Ping replies.
                (Did we ping someone?)
                Process them.*/
                /*PING REPLY RECD. PROCESSSING CODE GOES HERE*/
                }
            }
            
			if((ip->protocol==TCPPROTOCOL)){
				TCPhdr* Pack = (TCPhdr*)packet;
			/*<-------------WEBSERVER HANDLER START----------------->*/
				if(Pack->destPort==WWWPort){
					if(Pack->SYN==1){
						/*Its a SYN from a Client.*/
						/*Reply with a SYNACK*/
						return(ackTcp(Pack,(Pack->ip.len)+14,1,0,0,0));
			  		}else if((Pack->PSH==1)&&(Pack->ACK==1)){
						/*We have recd. a request from a Client.*/
						/*Set flag to fire a reply*/
						return(WebServer_ProcessRequest(Pack));
					}else if((Pack->FIN==1)){
						/*We've got a FIN(ACK?) from a client.*/
						/*ACK that,and thats the end of a connection*/
						return(ackTcp(Pack,(Pack->ip.len)+14,0,0,0,0));
					}	
			  }
			/*<=============WEBSERVER HANDLER END====================>*/
								
								  
			/*<-------------WEBCLIENT HANDLER START------------------>*/
			if((Pack->destPort==WClientPort)&&(memcmp(Pack->ip.source,serverIP,4)==0)){
				if((Pack->SYN==1)&&(Pack->ACK==1)){
					ackTcp(Pack,(Pack->ip.len)+14,0,0,0,0);
					WebClient_BrowseURL(Pack);
					WebClientStatus=3;
				}else{
					if( (WebClientStatus==3)&&((len-sizeof(TCPhdr))>12)) {
						/*Process Data*/
						WebClient_ProcessReply(Pack);
					}
					if(Pack->FIN==1){
						/*Reply with a FIN-ACK*/
						ackTcp(Pack,(Pack->ip.len)+14,0,1,0,0);
						WebClientStatus=0;
					}else if( ((Pack->ip.len-0x0028)>0 )&&(WebClientStatus!=0)) {
						/*Just ACK stuff that comes in*/
						ackTcp(Pack,(Pack->ip.len)+14,0,0,0,0);
					}
				}
			}
			/*<=============WEBCLIENT HANDLER END====================>*/            
			}
			
			/*Packet type check,as passed via proto*/
            if( ip->protocol == proto ){
                /*Yes,there is a packet of your requested protocol type.*/
                return 1;
            }
        
        	/*<------------UDP HANDLER START--------------------------->*/
			if( ip->protocol == UDPPROTOCOL){
				UDPPacket* UDPPtr = (UDPPacket*)packet;
				UDP_ProcessIncoming(UDPPtr);
				return 1;
			}
        	/*<------------UDP HANDLER END------------------------------>*/
			
   
    }
    
    return 0;
}
}


/*******************************************************************************
* Function Name: IPstackIdle
********************************************************************************
* Summary:
*   This function can be called in the Idle period of the stack,
*   possibly in an endless loop after it has finished the main tasks.
*   It uses GetPacket,and so can auto-reply to Pings and ARP requests.
* Parameters:
*   none.
*             
* Returns:
*  none.
*******************************************************************************/
void IPstackIdle(void){
    unsigned char packet[MAXPACKETLEN];
    TCPhdr* TCPacket = (TCPhdr*)packet;
    
    /*Check if Link is Up*/
    if(IsLinkUp()==0){
        return;
    }
    
    /*Nothing specific,just field the Pings and ARP Requests,SYN handshakes and GETs*/
    GetPacket(0,packet);
	
	if(WebClientStatus==1){
		/*Send a SYN*/
		WebClient_SendSYN();
		WebClientStatus=2;
		return;
	}

}

/*******************************************************************************
* Function Name: ackTcp
********************************************************************************
* Summary:
*   Creates an TCP ACK packet from a recd TCP packet.This is used to create
*   all kinds of ACK-s(including SYN-ACK) during an HTTP Transaction.
*
* Parameters:
*   tcp - pointer to the start of a TCP packet which is to be ACK'd.
*   len - length of the TCP packet which is to be ACK'd.
*   syn_val - If this is 1,then the ack will be a SYN ACK with MSS=1408.
*             If this is 0,then its a bare-bones ACK.
*             
* Returns:
*   The length of ACK packet made.
*******************************************************************************/
unsigned int ackTcp(TCPhdr* tcp, unsigned int len,unsigned char syn_val,unsigned char fin_val,unsigned char rst_val,unsigned int psh_val){
    char ack[4];
    unsigned int destPort;
    unsigned char dlength=0;
    unsigned char* datptr;
  
    /*Zero out the checksum fields*/
    tcp->chksum = 0x0;
    tcp->ip.chksum = 0x0;
  
    /*Swap the MACs in the ETH header*/
    memcpy( tcp->ip.eth.DestAddrs, tcp->ip.eth.SrcAddrs, 6 );
    memcpy( tcp->ip.eth.SrcAddrs, deviceMAC, 6 );
  
    /*Swap the IP Addresses in the IP header*/
    memcpy( tcp->ip.dest, tcp->ip.source, 4 );
    memcpy( tcp->ip.source, deviceIP, 4 );
  
    /*Swap the Ports in the TCP header*/
    destPort = tcp->destPort;
    tcp->destPort = tcp->sourcePort;
    tcp->sourcePort = destPort;
    /*Swap the ACK and Sequence numbers*/
    memcpy( ack, tcp->ackNo, sizeof(ack) );
    memcpy( tcp->ackNo, tcp->seqNo, sizeof(ack) );
    memcpy( tcp->seqNo, ack, sizeof(ack) );
	
	
    /*Increment Ack Number if its a SYN,or FIN(ACK) packet,
    If not,then fill it accordingly.*/
    if( (tcp->SYN==1)|| ((tcp->FIN==1)&&(tcp->PSH==0)) ) {//
        /*Increment Ack Number if its a SYN,or FIN packet.*/
        add32( tcp->ackNo, 1 );
    }else{
        add32( tcp->ackNo, len - sizeof(TCPhdr) );
    }
	
	if((tcp->PSH==1)&&(tcp->destPort==WClientPort)){
		 add32( tcp->ackNo, 1 );
	}

    /*If its supposed to be a SYNACK packet,then add MSS option.
    MSS= Maximum segment size
    If not,then set the appropriate value in the length field of TCP
    and dont add any options.*/
    if(syn_val){
        tcp->SYN = syn_val;
        datptr = (unsigned char*)(tcp) + sizeof(TCPhdr);
        *datptr++=0x02;
        *datptr++=0x04;
        *datptr++=HI8(300);
        *datptr++=LO8(300);
        tcp->hdrLen = 0x06;//6 because its length is 24bytes.Multiples of 4bytes.
        dlength+=4;//MSS option is 4bytes,so length of (data)options=4. 
    }else{
        tcp->SYN = 0;
        tcp->hdrLen = 0x05;//5 because its length is 20bytes.Multiples of 4bytes.
    }

    tcp->ACK=1;//Set the ACK flag.
    tcp->PSH=psh_val;//Clear the PSH flag.
    tcp->FIN=fin_val;//Clear the FIN flag.
    tcp->RST=rst_val;//Clear the RST flag.
    
    /*Length of the Packet*/
    len = sizeof(TCPhdr)+dlength;
    
    /*IP Length field.*/
    tcp->ip.len = (len-sizeof(EtherNetII));
    
    /*Compute the checksums*/
    tcp->ip.chksum=checksum((unsigned char*)tcp + sizeof(EtherNetII),sizeof(IPhdr)-sizeof(EtherNetII),0);
    tcp->chksum = checksum((unsigned char*)tcp->ip.source,0x08+0x14+dlength,2);

    return(MACWrite((unsigned char*)tcp,len));
}

/*******************************************************************************
* Function Name: IPstack_Start
********************************************************************************
* Summary:
*   This function initializes the IP Stack,and the Chip.
*   It performs an ARP request with the purpose of finding 
*   the router MAC Address for use by the remaining stack.
*   
*   This function *must be called first.*
*
*   Ensure that RouterIP is correct before calling this function.
*
* Parameters:
*   deviceMAC - The MAC Address you would like to assign to the ENC chip.
*   deviceIP - The IP Address you would like to assign to the ENC chip.
*             
* Returns:
*   TRUE(0)- if the initialization was successful,and routerMAC has been updated properly.
*   FALSE(1) - if the initialization(ARP for Router MAC) was not successful.
*******************************************************************************/
unsigned int IPstack_Start(unsigned char devMAC[6],unsigned char devIP[4]){  
    unsigned int i = 0;
    ARP arpPacket;
    
    /*Copy the passed MAC and IP into our Global variables*/
    memcpy(deviceMAC,devMAC,6);
    memcpy(deviceIP,devIP,4);

    /*Initialize SPI and the Chip's memory,PHY etc.*/
    initMAC( deviceMAC );
    
    /*Check if Link is Up*/
    if(IsLinkUp()==0){
        return FALSE;
    }
    
    /*Lets find the router's MAC address*/
    SendArpRequest( routerIP );
    
    /*Lets wait for the reply*/
    for(i=0; i < 0x5fff; i++){
        if( MACRead( (unsigned char*) &arpPacket, sizeof(ARP) )!=0 ){
            if( (arpPacket.eth.type == (ARPPACKET))&& (arpPacket.opCode == (ARPREPLY)) && (!memcmp( arpPacket.senderIP, routerIP, sizeof(routerIP) )) ){
                /*Aha! The router sends back its MAC.Copy it into our appropriate global var.*/
                memcpy( routerMAC, arpPacket.senderMAC, sizeof(routerMAC) );
                return TRUE;
            }
        }
    }
    return FALSE;
}


/* [] END OF FILE */