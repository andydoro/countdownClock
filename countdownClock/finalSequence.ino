
void finalSequence(void) {

  // needs some work, char flips, etc.
  // have everything go to 0!

  Serial.println("final sequence - zeros, bye.truMP");
  clearAllDisplays();

  // make pulsing switch faster!
  fadeAmount = fastFadeAmount;
  ledPulsing();

  //countdown all to zeros!

  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 5; i++) {
      // skip 2!
      if (i != 2) {
        countDownToZero(j, i);
      }
    }
  }

  delay(1000);   // ponder static zeros

  // blink all dem zeros
  A7seg[0].blinkRate(1); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(1); // set to 0 for no blink, 1,2,3 for blinking

  delay(2000);

  //ok stop blinking zeros and write something!
  // bye.truMP
  // blink all dem zeros
  A7seg[0].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking

  clearAllDisplays();
  delay(2000);

  // countdown each one of these
  // b
  countDownToZero(0, 0);
  A7seg[0].writeDigitRaw(0, B01111100);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  // Y
  countDownToZero(0, 1);
  A7seg[0].writeDigitRaw(1, B01101110);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  // E
  countDownToZero(0, 3);
  A7seg[0].writeDigitNum(3, 0xE);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  // dot?
  // 0x10 - decimal point
  A7seg[0].writeDigitRaw(2, 0x10);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  // t - lowercase
  countDownToZero(0, 4);
  A7seg[0].writeDigitRaw(4, B01111000);
  A7seg[0].writeDisplay();
  delay(numeralDelay);


  // r - lowercase
  countDownToZero(1, 0);
  A7seg[1].writeDigitRaw(0, B01010000);
  A7seg[1].writeDisplay();
  delay(numeralDelay);

  // u - lowercase
  countDownToZero(1, 1);
  A7seg[1].writeDigitRaw(1, B00011100);
  A7seg[1].writeDisplay();
  delay(numeralDelay);

  // M
  countDownToZero(1, 3);
  A7seg[1].writeDigitRaw(3, LETTER_M);
  A7seg[1].writeDisplay();
  delay(numeralDelay);


  // P
  countDownToZero(1, 4);
  A7seg[1].writeDigitRaw(4, B01110011);
  A7seg[1].writeDisplay();
  delay(numeralDelay);

  A7seg[0].writeDisplay();
  A7seg[1].writeDisplay();
  delay(2000);

  A7seg[0].blinkRate(1); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(1); // set to 0 for no blink, 1,2,3 for blinking
  delay(5000);

  clearAllDisplays();

  // stop blinking again.
  A7seg[0].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking

  // reset fade amount in case we exit ?
  fadeAmount = normalFadeAmount;

  delay(1000);

}
