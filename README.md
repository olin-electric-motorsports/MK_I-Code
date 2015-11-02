# CAN Nodes and API 2015-2016
This repository contains all the CAN programming for the 2015-2016 school year.

## Usage

To make and flash an ATmega with a certain Node's code (the most common operation), use the following:

```
$ sudo make FILE={name} flash
```

Note that {name} should be the filename of the \*.c code you wish to flash without the .c attached. For example:

```
$ sudo make FILE=blinky flash
```

## TODO
Make more nodes

## Nodes
*note: These nodes are not necessarily in order of importance*
(Crucial)
####Watchdog

Keep track of all nodes and error messages on the CAN line. Will initiate shutdown sequence if necessary, or light up the correct error lights.

####Control Panel

Display all necessary information to the driver.

####Emergency Buttons

Keep track of Emergency button states. Will not necessarily act on Emergency button presses, as most of them will be analog. 

####Insulation Modelling

Ensure no short-circuits throughout all electrical wiring. 

####BMS

Keep track of battery charge and ensure that the cells are not undervolted.

####Motor Controller/Throttle

Give throttle control to the motor controller via CAN. Keep track of motor controller error codes and warnings.

(Extra)
####Telemetry
####Real-time Telemetry w/ bluetooth
