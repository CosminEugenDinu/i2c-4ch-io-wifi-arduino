#include <Wire.h>
void numToStr(char* digits, int num, int len);

void setup() {
  Serial.begin(9600);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()

void requestEvent() {
  char buffLen = 6;
  int buffer[buffLen];
  // Wire.write(buff); // respond with message of 6 bytes
  int num = analogRead(A2);
  char digits[12];
  numToStr(digits, num, 12);
  Wire.write(digits);
}


void numToStr(char* digits, int num, int len) {
    int digit = 0;
    int zeroAscii = 48;
    int k_digit = len;
    // digits[len] = '\0'; // stream terminator character
    while (k_digit) {
        int exp = len - k_digit + 1;
        int base = 10;
        int pow = 1;
        while (exp) {
            pow *= base;
            exp -= 1;
        }
        digit = (num % pow - digit) / (pow / base);
        digits[k_digit-1] = digit + zeroAscii;
        k_digit -= 1;
    }
}
