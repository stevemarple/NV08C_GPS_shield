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

Fit a zero ohm resistor (0R) in place of C12. Omit L1 and C13.


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

The diagrams below indicate the jumper position. "JP5" indicates the
position of the silkscreen label.

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

