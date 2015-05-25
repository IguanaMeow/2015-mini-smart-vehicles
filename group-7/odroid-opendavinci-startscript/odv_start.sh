#!/bin/bash
cd /opt/odv/bin/
nohup ./supercomponent --cid=111 & 
nohup ./lanedetector --cid=111 --freq=20 &
nohup ./proxy --cid=111 --freq=20 &
./driver --cid=111 --freq=20
