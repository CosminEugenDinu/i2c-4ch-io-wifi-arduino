#include <Wire.h>

#define wireMsgLen 5
// const byte wireDeviceAddr = 1;

// wire (i2c) message received
char wireMsgRec[wireMsgLen];
// sequence of 4 10bit numbers unpacked from wire message
short wireValRec[4];

char serialMsgSent[8];

char printStr[] = "0000";

void setup()
{
  Wire.begin();       // join i2c bus (address optional for master)
  Serial.begin(9600); // start serial for output
}

void loop()
{
  byte msgLen = wireMsgLen;
  byte wireDevices = 2;
  while (wireDevices) {
    Serial.write("dev");
    Serial.write(wireDevices+'0');
    Serial.write(':');
    // delay(2000);
    Wire.requestFrom(wireDevices--, msgLen);
    byte i = 0;
    while (Wire.available())
    {                       // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      wireMsgRec[i++] = c;
    }
    delay(10);
    unpackWireMsg(wireValRec, wireMsgRec);
    for (byte i = 0; i < 4; i++) {
      numToStr(printStr, wireValRec[i], 4);
      Serial.write(i+'0');
      Serial.write(':');
      Serial.write(printStr);
      Serial.write(',');
    }

  } 
  Serial.println("");
  // if (Serial.available() > 0) {
  //   Serial.readBytes(serialReceived, wireMessageLen);
  // }

  // Wire.beginTransmission(wireDeviceAddr);
  // Wire.write(message);
  // Wire.write(serialReceived);
  // Wire.endTransmission();
  delay(500);
  // delay(10);
}

/************ library ***********************/
void numToStr(char *digits, int num, int len)
{
  char digit = 0;
  char zeroAscii = 48;
  int k_digit = len;
  // digits[len] = '\0'; // stream terminator character
  while (k_digit)
  {
    int exp = len - k_digit + 1;
    int base = 10;
    int pow = 1;
    while (exp)
    {
      pow *= base;
      exp -= 1;
    }
    digit = (num % pow - digit) / (pow / base);
    digits[k_digit - 1] = digit + zeroAscii;
    k_digit -= 1;
  }
}

void unpackWireMsg(short wireVals[4], char wireMsg[5])
{
  wireVals[0] = ((wireMsg[0] & 0xff) << 2) | (wireMsg[1] >> 6) & 0x3;
  wireVals[1] = ((wireMsg[1] & 0x3f) << 4) | (wireMsg[2] >> 4) & 0xf;
  wireVals[2] = ((wireMsg[2] & 0xf) << 6) | (wireMsg[3] >> 2) & 0x3f;
  wireVals[3] = ((wireMsg[3] & 0x3) << 8) & 0xff | wireMsg[4];

  // change from signed to unsigned numbers
  for (char i = 0; i < 4; i++)
  {
    wireVals[i] = wireVals[i] < 0 ? 1024 + wireVals[i] : wireVals[i];
  }
}

void packWireMsg(char wireMsg[5], short wireVals[4])
{
  wireMsg[0] = wireVals[0] >> 2;
  wireMsg[1] = (wireVals[0] << 6) | (wireVals[1] >> 4);
  wireMsg[2] = (wireVals[1] << 4) | (wireVals[2] >> 6);
  wireMsg[3] = (wireVals[2] << 2) | (wireVals[3] >> 8);
  wireMsg[4] = wireVals[3];
}

/************ library END *******************/