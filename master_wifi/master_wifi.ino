#include <Wire.h>

void unpack_5c4n(char c8b[5], short n10b[4]);
void pack_4n5c(short n10b[4], char c8b[5]);

#define wireMsgLen 5 // represent five 8bit numbers
#define wireValLen 4 // represent four 10bit numbers
const byte wireDevices = 2;

//wire (I2C) message received
char wireMsgRec[wireMsgLen];
//wire (I2C) message to send
char wireMsgSent[wireMsgLen] = {0};

// sequence of 4 10bit numbers received over i2c
short wireValRec[wireValLen];
// sequence of 4 10bit numbers to send over i2c
short wireValSent[wireValLen] = {0};

void setup()
{
  Wire.begin();       // join i2c bus (address optional for master)
  Serial.begin(9600); // start serial for output
}

void loop()
{
  byte msgLen = wireMsgLen;
  // wire addresses 0 - 7 are reserved; first usable addr is 8
  byte wireDevAddr = wireDevices + 7;
  while (7 < wireDevAddr)
  {
    Serial.write("dev");
    Serial.write(wireDevAddr + '0');
    Serial.write(':');
    // delay(2000);
    Wire.requestFrom(wireDevAddr--, msgLen);
    byte i = 0;
    while (Wire.available())
    {                       // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      wireMsgRec[i++] = c;
    }
    delay(10);
    unpack_5c4n(wireMsgRec, wireValRec);
    for (byte i = 0; i < 4; i++)
    {
      Serial.write(i + '0');
      Serial.write(':');
      Serial.print(wireValRec[i], DEC);
      Serial.write(',');
    }
  }
  Serial.println("");
  String serialInput = "";
  if (Serial.available() > 0)
  {
    // Serial.readBytes(serialReceived, wireMessageLen);
    serialInput = Serial.readString();
  }
  int numRec = serialInput.toInt();
  if (0 < numRec && numRec < 256)
  {
    wireValSent[0] = (short)numRec;
    pack_4n5c(wireValSent, wireMsgSent);
    Wire.beginTransmission(8);
    Wire.write(wireMsgSent, msgLen);
    Wire.endTransmission();
    delay(100);
  }
  else if (numRec != 0)
  {
    Serial.print("Error: Pontentiometer value range 0-255: ");
    Serial.print(numRec, DEC);
    Serial.println();
  }
  delay(500);
  // delay(10);
}

/************ library ***********************/

void unpack_5c4n(char c8b[5], short n10b[4])
{

  n10b[0] = ((c8b[0] & 0xff) << 2) | (c8b[1] >> 6) & 0x3;
  n10b[1] = ((c8b[1] & 0x3f) << 4) | (c8b[2] >> 4) & 0xf;
  n10b[2] = ((c8b[2] & 0xf) << 6) | (c8b[3] >> 2) & 0x3f;
  n10b[3] = ((c8b[3] & 0x3) << 8) | c8b[4] & 0xff;

  // change from signed to unsigned numbers
  for (int i = 0; i < 4; i++)
  {
    n10b[i] = (n10b[i] < 0) ? 1024 + n10b[i] : n10b[i];
  }
}

void pack_4n5c(short n10b[4], char c8b[5])
{
  c8b[0] = n10b[0] >> 2;
  c8b[1] = (n10b[0] << 6) | (n10b[1] >> 4);
  c8b[2] = (n10b[1] << 4) | (n10b[2] >> 6);
  c8b[3] = (n10b[2] << 2) | (n10b[3] >> 8);
  c8b[4] = n10b[3];

  // change from signed to unsigned numbers
  for (int i = 0; i < 4; i++)
  {
    c8b[i] = (c8b[i] < 0) ? 256 + c8b[i] : c8b[i];
  }
}

/************ library END *******************/