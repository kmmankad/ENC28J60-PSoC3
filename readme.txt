ENC28J60 with the PSoC3
----------------------

This project aims to create an Internet connected PSoC3 based device.

Current status: 
-ENC28J60 Hardware Test Code Done.(ENC28J60-HWTest.cydsn)
-ARP Request/Reply Code Done.(ENC28J60-ARPTest.cydsn)
-UDP done.(Example awaited.)
-WebServer done.  |
-DNS Done.        |--(ENC28J60-BasicWebserver.cydsn)
-Webclient done.  |

--Update 8 July 2012 - WebClient works fine now.Thanks to duncanspumpkin(http://mostlyprog.wordpress.com) who pointed out that I hadnt zero-ed out the checksum(before calculation) in the SYN sending function.

-Examples for UDP and also more complex Webclient,Webserver examples under construction.


This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
  
Based heavily on the work by Duncan Frost for the MSP430.
Credits and sincere thanks to him.


-kmmankad

http://kmmankad.blogspot.com

This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.

Last Updated on 07-July-2012.