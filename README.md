# frame-decoder

Small school project in C.

gets as input several frames from the txt file "frameeth.txt" and starts printing information for each packet.

example:

input:

ff ff ff ff ff ff ec 9b f3 06 ea b8 00 3a aa aa 03 00 00 00 08 06 01 00 19 06 ea b8 80 00 00 00 00 80 01 00 19 06 ea b8 80 80 0c 00 00 14 00 02 00 0f 00 00 00 00 00 00 00 00

output:

------------------------------------------------------------
Frame: ff ff ff ff ff ff ec 9b f3 06 ea b8 00 3a 00 00 aa aa 03 00 00 00 08 06 01 00 ... ...

==> frame Ethernet 802.3 LLC

MAC address dst: ff ff ff ff ff ff
                         - Tipo T/X:  Broadcast
MAC address src: ec 9b f3 6 ea b8
                         - S/M:  Singlecast
                         - OUI:  EC-9B-F3
Length: 003a (58B)
LLC: aa aa 03 (SNAP)
SNAP:
   OUI: 00 00 00
   Type ID: 0806ARP
Payload:
   01 00 19 06 ea b8 ... ...

Length field corresponds to packet length
------------------------------------------------------------

