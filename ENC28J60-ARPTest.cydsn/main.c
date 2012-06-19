/* Basic Ethernet Test Code for ENC28J60
 Author : Kartik Mankad
 Date : 18-06-2012
 This code is licensed as CC-BY-SA 3.0
 Description : This file tests the LAN connection to the ENC28J60,
               by requesting Address Resolution(ARP) of an IP Address 
               assumed present on the network.
               
               An ARP request will resolve an IP address to its corresponding MAC address. For
               this test the MAC that will be resolved is for the router.

               If sent correctly a reply will be recieved that contains the resolved MAC. 
               After resolving the IP the device will stop.
 
 This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from www.embeddedmarket.com
  Software: PSoC Creator 2.1 with cy_boot 3.0
  
Based heavily on the work by Duncan Frost for the MSP430.               
*/

#include <device.h>
#include <string.h>
#include "enc28j60.h"

/*Structure for Ethernet header*/
typedef struct
{
  unsigned char DestAddrs[6];
  unsigned char SrcAddrs[6];
  unsigned int type;
}  EtherNetII;

/*Ethernet Packet types.*/
#define ARPPACKET 0x0806
#define IPPACKET 0x0800

/*A structure defined for 
an ARP Packet*/
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

/*ARP OpCodes*/
#define ARPREPLY  0x0002
#define ARPREQUEST 0x0001

/*ARP hardware types*/
#define ETHERNET 0x0001

/*MAC address to be assigned to the ENC28J60*/
const unsigned char myMAC[6] = {0x00,0xa0,0xc9,0x14,0xc8,0x00};

/*Router MAC is not known at beginning and requires an ARP reply to find out.*/
unsigned char routerMAC[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

/*IP address to be assigned to the ENC28J60*/
unsigned char deviceIP[4] = {192,168,0,153};

/*IP Address of the router,whose hardware address we will
find using the ARP Request.*/
unsigned char routerIP[4] = {192,168,0,1};


/*******************************************************************************
* Function Name: SendArpPacket
********************************************************************************
* Summary:
*   Sets the correct bank,for further register manipulation.
*   Note that you should call this before any register manipulation,to set the
*   right bank.
* Parameters:
*   targetIP - The target IP Address for the ARP Request.(The one whose hardware
*              address we want.)
*   deviceMAC - The MAC Address of the ENC28J60,i.e the source MAC for the ARP
*               request.
*
* Returns:
*   Nothing.
*******************************************************************************/
static void SendArpPacket(unsigned char* targetIP,unsigned char deviceMAC[6]);

static void SendArpPacket(unsigned char* targetIP,unsigned char deviceMAC[6]){
 
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
    arpPacket.hardwareSize = 0x06;//sizeof(deviceMAC);
    arpPacket.protocolSize = 0x04;//sizeof(deviceIP);
    arpPacket.opCode = (ARPREQUEST);
  
    /*Target MAC is set to 0 as it is unknown.*/
    memset( arpPacket.targetMAC, 0, 6 );
    
    /*Sender MAC is the ENC28J60's MAC address.*/
    memcpy( arpPacket.senderMAC, deviceMAC, 6 );
    
    /*The target IP is the IP address we want resolved.*/
    memcpy( arpPacket.targetIP, targetIP, 4);
  
    /*Check if the last reply has come from an IP Address that we want,
    i.e someone else is already using it.*/
    if ( !memcmp( targetIP, deviceIP, 4 ) ){
        /*Yes,someone is using our IP.Then set the sender IP to 0.0.0.0*/
        memset( arpPacket.senderIP, 0, 4 );
    }else{
        /*No,no one is using our IP,so we can use it confidently.*/
        memcpy( arpPacket.senderIP, deviceIP, 4 );
    }
  
    /*Send the Packet*/
    MACWrite((unsigned char*)&arpPacket,sizeof(ARP));
}

void main( void ){  
    /*define a packet,of structure ARP,and lets call it arpPacket*/
    ARP arpPacket;
   
    /*Start the LCD,to be used for debugging purposes.*/
    LCD_Start();
    
    /*Initialize the ENC28J60*/
    initMAC(myMAC);
  
    /*Tell the user its Init'd.*/
    LCD_PrintString("Initialized");
    CyDelay(700);
    LCD_ClearDisplay();
  
    /*Send out an ARP*/
    SendArpPacket(routerIP,myMAC);
  
    /*Tell the user you sent an ARP Packet.*/
    LCD_PrintString("ARP Sent!");
    CyDelay(700);
    LCD_ClearDisplay();
  
    /*Go into a loop waiting for the reply,if any.*/
    while(1){
        /*This will read a packet,if any and write it into arpPacket*/
		if(MACRead((unsigned char*)&arpPacket,sizeof(ARP))==0){
           continue;
           }
        
        /*Check if the router sent us the packet we recd.*/
        if ( !memcmp( arpPacket.senderIP, routerIP, 4 ) ){
        
            /*Ah Success! We have found our Router's MAC Address*/
            /*Copy out the routers MAC address.*/
            memcpy( routerMAC, arpPacket.senderMAC,6 );
            
            /*Display it*/
            /*This wont come fully on a 16x2 display.Ah well.*/
            LCD_ClearDisplay();
            LCD_PrintString("ARP got!");
            LCD_Position(1,0);
            LCD_PrintInt8(routerMAC[0]);
            LCD_PutChar(':');
            LCD_PrintInt8(routerMAC[1]);
            LCD_PutChar(':');
            LCD_PrintInt8(routerMAC[2]);
            LCD_PutChar(':');
            LCD_PrintInt8(routerMAC[3]);
            LCD_PutChar(':');
            LCD_PrintInt8(routerMAC[4]);
            LCD_PutChar(':');
            LCD_PrintInt8(routerMAC[5]);
            break;
    }
  }
  
  
  for(;;);
}



