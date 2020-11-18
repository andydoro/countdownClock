
void startupSequence(void) {


  Serial.println("startup sequence - TIME:LEFT");
  // have each digit count down, then display letter
  // write "time left"

  /*
    for (int i = 9; i > -1; i--) {
      A7seg[0].writeDigitNum(0, i);
      A7seg[0].writeDisplay();
      delay(startupDelay);
    }
  */
  countDownToZero(0, 0);
  // T
  // I think I can use a 7 here...
  //A7seg[0].writeDigitRaw(0, B00000111);
  A7seg[0].writeDigitNum(0, 7);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  countDownToZero(0, 1);
  // I
  A7seg[0].writeDigitNum(1, 1); // 1 looks like I
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  countDownToZero(0, 3);
  // M
  A7seg[0].writeDigitRaw(3, LETTER_M);
  A7seg[0].writeDisplay();
  delay(numeralDelay);

  countDownToZero(0, 4);
  // E
  A7seg[0].writeDigitNum(4, 0xE);
  A7seg[0].writeDisplay(); // gotta call this so it shows up!
  delay(numeralDelay);


  // A7seg[1]
  countDownToZero(1, 0);
  // L
  A7seg[1].writeDigitRaw(0, B00111000);
  A7seg[1].writeDisplay();
  delay(numeralDelay);


  countDownToZero(1, 1);
  // E
  A7seg[1].writeDigitNum(1, 0xE);
  A7seg[1].writeDisplay();
  delay(numeralDelay);


  countDownToZero(1, 3);
  // F
  A7seg[1].writeDigitNum(3, 0xF);
  A7seg[1].writeDisplay();
  delay(numeralDelay);

  countDownToZero(1, 4);
  // T
  //A7seg[1].writeDigitRaw(4, B00000111);
  A7seg[1].writeDigitNum(4, 7);
  A7seg[1].writeDisplay();
  delay(numeralDelay);

  //A7seg[0].writeDisplay();
  //A7seg[1].writeDisplay();
  //delay(1000);

  A7seg[1].writeDigitRaw(2, 0x0C); // draw only left colon on 2nd display (in the middle of two displays);
  A7seg[1].writeDisplay();
  delay(1000);

  // the two displays weirdly go out of sync...
  A7seg[0].blinkRate(BLINKRATE); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(BLINKRATE); // set to 0 for no blink, 1,2,3 for blinking
  delay(3000);

  clearAllDisplays();

  // stop blinking again.
  A7seg[0].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking
  A7seg[1].blinkRate(0); // set to 0 for no blink, 1,2,3 for blinking

  delay(1000);
}
