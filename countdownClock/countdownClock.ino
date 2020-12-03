
/* Trump Countdown Clock

   displays DAYS, HOURS, MINUTES (and SECONDS) on 8 digits
   DDDD : HH : MM
   DD : HH : MM : SS
   flexible display

   HARDWARE:
     Metro Mini https://www.adafruit.com/product/2590
     7-Segment Display w/I2C Backpack https://www.adafruit.com/product/1270 x 2
     DS1307 RTC https://www.adafruit.com/product/264
     Illuminated Toggle Switch w/ Cover https://www.adafruit.com/product/3218


  TO DO:
    - have illuminated switch flash faster when trumpIsOver, code is blocking

  DONE:
    - change dynamically based on remaining days left
    - weird bug where starting digits disappear... why?
    - support for timezone and daylight saving time
    - blink dots for seconds
    - add startup sequence
    - flip up animation for digit changes
    - sequence for reaching 0 secondsLeft
    - add crazy button? would set secondsLeft to 0!
    - write code to pulse illuminated switch up and down
    - define letters

*/

#include <RTClib.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

RTC_DS1307 rtc;

// array to hold our two 4-digit backpacks
Adafruit_7segment A7seg[2] = {Adafruit_7segment(), Adafruit_7segment()};

// unix target date
// countdown until:
// January 20, 2021, 12:00:00 pm
// using: https://www.epochconverter.com/
// this date is in Standard Time
// verify calculations here:
// https://www.timeanddate.com/date/durationresult.html
const unsigned long trumpOver = 1611144000;

//#define GMT_OFFSET -5 // Eastern Standard Time is GMT-5

// Do you live in a country or territory that observes Daylight Saving Time?
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// This is programmed for DST in the US / Canada. If your territory's DST operates differently,
// you'll need to modify the code in the calcTheTime() function to make this work properly.
const boolean OBSERVE_DST = true;

// constants for calculations
//const unsigned int SECONDS_PER_DAY = 86400; // apparently built-in constant for RTClib
//const signed int SECONDS_PER_HOUR = 3600; // must be signed or math gets weird
#define SECONDS_PER_HOUR 3600

const int delayTime = 20;
const int startupDelay = 50;
const int numeralDelay = 150;

const byte BLINKRATE = 2; // 0 is no blink, 1-3 are all acceptable.

const byte brightness = 15; // 0-15 sets brightness
// change based on time of day?

const byte NUMDIGITS = 8;

// set to 2 or 4, define DDDD : HH : MM      or
//                         DD : HH : MM : SS
byte numDayDigits = 4; // set based on days left

// define our digits
// skip 2 and 7, those are the dots
const byte digitMap[NUMDIGITS] = {0, 1, 3, 4, 5, 6, 8, 9};

int clockDigits[NUMDIGITS];
int lastClockDigits[NUMDIGITS];

boolean firstTime = true; // set flag so we turn blanks into numerals on first pass

boolean trumpIsOver = false; // check for a switch or if time is past

const int switchPin = 2;
const int ledPin = 3; // can PWM the LED inside the illuminated switch :)

int ledFade; // for PWM brightness- this is sinked
boolean fadeDirection = true; // flip back and forth for pulsing effect

const int normalFadeAmount = 16;
const int fastFadeAmount = 32;

int fadeAmount = normalFadeAmount;

// define letters
#define LETTER_M B00110111  // how to write M on a 7-segment display


void setup ( ) {

  Serial.begin(115200);

  //Serial.println("program begins!");

  Wire.begin();

  // init RTC
  rtc.begin();

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));

    // DST? If we're in it, let's subtract an hour from the RTC time to keep our DST calculation correct. This gives us
    // Standard Time which our DST check will add an hour back to if we're in DST.
    if (OBSERVE_DST == true) {
      DateTime standardTime = rtc.now();
      if (checkDst() == true) { // check whether we're in DST right now. If we are, subtract an hour.
        standardTime = standardTime.unixtime() - SECONDS_PER_HOUR;
      }
      rtc.adjust(standardTime);
    }
  }

  // pinModes
  pinMode(switchPin, INPUT); // we're using pulldown bc of illuminated switch
  pinMode(ledPin, OUTPUT); // illuminated LED in switch. we're sinking this.


  A7seg[0].begin(0x70);
  A7seg[1].begin(0x71);

  A7seg[0].setBrightness(brightness); // 0-15 sets brightness
  A7seg[1].setBrightness(brightness);

  // clear display
  clearAllDisplays();
  delay(1000);

  //startup sequence
  startupSequence();

  //finalSequence();

  // stop any blink
  A7seg[0].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking

  clearAllDisplays();
  delay(2000);

}


void loop () {

  //Serial.println("loop");
  long secondsLeft; // var for counting the seconds left. needs to be long, int doesn't work!

  // check switch first

  if (digitalRead(switchPin) == HIGH) {
    trumpIsOver = true;
    firstTime = true; // set this up in case we switch back ;)
    Serial.println("switch switched!");

  } else {
    trumpIsOver = false; // not yet..
    // check RTC to see whether trump is over.

    // our time is going to be in Standard Time, not Daylight Saving Time
    // that solves the problem of taking into account Daylight Saving Time since target date is in Standard Time

    delay(1); // trying to stop weird bug where digits go out?
    DateTime theTime = rtc.now();

    // Serial print out
    // RTC Time - Standard Time
    Serial.println("RTC time: ");
    printTheTime(theTime);

    //theTime = theTime.unixtime();

    // Serial print out
    // time with GMT offset
    /*
      Serial.println("time at GMT: ");
      printTheTime(theTime);

      Serial.print("seconds since 1970: ");
      Serial.println(theTime.unixtime());
    */

    // how many seconds left?
    secondsLeft = trumpOver - theTime.unixtime();

    Serial.print("seconds left: ");
    Serial.println(secondsLeft);

    if (secondsLeft < 0) {
      trumpIsOver = true;
      Serial.println("seconds left LESS THAN ZERO");

    }
  }

  // what to do based on whether or not trump is over

  if (trumpIsOver == true) {
    // TRUMP IS OVER!!
    Serial.println("trump is over! (if you want it)");
    delay(1000);
    clearAllDisplays();
    delay(2000); // delay! this is a momentous occasion!
    finalSequence();

  } else {

    // trump's not over :(
    // calculate and display time left

    int daysLeft, hoursLeft, minutesLeft, leftoverSecondsLeft; // create vars

    // calculate days, hours, minutes left
    daysLeft = secondsLeft / SECONDS_PER_DAY;
    hoursLeft = (secondsLeft % SECONDS_PER_DAY) / SECONDS_PER_HOUR; // throw away anything over a full day, divide by number of seconds in an hour
    minutesLeft = (secondsLeft % SECONDS_PER_HOUR) / 60; // throw away anything over a full hour, divide by 60 (seconds per minute)
    leftoverSecondsLeft = secondsLeft % 60;

    // print our calculations of time left
    Serial.print("days left: ");
    Serial.println(daysLeft);
    Serial.print("hours left: ");
    Serial.println(hoursLeft);
    Serial.print("minutes left: ");
    Serial.println(minutesLeft);
    Serial.print("seconds left: ");
    Serial.println(leftoverSecondsLeft);

    // figure out days left, which affects our display
    if (daysLeft < 100) {
      numDayDigits = 2;
    } else {
      numDayDigits = 4;
    }

    // get our digits
    if (numDayDigits == 4) {
      clockDigits[0] = daysLeft / 1000;
      clockDigits[1] = (daysLeft % 1000) / 100;
      clockDigits[2] = (daysLeft % 100) / 10;
      clockDigits[3] = daysLeft % 10;
      clockDigits[4] = hoursLeft / 10;
      clockDigits[5] = hoursLeft % 10;
      clockDigits[6] = minutesLeft / 10;
      clockDigits[7] = minutesLeft % 10;
    } else {
      clockDigits[0] = (daysLeft % 100) / 10;
      clockDigits[1] = daysLeft % 10;
      clockDigits[2] = hoursLeft / 10;
      clockDigits[3] = hoursLeft % 10;
      clockDigits[4] = minutesLeft / 10;
      clockDigits[5] = minutesLeft % 10;
      clockDigits[6] = leftoverSecondsLeft / 10;
      clockDigits[7] = leftoverSecondsLeft % 10;
    }

    // compare digits, have they changed?

    // figure out leading zeros for daysLeft, we're skipping those
    int startingDigit = 0;
    if (numDayDigits == 4) {
      if (daysLeft < 10) {
        startingDigit = 1;
        // clear leading zeros if we drop below
        A7seg[0].writeDigitRaw(0, B00000000);
        A7seg[0].writeDigitRaw(1, B00000000);
        A7seg[0].writeDigitRaw(3, B00000000);
      } else if (daysLeft < 100) {
        startingDigit = 2;
        // clear leading zeros if we drop below
        A7seg[0].writeDigitRaw(0, B00000000);
        A7seg[0].writeDigitRaw(1, B00000000);
      } else if (daysLeft < 1000) {
        startingDigit = 1;
        // clear leading zeros if we drop below
        A7seg[0].writeDigitRaw(0, B00000000);
      }
    } else { // for 2 day digits
      if (daysLeft < 10) {
        startingDigit = 1;
        // clear leading zeros if we drop below
        A7seg[0].writeDigitRaw(0, B00000000);
      }
    }

    // display digits
    for (int i = startingDigit; i < NUMDIGITS; i++) { //skip last digit
      if ((clockDigits[i] != lastClockDigits[i]) || (firstTime == true)) { // skips 0s first round...
        if (numDayDigits == 4) {
          flipNum(i / 4, i % 4, lastClockDigits[i], clockDigits[i]);
        } else { // skip seconds digit
          flipNumSkip(i / 4, i % 4, lastClockDigits[i], clockDigits[i]);
        }
        lastClockDigits[i] = clockDigits[i];
      }
    }

    if (firstTime == true) {
      firstTime = false; // done!
    }

    //void displayDigits();
    // blink the colons every second
    /*
      0x02 - center colon (both dots)
      0x04 - left colon - lower dot
      0x08 - left colon - upper dot
      0x10 - decimal point
      //clockDisplay.writeDigitRaw(2, 0x0C); // draw only left colon
      //clockDisplay.writeDigitRaw(2, 0x08);
    */
    if (numDayDigits != 4) {
      A7seg[0].writeDigitRaw(2, 0x02); // draw colon on segment 1
    }
    
    if (secondsLeft % 2 == 0) {
      //clockDisplay.drawColon(true);
      A7seg[1].writeDigitRaw(2, 0x0E); // draw both colons
    } else {
      //clockDisplay.drawColon(false);
      A7seg[1].writeDigitRaw(2, 0x0C); // draw only left colon
    }

    // gotta update for dots!
    A7seg[0].writeDisplay();
    A7seg[1].writeDisplay();

    delay(delayTime);
  }

  // illuminated switch LED animations
  ledPulsing();

}
