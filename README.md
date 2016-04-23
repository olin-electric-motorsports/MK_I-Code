# CAN Nodes and API 2015-2016
This repository contains all the CAN programming for the 2015-2016 school year.

## Usage

To make and flash an ATmega with a certain Node's code (the most common operation), use the following:

```
$ sudo make build={folder name} flash
```

Note that {name} should be the folder of the \*.c code you wish to flash. This allows you to split up your code into multiple files if you need it. For example:

```
$ sudo make build=demos/Blinky flash
```

## TODO
Make more nodes

## Nodes
*note: These nodes are not necessarily in order of importance*

#####All
- Detect heartbeat and have auto-resetable CAN line in case it detects that its own CAN line has failed.
- Sends out some sort of heartbeat message
- Internal watchdog circuit activation
- Completely asynchronous code architecture
- CPU shutoff when not interrupted. There are no nodes which require there to be ANY code in the infinite `for(;;)` loop (or `while` loop if thats how you roll).

#####Dashboard
- Display all necessary information to the driver via LCD
- Detect startup button
- Detect the switches that are attached to it

#####Panel Board
- Keep track of all nodes and error messages on the CAN line. Will initiate shutdown sequence if necessary, or light up the correct error lights.
- Sends out heartbeat so that every node knows its CAN line is working

##### Bulkhead
- Senses Throttle position every 100ms and sends a CAN message with information
- Senses Break every 100ms and sends a CAN message with information. Will also set throttle to 0 if break is pressed.
- Detects throttle implausibility with 10% difference on Pots as well as over-reading or under-reading
- Wheel speed measurement 

##### Air-control Board
- Detects when the motor controller has finished precharging.

#####BMS x 4
- Monitors cell voltage to ensure that they do not overvolt (>4.15V) or undervolt (<3V). 
- Shunts cells if over-volted for 9/10 of a second then rests for a tenth of a second
- Monitors temperature of cells
- Monitors temperature of self

- On undervolt or overtemperature it will shutdown the power draw of the car and send a CAN notification

- Heartbeat includes cell shunting status

#####MCC
- Supplies PWM on isolated line to motor controller throttle inputs.
- CAN message updates throttle with timeout system to disable throttle if CAN message is not timely.
- Recieves wheel speed updates for traction control
- Manages FS switch for Motor controller functionality

