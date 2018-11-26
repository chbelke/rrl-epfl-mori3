# Structure


### dsPIC33EP512GM604
 - I2C -> Magnetic encoder, accel, DAC, LED/SMA Driver
 - Tx/Rx -> Serial Communication between Modules
 	- Tight tolerance for edge expansion
 	- Synchronize leg movements across many modules
 - Motor Drivers


### ESP8266:
 - Wifi Communication
 - Module configuration
   - Tell microchip angle and connect/disconnect
 - Checks for power
 - Error communication
 - Contains module identifier number
_Options:_
 - Can save state machine and state transition matrix
  	- Will determine it's own location
 - Just relays computer information to MCU

### Computer:
  - Selects user state input
  	
_Options:_
 - Relays just state to ESP
 - Determines which module goes where
 	- Can walk each module through tranistions

# Examples

## Configuration Change Option 1
Computer has control, ESP just a relay

Computer:
 - User sets state to "dog"
 - Retrieves previous configuration 
 	- Either through memory or through asking each ESP8266 (I favour the former)
 - Sends information to each ESP8266
    - Angle and whether to expect to connect
    - Sends information in proper sequence

ESP8266:
 - Sends handshake to computer
 - Relays angle and connection information to MCU

MCU:
 - Recieves angle and connection information
 - Configures motors and SMA to appropriate configuration


## Configuration Change Option 2
State transitions stored in ESP memory (state machine style)

Computer:
 - User sets state to "dog"
 - Sends "dog" configuration to ESP

ESP8266:
 - Sends handshake to computer
 - Looks up state transition between current state and next state (using SPIFFs)
 	- Angles, connections, and sequence
 - Relays angle and connection information to MCU

MCU
 - Recieves angle and connection information
 - Configures motors and SMA to appropriate configuration