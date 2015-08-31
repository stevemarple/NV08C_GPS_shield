# NV08C GPS shield v1.0

## Build instructions

The Gerber files are intended to use with 1.2mm thickness PCB in order
to obtain the correct 50 ohm impedance for the antenna track.

The PCB footprints have been chosen to enable soldering by hand rather
than requiring a reflow oven. The build order below should mean all
parts are accessible for soldering.

  1. NV08C module.
  2. 74LVC1T45 logic level translators.
  3. U.FL connector (if using).
  4. Surface mount resistors.
  5. Surface mount capacitors.
  6. Battery holder.
  7. Jumpers.
  8. Stacking headers.
  9. MCP1702 voltage regulator (3.3V, if using).
  10. SMA connector.


## Power supply

The 3.3V supply for the PCB can be derived from the main 5V supply by
using an onboard voltage regulator. In this case fit C1 (1u), C2 (1u)
and IC1 (MCP1702-3.3). A shunt should be fitted to JP1 to select the
`VREG` option. 

If preferred the PCB can be powered from the 3.3V supply; fit a shunt
to JP1 to select the `3V3` option. C1, C2 and IC1 can be omitted. On a
standard Arduino or Arduino Mega2560 board the 3.3V supply is limited
to a maximum current draw of 50mA which is not enough for the NV08C
module.


## Battery backup

To enable battery backup of the NV08C memory contents and to maintain
the current time fit a CR2032 lithium cell to the battery holder. The
board can be safely used without a backup battery; a pair of Schottky
diodes ensure power is always provided when the 3.3V supply is on.

  
## Active and passive antenna options

Different active and passive antenna options are possible depending on
which components are fitted. 

### Active antenna, 2.85V supply.

To use the internal 2.85V supply fit a zero ohm resistor (0R) in place of C12. Omit L1, C13 and JP6.


### External or no power supply

Fit C12 (22p), C13 (22p) and L1 (47n). The antenna supply voltage is
selected by JP6 (V_ANT). Depending on how a shunt is fitted either a
3.3V or 5V supply is provided to the active antenna. Omit the shunt if
using a passive antenna.


## PPS output

The PPS output can be connected to digital inputs 6 or 7 by fitting a
shunt to JP8. The shunt can be omitted if the PPS output is not
required.


## NV08C reset

A hardware reset of the NV08C is possible by fitting a shunt to
JP4. In this case A0 should be configured as an output and must be
high for normal operation of the NV08C module.


## Selection of Arduino and NV08C UARTs

The PCB enables the Arduino Serial and Serial1 UARTs to be connected
to the NV08C UART1 or UART2. UART1 and UART1 can be used
simultaneously but do not connect Serial and Serial1 to the same NV08C
UART, it will short the Serial and Serial1 TX lines together.

The diagrams below indicate the jumper position with a `=` marked between the jumper pins (`o`). "JPx" indicates the
position of the silkscreen label.

**JP1, 3V3 supply**

Source of the board's 3V3 power supply, which can be derived either form the Arduino's 3V3 supply or regulated from the Arduino's 5V supply by on onboard MCP1703-3.3 voltate regulator. Warning: JP1 is immediately adjecent to JP6.

  3V3 derived from on-board regulator:

    o=o o  JP1

  3V3 from Arduino's 3V3 supply:

    o o=o  JP1

**JP2, ISP link**

Use 3 jumpers to link digital pins 11, 12, and 13 to the ISP header. Fit jumpers only on Arduino boards where digital pin 11 is !MOSI, 12 is !MISO, and 13 is SCK. Do not fit the jumpers when using an Arduino Mega.

  Link SPI bus to ISP header:

      JP2   
     o = o
     o = o
     o = o


**JP3, ISP VCC**

Select voltage supply connected to the ISP header. On the standard Atmel ISP header the VCC pin indicates the voltage level used for signalling. On the Arduino the ISP header is normally connected to 5V, even for boards with 3V3 output levels.

  ISP header VCC connected to 5V:
  
    JP3  o=o o

  ISP header VCC connected to IOREF:
  
    JP3  o o=o


**JP4, GPS reset**

Fit jumper to use `A0` digital pin to control the GPS !RESET input. If no jumper is fitted then to reset the NV08C-CSM the power must be cycled or a serial command sent.

**JP5**

  Serial connected to UART1:

     JP5
    o o o
    o o o
    o=o o
    o=o o


  Serial1 connected to UART1:

     JP5
    o o o
    o o o
    o o=o
    o o=o


  Serial connected to UART2:

     JP5
    o=o o
    o=o o
    o o o
    o o o


  Serial1 connected to UART2:

     JP5
    o o=o
    o o=o
    o o o
    o o o


**JP6, GPS antenna supply voltage**

Select the supply voltage for an active GPS antenna, or omit the jumper for a passive antenna. The jumper block should not be fitted if the internal 2.85V supply is to be used.


**JP7, config**

Start-up Configuration option for the NV08C-CSM GPS module. From left to right the poistions are for GPIO3-GPIO7. GPIO3-5 are pulled up to 3V3 when a jumper is fitted, GPIO6 and GPIO7 are pulled to GND when the jumper is fitted. See datasheet for more information.


**JP8, pulse-per-second**

Fit a jumper to connect the pulse-per-second (PPS) output to digital pin 6 or 7.

  PPS connected to digital pin 7:
 
     JP8
    o=o o
  
  PPS connected to digital pin 6:
  
     JP8
    o o=o
  
