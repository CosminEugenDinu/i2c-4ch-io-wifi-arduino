#include <Wire.h>
#include <string.h>

const byte wireDevices = 4; // number of connected wire devices
const byte firstDeviceAddr = 7;
#define wireMsgLen 8 // represent five 8bit numbers
const byte wireValLen = 4; // represent four numbers

//wire (I2C) message to send
char wireMsgSent[wireMsgLen] = {0};
// sequence of 4 10bit numbers to send over i2c
short wireValSent[wireValLen];

//wire (I2C) message received
byte wireMsgRec[wireMsgLen] = {0};
short wireValsRec[wireValLen];


void setup()
{
  Wire.begin();       // join i2c bus (address optional for master)
  Serial.begin(9600); // start serial for output
}

// void loop()
// {
//   const byte msgLen = wireMsgLen;
//   Wire.requestFrom(8, 4);
//   byte msg[4];
//   byte i = 0;
//   while (i < 4)
//   {                       // slave may send less than requested
//     byte c = Wire.read(); // receive a byte as character
//     msg[i++] = c;
//     Serial.print(msg[i-1]);
//   }
//   Serial.println();
//   delay(100);
// }

void loop()
{
  const byte msgLen = wireMsgLen;
  String responseStr = String("[");

  // wire addresses 0 - 7 are reserved; first usable addr is 8
  byte wireDevAddr = wireDevices + firstDeviceAddr;
  while (firstDeviceAddr < wireDevAddr)
  {
    Wire.requestFrom(wireDevAddr--, msgLen);
    byte i = 0;
    while (i < msgLen)
    {                       // slave may send less than requested
      byte c = Wire.read(); // receive a byte as character
      wireMsgRec[i++] = c;
    }

    // merge 2 by 2 bytes in one short number
    wireValsRec[0] = wireMsgRec[0];
    wireValsRec[0] = (wireValsRec[0] << 8) | wireMsgRec[1];

    wireValsRec[1] = wireMsgRec[2];
    wireValsRec[1] = (wireValsRec[1] << 8) | wireMsgRec[3];
    
    wireValsRec[2] = wireMsgRec[4];
    wireValsRec[2] = (wireValsRec[2] << 8) | wireMsgRec[5];

    wireValsRec[3] = wireMsgRec[6];
    wireValsRec[3] = (wireValsRec[3] << 8) | wireMsgRec[7];

    // delay(100);
    responseStr.concat('[');
    for (byte i = 0; i < wireValLen; i++)
    {
      responseStr.concat(String(wireValsRec[i], DEC));
      if (i < wireValLen - 1) {
        responseStr.concat(',');
      }
    }
    responseStr.concat("]");
    if (firstDeviceAddr < wireDevAddr) {
      responseStr.concat(',');
    }

  }
  responseStr.concat("]\n");
  Serial.print(responseStr);



  // String serialInput = "";
  // if (Serial.available() > 0)
  // {
  //   // Serial.readBytes(serialReceived, wireMessageLen);
  //   serialInput = Serial.readString();
  // }
  // int numRec = serialInput.toInt();
  // if (0 < numRec && numRec < 256)
  // {
  //   wireValSent[0] = (short)numRec;
  //   pack_4n5c(wireValSent, wireMsgSent);
  //   Wire.beginTransmission(8);
  //   Wire.write(wireMsgSent, msgLen);
  //   Wire.endTransmission();
  //   delay(100);
  // }
  // else if (numRec != 0)
  // {
  //   Serial.print("Error: Pontentiometer value range 0-255: ");
  //   Serial.print(numRec, DEC);
  //   Serial.println();
  // }

  delay(10);
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