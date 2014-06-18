# NV08C_IRIG_B

This sketch is designed to replace an old GPS receiver with an IRIG-B
data format output. The format description for the original unit is
described in *System Manual for the Imaging Riometer for Ionospheric
Studies*.

In the original unit a character was sent to set the number of steps
per second (always 8). In this sketch the number of steps per second
is fixed to 8. Thus the serial data stream outputs the date, time and
step number (0-7) 8 times per second. The original unit featured a
time quality byte which is believed to be the number of satellites
used in the time calculation, as the ASCII numerical value (possibly
hexadecimal). As the NV08C-CSM can use more than 15 satellites in its
time calculation the time quality byte displays the ASCII characters `0`
through to `9` for the numbers 0 to 9, `A` for 10, `F` for 15, `G` for
16 etc. 36 or more satellites are indicated by `Z`.

In addition to the serial data stream the built-in LED is used to
indicate the presence of the PPS signal, turning on for 125ms at the
start of each second. A 4 Hz signal on pin 8 can be used to check the
timing accuracy (it toggles state at each step output).

