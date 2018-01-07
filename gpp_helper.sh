#!/bin/bash

g++ -c -o HC_SR04.o HC_SR04.cpp -g
g++ -c -o main.o main.cpp -g
g++ -Wall -o test HC_SR04.o main.o -lwiringPi -lpthread -lpigpio -g
