/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Basic Webserver Example
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This runs a webserver on the device's IP,on port 80
               and will display a simple webpage showing current die temp.
				
			   This will send a GET Query to a webserver and download a webpage,
			   displaying "200 OK" on the LCD when it gets a successful reply from the server.
			   
 Note : You may change Router IP,DNS Server IP,Webserver port and Webclient port in "globals.c".
 		The reply processing function is in "Webclient.c",named WebClient_ProcessReply.
		
  This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
*/

#include <device.h>
#include "IPstackMain.h"
#include <stdio.h>
#include <string.h>

/*MAC address to be assigned to the ENC28J60*/
const unsigned char myMAC[6] = {0x00,0xa0,0xc9,0x14,0xc8,0x00};

/*IP address to be assigned to the ENC28J60*/
const unsigned char myIP[4] = {192,168,1,153};

void main( void ){
	unsigned char Query[250]="GET /html/demo/hello.html HTTP/1.1\r\nHost: december.com\r\nUser-Agent: PSoC3\r\nConnection: close\r\n\r\n";
    
	LCD_Start();
    LCD_PrintString("Started.");
	
    /*Initialize the IP Stack*/
    IPstack_Start(myMAC,myIP);
	
	/*Set the Query in the global query variable WebClientQuery*/
	memcpy(WebClientQuery,Query,strlen(Query));
	
	/*Perform a DNS Lookup,and a PUT request,if the DNS worked.*/
	if(DNSLookup("december.com")==TRUE){
		/*Send the Query stored in WebClientQuery*/
       	WebClient_Send();
    }
	
    for(;;){
    	IPstackIdle();
    }    
    
}