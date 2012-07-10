ENC28J60 with the PSoC3
----------------------------------------------------------------------
This project aims to create an Internet connected PSoC3 based device.

Basic UDP RX/TX Example:
-This project implements a Webserver at its IP,which displays the 
 current Die Temperature on a simple HTML page.

-On receiving a UDP packet with data as "Invoke.",it responds with a UDP
 packet with payload as "Hello World".All other UDP traffic to its IP
 is responded to by an "Access Denied" message as payload to the UDP packet
 it sends in reply.

-Tested with PyUDPComm,a simple python based commandline UDP communicator.
-----------------------------------------------------------------------


--Update 8 July 2012 - WebClient works fine now.Thanks to duncanspumpkin(http://mostlyprog.wordpress.com) who pointed out that I hadnt zero-ed out the checksum(before calculation) in the SYN sending function.

This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
            PyUDPComm v1.0 (https://github.com/kmmankad/PyUDPComm) 
Based heavily on the work by Duncan Frost for the MSP430.
Credits and sincere thanks to him.


-kmmankad

http://kmmankad.blogspot.com

This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.

Last Updated on 08-July-12.