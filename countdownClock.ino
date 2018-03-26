/* Trump Countdown Clock

   displays DAYS, HOURS, MINUTES on 8 digits
   DDDD : HH : MM

   HARDWARE:
     Metro Mini https://www.adafruit.com/product/2590
     7-Segment Display w/I2C Backpack https://www.adafruit.com/product/1270 x 2
     DS1307 RTC https://www.adafruit.com/product/264
     Illuminated Toggle Switch w/ Cover https://www.adafruit.com/product/3218


  TO DO:
    - have illuminated switch flash faster when trumpIsOver, code is blocking

  DONE:
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

// turn into array
Adafruit_7segment A7seg[2] = {Adafruit_7segment(), Adafruit_7segment()};

/*
  Adafruit_7segment daysDisplay = Adafruit_7segment();
  Adafruit_7segment clockDisplay = Adafruit_7segment(); // for hours and minutes
*/

// unix target date
// countdown until:
// January 20, 2021, 9:00:00 am
// using: https://www.epochconverter.com/
// this date is in Standard Time
// verify calculations here:
// https://www.timeanddate.com/date/durationresult.html
const unsigned long trumpOver = 1611151200;

#define GMT_OFFSET -5 // Eastern Standard Time is GMT-5

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

const int BLINKRATE = 2; // 0 is no blink, 1-3 are all acceptable.

const int brightness = 10; // 0-15 sets brightness
// change based on time of day?

#define NUMDIGITS 10 // our digit array size. extra two for "2"s on our display, which are the dots

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

  Serial.begin(9600);

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

  // not working properly?
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

    DateTime theTime = rtc.now();

    // Serial print out
    // RTC Time - Standard Time
    Serial.println("RTC time: ");
    printTheTime(theTime);

    // take into account TimeZone
    theTime = theTime.unixtime() - (GMT_OFFSET * SECONDS_PER_HOUR);

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

    int daysLeft, hoursLeft, minutesLeft; // create vars

    // calculate days, hours, minutes left
    daysLeft = secondsLeft / SECONDS_PER_DAY;
    hoursLeft = (secondsLeft % SECONDS_PER_DAY) / SECONDS_PER_HOUR; // throw away anything over a full day, divide by number of seconds in an hour
    minutesLeft = (secondsLeft % SECONDS_PER_HOUR) / 60; // throw away anything over a full hour, divide by 60 (seconds per minute)

    // print our calculations of time left
    Serial.print("days left: ");
    Serial.println(daysLeft);
    Serial.print("hours left: ");
    Serial.println(hoursLeft);
    Serial.print("minutes left: ");
    Serial.println(minutesLeft);

    // get our digits
    clockDigits[0] = daysLeft / 1000;
    clockDigits[1] = (daysLeft % 1000) / 100;

    clockDigits[3] = (daysLeft % 100) / 10;
    clockDigits[4] = daysLeft % 10;
    clockDigits[5] = hoursLeft / 10;
    clockDigits[6] = hoursLeft % 10;

    clockDigits[8] = minutesLeft / 10;
    clockDigits[9] = minutesLeft % 10;

    // compare digits, have they changed?
    // we have to skip 2s because that's the dots...

    // figure out leading zeros for daysLeft, we're skipping those
    int startingDigit = 0;
    if (daysLeft < 10) {
      startingDigit = 4;
      // clear leading zeros if we drop below
      A7seg[0].writeDigitRaw(0, B00000000);
      A7seg[0].writeDigitRaw(1, B00000000);
      A7seg[0].writeDigitRaw(3, B00000000);
    } else if (daysLeft < 100) {
      startingDigit = 3;
      // clear leading zeros if we drop below
      A7seg[0].writeDigitRaw(0, B00000000);
      A7seg[0].writeDigitRaw(1, B00000000);
    } else if (daysLeft < 1000) {
      startingDigit = 1;
      // clear leading zeros if we drop below
      A7seg[0].writeDigitRaw(0, B00000000);
    }

    // display digits
    for (int i = startingDigit; i < NUMDIGITS; i++) {
      if ((i != 2) && (i != 7)) { // gotta skip 2s!
        if ((clockDigits[i] != lastClockDigits[i]) || (firstTime == true)) { // skips 0s first round...
          flipNum(i / 5, i % 5, lastClockDigits[i], clockDigits[i]);
          lastClockDigits[i] = clockDigits[i];
        }
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
    if (secondsLeft % 2 == 0) {
      //clockDisplay.drawColon(true);
      A7seg[1].writeDigitRaw(2, 0x0E); // draw both colon
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


