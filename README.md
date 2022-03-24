# CyberDeck Microcontroller Computer
Utilizing multiple microcontrollers, eventually create a fully working computer with custom language interpreter.
The idea is to create a robotic cyberdeck. Write in a custom language on a handheld to program robots and games.
A handheld gameboy like device with screen, sound, full keyboard and mouse which can be plugged into the robot chassis. A Rii 8+ Bluetooth Keyboard/Mouse can be used to control the robot system from afar. Think of a handheld Commodore64 that controls a robot.

## Overall Status
Currently I have a simple 2 microcontroller system (2x 4.1 Teensy boards) each with their own 16Mb of PSRAM. They both communicate over Serial1 on ports 0 for Rx and 1 for Tx. One is the mainboard with USB Host functionality for the BT Keyboard/Mouse. Mainboard transmits a simple message if F1, F2, F3, or END is sent from BT controller. The Graphics board simply interprets the Serial message and executes a function. I've cobbled these two sketches together from multiple example code sketches and stackoverflow. It's purely a demo of eventually, what is to come.

## Software Status
I'm debating on where to put the bulk of the main text file handeling code. Either on the mainboard or on the graphics board.
* If placed on the mainboard, the graphics board needs to be sent the entire program text file that is being worked on. Once editing is done, it can then transmit the changes, to the mainboard.
* * I dislike this for a few reasons. Mostly reliability of text files being transmitted back and forth, but speed of the transactions too. Maybe the reliability and speed isn't a problem.
* If placed on the graphics board, both file handeling types graphics and text can be done simply on one chip.
* * This leaves less room for graphics code on the graphics board, but gives program room on the mainboard. I think puting text file handeling on the graphics board might slowdown graphics calls, but we will see.

## Hardware Status
Designing a computer on multiple microcontrollers is a complicated dance of abstraction for speed and reliability. I've got the general idea of what I want to do, but I'd like to do testing of several hardware layouts to ensure that speed and reliability are taken into account. Maybe the computer is a "chain" of microcontrollers, maybe the computer is a "web" of microcontrollers, maybe it's a "chain" and a "web" combined. Will need to test different layouts with different communication protocols to find out.

### System Specifics 
Computer will use multiple microcontrollers. Each microcontroller will have special hardware attached. Not sure in what way to structure the hardware yet. Below is a general idea. Most likely they can all communicate over Serial1, Serial2, Serial3, Serial4, Serial5 ports with no major downsides. SPI and DMA can be used later.
* A mainboard 4.1 runs the handheld, telling the other teensy 4.1 boards to do something, receive interrupts from other teensy boards
* * Using it's SD slot for handheld programs / configuration files. Using the USB host to connect a keyboard.
* One displayboard 4.1 could run the HX8357 or ILI9341 screen
* * Talk to it's SD slot for things like bitmap graphics.
* One soundboard 4.1 could run sound over I2S to a PCM5102
* * Talk to it's SD slot for sound files.
* One robotboard 4.1 could run robot programs.
* * Using it's SD slot for robot logging and extra storage.
* One expansion board 4.1 could be added into an expansion slot for extra computing power.
* * Like a co-processor to offload tasks onto.

### I don't know anymore.
Perhaps utilizing Direct Access Memory (DMA) with SPI is the ticket. IDK if EXMEM/PSRAM is DMA'able but that would be neat.
