#include <avr/wdt.h>

#include <TinyGPS.h>
#include <AsyncDelay.h>
#include <Wire.h>
#include <RTCx.h>

// GPS parsing object
TinyGPS nmea;

// Refer to streams logically
Stream& console = Serial;
Stream& gps = Serial1;

bool debug = false;
AsyncDelay debugTimeout;
unsigned long maxDebugTime_ms = 300000; // 5 minutes


/*
 * IRIG-B format is:
 *  1 '*' 
 *  2 step number ('0' to '7')
 *  3 units second digit (ASCII decimal) 
 *  4 tens second digit 
 *  5 units minute digit 
 *  6 tens minute digit 
 *  7 units hour digit 
 *  8 tens hour digit 
 *  9 units digit day-of-year 
 * 10 tens digit day-of-year 
 * 11 hundreds digit day-of-year 
 * 12 tens year digit 
 * 13 units year digit 
 * 14 time quality 
 * 15 CR 
 * 16 LF 
 */

const int bufferLen = 20; 
char bufferA[bufferLen] = "";
char bufferB[bufferLen] = "";
char *nextTime = bufferA;
char *thisTime = bufferB;
bool thisFixValid = false;
bool nextFixValid = false;

RTCx::time_t nextPPS = 0;

volatile bool ppsTriggered = false;

// Toggle an output pin state at every step. This will be useful for
// checking the timing and delays.
bool squareWaveOut = LOW;
uint8_t squareWavePin = 8;

// The original IRIG-B unit read a character from the serial stream to
// configure the number of steps per second. Configure in the firmware
// instead - it's more useful to keep the serial data input available
// for talking to the bootloader. stepsPerSecond should divide exactly
// into 1000 otherwise the steps will not be evenly distributed
// through the second.
const uint8_t stepsPerSecond = 8;
uint8_t stepCounter = 0;
AsyncDelay stepDelay;

void resetGPS(bool cold = true);

void ppsHandler(void)
{
  ppsTriggered = true;
}


void resetGPS(bool cold)
{
  if (cold)
    gps.print("$PORST,F*20\r\n"); // Factory restart and cold start
  else
    gps.print("$PORST,W*31\r\n"); // Keep settings and warm restart
}


void formatTime(const RTCx::time_t &t, uint16_t numSat,
		char *buffer, size_t bufferLen)
{
  struct RTCx::tm tm;
  char satCode;
  
  if (numSat == TinyGPS::GPS_INVALID_SATELLITES)
    satCode = '0';
  else if (numSat < 10)
    satCode = '0' + numSat; // '0' to '9'
  else if (numSat > 35)
    satCode = 'Z'; 
  else
    satCode = ('A' + numSat - 10); // 'A' to 'Z'

  RTCx::gmtime_r(&t, &tm);

  int yy;
  if (tm.tm_year + 1928 < 2000)
    yy = 00;
  else
    yy = (tm.tm_year + 1928) % 100;
  
  int ydayp1 = tm.tm_yday + 1;
  snprintf(buffer, bufferLen,
	   "%1d%1d%1d%1d%1d%1d%1d%1d%1d%02d%c\r\n",
	   (tm.tm_sec % 10), (tm.tm_sec / 10),
	   (tm.tm_min % 10), (tm.tm_min / 10),
	   (tm.tm_hour % 10), (tm.tm_hour / 10),
	   (ydayp1 % 10), ((ydayp1 / 10) % 10), (ydayp1 / 100),
	   yy, satCode);

}


void setup(void)
{
  wdt_enable(WDTO_8S);
  
  // Set I/O as inputs with pull-ups. Leave D0 and D1 as normal INPUT
  // without pullups.
  for (uint8_t i = 2; i < NUM_DIGITAL_PINS; ++i)
    pinMode(i, INPUT_PULLUP);
  
#ifdef CALUNIUM
  pinMode(3, INPUT);
  pinMode(4, INPUT);
#else
  // Assume Arduino Mega(2560)
  pinMode(18, INPUT);
  pinMode(19, INPUT);
#endif
  Serial.begin(9600); // console
  Serial1.begin(115200); // gps

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(squareWavePin, OUTPUT);
  digitalWrite(squareWavePin, LOW);
  // resetGPS();
  
  // Compatibility mode off
  // $PONME,,,1,0*58

  // Set compatibility mode on. Talker ID is set to "GP" even when
  // using GNSS, so that GPGGA is sent, not GLGGA or GNGGA.
  gps.print(",,,0,0*59\r\n");

  // Delete all messages from the list which are sent.
  gps.print("$PORZB*55\r\n");

  // Send RMC and GGA messages.
  gps.print("$PORZB,RMC,1,GGA,1*48\r\n");

#ifdef CALUNIUM
  pinMode(6, INPUT);
  attachInterrupt(2, ppsHandler, RISING);
#else
#error Please configure interrupt handler code for alternative board.
#endif
}


void loop(void)
{
  wdt_reset();
  
  if (ppsTriggered) {
    ppsTriggered = false;
    stepCounter = 0;

    // Swap buffers
    char *tmp = thisTime;
    thisTime = nextTime;
    nextTime = tmp;

    // Invalidate data in the nextTime buffer
    *nextTime = '\0';

    thisFixValid = nextFixValid;
    nextFixValid = false;
    
    squareWaveOut = HIGH;
  }

  if ((stepCounter == 0 || stepDelay.isExpired())
      && stepCounter < stepsPerSecond && *thisTime) {

    digitalWrite(squareWavePin, squareWaveOut);
    squareWaveOut = !squareWaveOut;

    // Start timer before sending serial data (might be slow)
    stepDelay.start(1000 / stepsPerSecond, AsyncDelay::MILLIS);

    if (*thisTime) {
      // Print step details to console
      console.print('*');
      console.print(stepCounter, DEC);
      console.print(thisTime);
    }

    switch (stepCounter) {
    case 0:
      // Turn LED on if have a valid fix
      digitalWrite(LED_BUILTIN, thisFixValid);

      // Generate the next time string based on current time. This
      // occurs immediately after the string for the first time step
      // has been printed, and before any NMEA processing for the
      // current time can have occurred. Thus if another value occurs
      // (eg time correction, leap seconds etc) then it will overwrite
      // this value.
      formatTime(++nextPPS, 0, nextTime, bufferLen);
      break;

    case 1:
      digitalWrite(LED_BUILTIN, !thisFixValid);
      break;
    }
	
    ++stepCounter;
  }
  
  while (!ppsTriggered && gps.available()) {
    char c = gps.read();
    if (debug) {
      console.print(c);
    }
    
    if (nmea.encode(c)) {
      // New sentence
      int16_t year;
      uint8_t month, mday, hour, minute, second, hundredths;
      unsigned long fix_age;
      nmea.crack_datetime(&year, &month, &mday,
			  &hour, &minute, &second, &hundredths, &fix_age);

      if (fix_age < 1000) {
	struct RTCx::tm tm;
	tm.tm_year = year - 1928; // Use 1928, not 1900 for extra range
	tm.tm_mon = month - 1;
	tm.tm_mday = mday;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;

	// Seconds since 1998, not 1970. This gives 28 years more before
	// the unix time rollover bug kicks in, ie 2038+28 = 2066.
	nextPPS = RTCx::mktime(tm) + 1;
	nextFixValid = true;
	formatTime(nextPPS, nmea.satellites(), nextTime, bufferLen);
	

	if (debug) {
	  console.println();
	  console.print("Next: ");
	  console.print(nextTime);
	}
	
      }
    }
  }

  while (!ppsTriggered && console.available()) {
    char c = console.read();
    if (c == '\n')
      gps.print("\r\n");
    else if (c == '!') {
      console.println("Reset GPS");
      resetGPS(false);
    }
    else if (c == '#') {
      debug = !debug;
      if (debug) {
	console.println("Entering debug mode");
	debugTimeout.start(maxDebugTime_ms, AsyncDelay::MILLIS);
      }
      else
	console.println("Leaving debug mode");
    }
    else if (c == 'f') {
      console.print("MCU frequency: ");
      console.println(F_CPU, DEC);
    }
    else
      gps.print(c);
  }

  // Do not stay in debug mode permanently in case the symbol to enter
  // was sent accidentally.
  if (debug && debugTimeout.isExpired()) {
    console.println("Debug mode timeout");
    debug = false;
  }
}
