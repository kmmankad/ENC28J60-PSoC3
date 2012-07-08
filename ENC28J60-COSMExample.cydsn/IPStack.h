/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Common IP Stack functions,structures,parameters
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines functions,structures,parameters 
               common to protocols implemented.
 
*/

#ifndef IPSTACK_H
#define IPSTACK_H

/*Maximum length of packet that the device will entertain*/
#define MAXPACKETLEN 600

/*UDP Port for DNS Lookup*/
#define DNSUDPPORT 53

/*Ping OpCodes*/
#define ICMPREPLY 0x00
#define ICMPREQUEST 0x08

/*Internet Protocol Codes*/
#define ICMPPROTOCOL 0x1 //ICMP
#define UDPPROTOCOL 0x11 //UDP
#define TCPPROTOCOL 0x6 //TCP

/*Ethernet Packet types*/
#define ARPPACKET 0x0806
#define IPPACKET 0x0800

/*ARP OpCodes*/
#define ARPREPLY  0x0002
#define ARPREQUEST 0x0001

/*ARP Hardware types*/
#define ETHERNET 0x0001


/*Struct for ETH header*/
typedef struct
{
  unsigned char DestAddrs[6];
  unsigned char SrcAddrs[6];
  unsigned int type;
}  EtherNetII;

/*Struct for ARP packet*/
typedef struct
{
  EtherNetII eth;
  unsigned int hardware;
  unsigned int protocol;
  unsigned char hardwareSize;
  unsigned char protocolSize;
  unsigned int opCode;
  unsigned char senderMAC[6];
  unsigned char senderIP[4];
  unsigned char targetMAC[6];
  unsigned char targetIP[4];
} ARP;

/*Struct for IP header*/
typedef struct
{
  EtherNetII eth;
  unsigned char hdrlen : 4;
  unsigned char version : 4;
  unsigned char diffsf;
  unsigned int len;
  unsigned int ident;  
  unsigned int flags;
  unsigned char ttl;
  unsigned char protocol;
  unsigned int chksum;
  unsigned char source[4];
  unsigned char dest[4];
}IPhdr;

/*Struct for TCP Header*/
typedef struct
{
  IPhdr ip;
  unsigned int sourcePort;
  unsigned int destPort;
  unsigned char seqNo[4];
  unsigned char ackNo[4];
  unsigned char NS:1;
  unsigned char reserverd : 3;
  unsigned char hdrLen : 4;
  unsigned char FIN:1;
  unsigned char SYN:1;
  unsigned char RST:1;
  unsigned char PSH:1;
  unsigned char ACK:1;
  unsigned char URG:1;
  unsigned char ECE:1;
  unsigned char CWR:1;
  unsigned int wndSize;
  unsigned int chksum;
  unsigned int urgentPointer;
}TCPhdr;

/*Struct for UDP header*/
typedef struct
{
  IPhdr ip;
  unsigned int sourcePort;
  unsigned int destPort;
  unsigned int len;
  unsigned int chksum;
}UDPhdr;

/*Struct for a complete(with data payload) UDP packet*/
typedef struct
{
  UDPhdr udp;
  unsigned char Payload[MAXPACKETLEN];
}UDPPacket;

/*Struct for a complete(with data payload) TCP packet*/
typedef struct
{
  TCPhdr TCP;
  unsigned char Payload[MAXPACKETLEN];
} TCPPacket;

/*Struct for an ICMP(Ping) header*/
typedef struct
{
  IPhdr ip;
  unsigned char type;
  unsigned char codex;
  unsigned int chksum;
  unsigned int iden;
  unsigned int seqNum;
} ICMPhdr;

/*Struct for a DNS header*/
typedef struct
{
  UDPhdr udp;
  unsigned int id;
  unsigned int flags;
  unsigned int qdCount;
  unsigned int anCount;
  unsigned int nsCount;
  unsigned int arCount;
} DNShdr;

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
unsigned int IPstack_Start(unsigned char deviceMAC[6],unsigned char deviceIP[4]);

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
* none.
*******************************************************************************/
void IPstackIdle(void);

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
void add32(unsigned char *op32, unsigned int op16);

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
*******************************************************************************/
uint16 checksum(uint8 *buf, uint16 len,uint8 type);

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
*   proto - value of the protocol field in the IP header(TCP,ICMP,UDP,IP)
*   destIP - IP address to be filled in the destination IP field.
*             
* Returns:
*   none.
*******************************************************************************/
void SetupBasicIPPacket( unsigned char* packet, unsigned char proto, unsigned char* destIP);


/*******************************************************************************
* Function Name: GetPacket
********************************************************************************
* Summary:
*   This function checks for packets recd,and has been designed to
*   automatically reply to Ping Requests and ARP Requests.
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
*   none.
*******************************************************************************/
unsigned int GetPacket( int proto, unsigned char* packet );

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
unsigned int ackTcp(TCPhdr* tcp, unsigned int len,unsigned char syn_val,unsigned char fin_val,unsigned char rst_val,unsigned int psh_val);



#endif

/* [] END OF FILE */