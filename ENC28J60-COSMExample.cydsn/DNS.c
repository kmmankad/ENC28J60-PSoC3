/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : DNS Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines the function required for
               carrying out DNS Lookups.
*/


#include "IPStackMain.h"

/*******************************************************************************
* Function Name: DNSLookup
********************************************************************************
* Summary:
*   This function performs a DNSLookup and saves the returned IP Address,
*   (if any) in the variable serverIP defined in "globals.c"
*
* Parameters:
*   url - a constant character string that is the domain name to be
*         looked up. eg: DNSLookup("google.com");
*             
* Returns:
*   TRUE(0)- if the lookup was successful,and serverIP has been updated properly.
*   FALSE(1) - if the lookup was not successful.
*******************************************************************************/
unsigned int DNSLookup( const char* url ){
    /*Setup variables required*/
    unsigned char packet[MAXPACKETLEN];
    unsigned int len,noChars = 0;
    const char* c;
    unsigned char* dnsq;
    unsigned int timeout=9000;
    
	/*Structure the variable 'packet' as a DNS header*/
    DNShdr* dns = (DNShdr*)packet;
    
    /*Setup a basic IP packet,of type UDP,since DNS uses UDP
      as a transport layer*/
    SetupBasicIPPacket(packet, UDPPROTOCOL, dnsIP);
    
    /*Zero out the IP flags*/
    dns->udp.ip.flags = 0x0;
    
    /*----Setup UDP Header----*/
    dns->udp.sourcePort = (0xABCD);//chosen at random.
    dns->udp.destPort = (DNSUDPPORT);//Set destination port 53.
    dns->udp.len = 0;
    dns->udp.chksum = 0;
  
  
    dns->id = (0xbaab); //chosen at random.
    dns->flags = (0x0100);
    dns->qdCount = (1);
    dns->anCount = 0;
    dns->nsCount = 0;
    dns->arCount = 0;
    
    /*----Setup the DNS Query----*/
    /* Format the URL into a proper DNS Query*/
    dnsq = (char*)(packet + sizeof(DNShdr) + 1);//Note the +1.
 
    for( c = url; *c != '\0' && *c !='\\'; ++c, ++dnsq){
        *dnsq = *c;
        if ( *c == '.' ){
            *(dnsq-(noChars+1)) = noChars;
            noChars = 0;
        }
        else ++noChars;
    }
    
    *(dnsq-(noChars+1)) = noChars;
    *dnsq++ = 0;
    
    /* Define the host type and class*/
    *dnsq++ = 0;
    *dnsq++ = 1;
    *dnsq++ = 0;
    *dnsq++ = 1;
    
    /*Length of the Packet*/
    len = (unsigned char*)dnsq-packet;
    
    /*Set the IP and UDP length fields*/
    dns->udp.len = (len-sizeof(IPhdr));
    dns->udp.ip.len = (len-sizeof(EtherNetII));
    
    /*Calculate the UDP and IP Checksums*/
    dns->udp.ip.chksum=checksum((unsigned char*)dns + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    dns->udp.chksum=checksum((unsigned char*)dns->udp.ip.source,(len+8)-sizeof(IPhdr),1);
    //(len+8) because Source IP and DestIP,which are part of the pseduoheader,are 4 bytes each.
    
    /*Send the DNS Query packet*/
    MACWrite(packet,len);
    
    /*Now that we have sent the query,
      we wait for the reply,and then process it.*/
      
    while(timeout--){
        /*Wait for a packet of type UDP*/
        if(GetPacket(UDPPROTOCOL, packet)!=1){
			continue;
		}
        /*We got a UDP packet*/
        /*Check if that packet is sent from port 53,
          i.e. its a DNS reply. */
        if( ((UDPhdr*)packet)->sourcePort == (DNSUDPPORT)){
        /*Yes,its a DNS Reply Packet.*/
            dns = (DNShdr*)packet;
            /*Check if its our ID,and there are no errors.*/
            if ( (dns->id == (0xbaab)) && ((dns->flags && 0x008F)!=0x0080)){
            /*Yes,it is error free,and our DNS Reply.Lets extract the IP*/
                dnsq=packet+len;
                /*Lets go into a loop to browse through the returned resources.*/
                for(;;){
                    if(*dnsq==0xC0){//Is it a pointer?
                        dnsq+=2;
                    }else{
                        /*we just search for the first, zero=root domain
                        all other octets must be non zero*/
                        while (++dnsq < packet+len ) {
                            if(*dnsq == 0){
                                ++dnsq;
                                break;
                            }       
                        }       
                    }
                    /* There might be multipe records in the answer. 
                       We are searching for an 'A' record (contains IP Address).*/
                    if (dnsq[1] == 1 && dnsq[9] == 4) { /*Check if type "A" and IPv4*/
                        /*Aha! We have our IP!.Lets save it to the global variable serverIP*/
                        memcpy( serverIP, dnsq+10, sizeof(serverIP));
                        return(TRUE);
                        break;
                    }
                    /*Advance pointer to browse the remaining records,since we
                      havent got the right one with an IP*/
                    dnsq += dnsq[9] + 10;
                }//for loop to browse records       
        
                break;
            }else{
                return(FALSE);
            }
        }
    }//Outer Packet waiting while loop
    return(FALSE);
}
/* [] END OF FILE */