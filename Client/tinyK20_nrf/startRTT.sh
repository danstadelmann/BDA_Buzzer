#!/bin/bash
#This scripts the JLinkExe-server for EFM32GG880F1024
echo "connect" | JLinkExe -device MK20DX128xxx5 -speed 4000 -if SWD &

#Starting the JLinkRTTClient:
JLinkRTTClient
