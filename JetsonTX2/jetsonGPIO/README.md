# jetsonGPIO
jetsonGPIO - Testbuild

Scott Logan Smith
mrgibbes@gmail.com

A straightforward library to interface with the NVIDIA Jetson TK1, TX1, TX2 Development Kit GPIO  pins.

This is a fork of original code listed below.  I as well as many others I am sure have also modified the code far from the orginal mentioned below for license purposes.

jetsonTX2GPIO_test.cpp is used to test he Forked jetsonTX1GPIO Library.

To compile: g++ -O2 -Wall jetsonTX2GPIO_test.cpp jetsonGPIO.c -o jetsonTX2GPIOTest

To run: sudo ./jetsonTX2GPIOTest

https://github.com/mrgibbes/jetsonTX1GPIO.git

---------------------------------------------------------------------------------------------
A straightforward library to interface with the NVIDIA Jetson TX1 Development Kit GPIO  pins.

Based on Software by RidgeRun
https://developer.ridgerun.com/wiki/index.php/Gpio-int-test.c
 * Copyright (c) 2011, RidgeRun
 * All rights reserved.

and ideas from Derek Malloy Copyright (c) 2012
https://github.com/derekmolloy/beaglebone

exampleGPIApp.cpp describes a simple usage case using a tactile button and LED as input and output.


