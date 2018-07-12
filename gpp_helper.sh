#!/bin/bash

g++ -c -o HC_SR04.o HC_SR04.cpp -std=c++14 -g
g++ -c -o main.o main.cpp -std=c++14 -g
g++ -Wall -o test HC_SR04.o main.o -std=c++14 -lwiringPi -lpthread -lpigpio -lrt -g
