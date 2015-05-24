#!/bin/bash
#written by Nicolas Kheirallah
#Code too test the software on Odroid or a Lubuntu installation
/usr/bin/lxterminal -e ./supercomponent --cid=111 
/usr/bin/lxterminal -e ./cockpit --cid=111
/usr/bin/lxterminal -e ./camgen --cid=111 --freq=20
/usr/bin/lxterminal -e ./lanedetector --cid=111 --freq=20
/usr/bin/lxterminal -e ./driver  --cid=111 --freq=20
/usr/bin/lxterminal -e ./vehicle --cid=111 --freq=20
/usr/bin/lxterminal -e ./irus --cid=111 --freq=20
