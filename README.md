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

###Crucial Nodes
#####Watchdog

Keep track of all nodes and error messages on the CAN line. Will initiate shutdown sequence if necessary, or light up the correct error lights.

#####Control Panel

Display all necessary information to the driver.

#####Emergency Buttons

Keep track of Emergency button states. Will not necessarily act on Emergency button presses, as most of them will be analog. 

#####Insulation Modelling

Ensure no short-circuits throughout all electrical wiring. 

#####BMS

Keep track of battery charge and ensure that the cells are not undervolted.

#####Motor Controller/Throttle

Give throttle control to the motor controller via CAN. Keep track of motor controller error codes and warnings.

##### Precharge Check Node

Detects when the motor controller has finished precharging.

###Extra Nodes
#####Telemetry
#####Real-time Telemetry w/ bluetooth
