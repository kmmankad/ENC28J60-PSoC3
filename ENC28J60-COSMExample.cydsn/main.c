/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : COSM Example - WebClient
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This runs a webserver on the device's IP,on port 80
               and will display a simple webpage showing current die temp.
				
			   Webclient part will upload current die temperature to COSM.com,every 30 seconds.
			   If the DNS Query works,a "DNS Worked." message will be displayed.
			   Change the Query's API Key and FEED ID(65875 here.) for your case.
			   For each successful reply from the COSM server,a '.' will be printed on the character LCD,
			   and an LED connected to P6_2(sourcing) will blink.
			   
			   Feed link - https://cosm.com/feeds/65875
			   
 Note : You may change Router IP,DNS Server IP,Webserver port and Webclient port in "globals.c".
 		The reply processing function is in "Webclient.c",named WebClient_ProcessReply.
		The webpage creating function is in "Webserver.c",named WebServer_ProcessRequest
		
  This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
*/

#include <device.h>
#include <stdio.h>
#include <string.h>

/*Include the main file of the IPStack*/
#include "IPstackMain.h"


/*MAC address to be assigned to the ENC28J60*/
const unsigned char myMAC[6] = {0x00,0xa0,0xc9,0x14,0xc8,0x00};

/*IP address to be assigned to the ENC28J60*/
const unsigned char myIP[4] = {192,168,1,153};

/*Bit that gets set every Terminal count of the timer*/
bit QSend=0;

/*Bit to reflect the success of the DNS Query*/
bit DNSWorked=0;

/*ISR that is executed every terminal count of the timer,
and this sets the QSend bit,so that the webclient sends a query*/
CY_ISR(RequestSender){
	QSend=1;//Set the bit to flag a query transmission.
	Timer_ReadStatusRegister();//Read Status Reg to clear interrupt.
}

void main( void ){
	/*Variable to hold the Die Temp*/
    int16 PSoCTemp;
	
	/*Start the Interrupt component.*/
	TimerInt_Start();
	
	/*Set the vector to our above written ISR*/
    TimerInt_SetVector(RequestSender);
	
	/*Start the LCD*/
	LCD_Start();
	
    /*Initialize the IP Stack*/
    IPstack_Start(myMAC,myIP);
	
	/*Start the Timer*/
	Timer_Start();
	
	/*Enable Global Interrupts*/
	CyGlobalIntEnable;


    for(;;){
		/*If 30seconds are up..*/
		if(QSend==1){	
			/*DNS Lookup*/
			if(DNSWorked==0){
				DNSWorked=~DNSLookup("api.cosm.com");
				LCD_PrintString("DNS Worked.");
				LCD_Position(1,0);
			}
			/*Format and send the PUT Request to api.cosm.com*/
			if(DNSWorked==1){
				DieTemp_GetTemp(&PSoCTemp);
				sprintf(WebClientQuery,"PUT /v2/feeds/65875.csv HTTP/1.1\r\nHost: api.cosm.com\r\nX-ApiKey:bCbTKgjgs3WzkSUBQdkeSjd4x0CSAKxPVE1OazB1NnNJRT0g\r\nUser-Agent: PSoC3\r\nContent-Length: 14\r\nContent-Type: text/csv\r\nConnection: close\r\n\r\nTemperature,%d\r\n",PSoCTemp);	
    			WebClient_Send();
				StatusLED_Write(1);
			}
   			/*Reset the flag*/
			QSend=0;
		}
		 	IPstackIdle();
	}    
}    