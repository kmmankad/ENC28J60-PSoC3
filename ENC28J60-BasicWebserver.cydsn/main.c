/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Basic Webserver Example
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This runs a webserver on the device's IP,on port 80
               and will display a simple webpage showing current die temp.
               It will also respond to pings,and ARP requests.
  WEBCLIENT FUNCTIONALITY UNDER TEST.Probably wont work.
  2-06-2012
*/

#include <device.h>
#include "IPstackMain.h"

/*MAC address to be assigned to the ENC28J60*/
const unsigned char myMAC[6] = {0x00,0xa0,0xc9,0x14,0xc8,0x00};

/*IP address to be assigned to the ENC28J60*/
const unsigned char myIP[4] = {192,168,1,153};


void main( void ){
    LCD_Start();
    /*Initialize the IP Stack*/
    IPstack_Start(myMAC,myIP);
    
    LCD_PrintString("Router MAC");
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
    
    for(;;){
    /*DNS lookup*/
        if(DNSLookup("google.com")==TRUE){
            CyDelay(1000);
            LCD_ClearDisplay();
            LCD_PrintString("Google.com");
            LCD_Position(1,0);
            LCD_PrintNumber(serverIP[0]);
            LCD_PutChar('.');
            LCD_PrintNumber(serverIP[1]);
            LCD_PutChar('.');
            LCD_PrintNumber(serverIP[2]);
            LCD_PutChar('.');
            LCD_PrintNumber(serverIP[3]);
            for(;;){
            WebClient_Send("GET / HTTP/1.1\r\nHost: google.com\r\n\r\n");
            IPstackIdle();
            }
        }
    }
}