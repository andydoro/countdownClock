

// flipflap display animation for digit change
void flipNum(int dispIndex, int digitIndex, int oldNum, int targetNum) {
  // display is counting down, so count up to make more exciting ?
  int i = oldNum;
  while (i != targetNum) {
    A7seg[dispIndex].writeDigitNum(digitMap[digitIndex], i);
    i++;
    // rollover
    if (i > 9) {
      i = 0;
    }
    A7seg[dispIndex].writeDisplay();
    delay(startupDelay);
  }
  A7seg[dispIndex].writeDigitNum(digitMap[digitIndex], targetNum);
  A7seg[dispIndex].writeDisplay();
  delay(numeralDelay); // longer pause between numerals
}

void flipNumSkip(int dispIndex, int digitIndex, int oldNum, int targetNum) {
  // display is counting down, so count up to make more exciting ?
  int i = oldNum;
  if (dispIndex == 1 && digitIndex == 3) {
    //if (i != targetNum) {
    // A7seg[dispIndex].writeDigitNum(digitMap[digitIndex], targetNum); // just write it
    // }
  } else {
    //int i = oldNum;
    while (i != targetNum) {
      A7seg[dispIndex].writeDigitNum(digitMap[digitIndex], i);
      i++;
      // rollover
      if (i > 9) {
        i = 0;
      }
      A7seg[dispIndex].writeDisplay();
      delay(startupDelay);
    }
  }
  A7seg[dispIndex].writeDigitNum(digitMap[digitIndex], targetNum);
  A7seg[dispIndex].writeDisplay();
  delay(numeralDelay); // longer pause between numerals
}

// numbers counting down animation, used in startupSequence and finalSequence
void countDownToZero(int dispIndex, int numIndex) {
  for (int i = 9; i > -1; i--) {
    A7seg[dispIndex].writeDigitNum(numIndex, i);
    A7seg[dispIndex].writeDisplay();
    delay(startupDelay);
  }
}

// clear all displays
void clearAllDisplays(void) {
  A7seg[0].clear();
  A7seg[1].clear();
  A7seg[0].writeDisplay();
  A7seg[1].writeDisplay();
}


// print time to serial
void printTheTime(DateTime theTimeP) {
  Serial.print(theTimeP.year(), DEC);
  Serial.print('/');
  Serial.print(theTimeP.month(), DEC);
  Serial.print('/');
  Serial.print(theTimeP.day(), DEC);
  Serial.print(' ');
  Serial.print(theTimeP.hour(), DEC);
  Serial.print(':');
  Serial.print(theTimeP.minute(), DEC);
  Serial.print(':');
  Serial.print(theTimeP.second(), DEC);
  Serial.println();
}


// illuminated switch LED animations
void ledPulsing(void) {

  ledFade = constrain(ledFade, 0, 255); // keep within bounds

  analogWrite(ledPin, ledFade);

  //Serial.print("ledFade: ");
  //Serial.println(ledFade);

  if (fadeDirection == true) {
    ledFade += fadeAmount;
  } else {
    ledFade -= fadeAmount;
  }
  // if we go over... change directions
  if (ledFade > 255 || ledFade < 0) {
    fadeDirection = !fadeDirection;
  }

}
