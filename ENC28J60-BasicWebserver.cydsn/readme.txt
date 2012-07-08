ENC28J60 with the PSoC3
----------------------------------------------------------------------
This project aims to create an Internet connected PSoC3 based device.

WebServer( and WebClient Example):
-This project implements a Webserver at its IP,which displays the 
 current Die Temperature on a simple HTML page.
-This project performs a DNS Request for www.december.com and if successful in
 getting its IP,proceeds to make an HTTP Request as follows.
-This project also sends a GET request to http://www.december.com/html/demo/hello.html,and
 displays "200 OK" on a connected Character LCD when it receives a successful reply from the server.


-----------------------------------------------------------------------
Current status: 
-WebClient,Webserver,DNS,PING,ARP all work perfectly.
-For Webserver Traffic dump(Wireshark 1.6.8) see Traffic Dumps/WebServer_Traffic.txt
-For WebClient Traffic dump(Wireshark 1.6.8) see Traffic Dumps/WebClient_Traffic.txt
--WebClient Traffic dump is for the PSoC downloading a webpage hosted on a local WAMP server,
  since I dont know how to sniff on the router.

--Update 8 July 2012 - WebClient works fine now.Thanks to duncanspumpkin(http://mostlyprog.wordpress.com) who pointed out that I hadnt zero-ed out the checksum(before calculation) in the SYN sending function.

This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
  
Based heavily on the work by Duncan Frost for the MSP430.
Credits and sincere thanks to him.


-kmmankad

http://kmmankad.blogspot.com

This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.

Last Updated on 08-July-12.