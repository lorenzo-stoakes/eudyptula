#!/bin/bash

insmod hello.ko
echo "AliceAliceAliceAliceAlice" > /dev/eudyptula
echo "Bob" > /dev/eudyptula
sleep 15
echo "Dave" > /dev/eudyptula
echo "Gena" > /dev/eudyptula
rmmod hello
dmesg -T | tail -4
echo ---
sleep 15
dmesg -T | tail -4
