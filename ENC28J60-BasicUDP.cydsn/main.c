/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Basic UDP RX/TX Example
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This runs a webserver on the device's IP,on port 80
               and will display a simple webpage showing current die temp.
				
			   On receiving a UDP Packet with payload as "Invoke.",it will respond with a 
			   UDP Packet with payload "Hello World",and any other payload directed to it 
			   will be replied to with a UDP packet containing "Access Denied."
			   
			   See the screenshots in the second and third pages of the Top Design canvas.
			   
 Note : You may change Router IP,Webserver port and UDP Source port in "globals.c".
 		The UDP reply processing function is in "UDP.c",named UDP_ProcessIncoming.
		
  This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0 & PyUDPComm v1.0 (https://github.com/kmmankad/PyUDPComm)
*/

#include <device.h>
#include "IPstackMain.h"

/*MAC address to be assigned to the ENC28J60*/
const unsigned char myMAC[6] = {0x00,0xa0,0xc9,0x14,0xc8,0x00};

/*IP address to be assigned to the ENC28J60*/
const unsigned char myIP[4] = {192,168,1,153};

void main( void ){
    
    /*Initialize the IP Stack*/
    IPstack_Start(myMAC,myIP);
	
    for(;;){
    	IPstackIdle();
    }    
    
}