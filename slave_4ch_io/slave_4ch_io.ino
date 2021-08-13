#include <Wire.h>
#include <SPI.h>

void digitalPotWrite(int potAddr, int value);

const byte wireThisAddr = 11; // addr 0 - 7 are reserved; use 0 - 127

#define wireMsgLen 8  // 8 bytes 
const byte analogValsLen = 4; // corespond to pins 0..3 
const byte buffLen = 10; // number of reads per pin -> medium 
const byte analogPins[analogValsLen] = {A0, A1, A2, A3};
const byte spi_CS_pin10 = 10; // SPI ship select
const byte potAddr = 0x11;    // digital potentiometer address

//wire (I2C) message to send
byte wireMsg[wireMsgLen] = {0};
short analogBuffers[analogValsLen][buffLen]; 
short analogVals[analogValsLen] = {0}; // will contain average of values from analogBuffers

void setup()
{
  // pinMode(spi_CS_pin10, OUTPUT);
  // SPI.begin();

  Serial.begin(9600);

  Wire.begin(wireThisAddr);     // join i2c bus with this device address
  Wire.onRequest(wireReqEvent); // register event
  // // Wire.onReceive(wireRecEvent);

  // // adjust Highest Resistance .
  // digitalPotWrite(potAddr, 0x00);
  // delay(1000);

  // adjust  wiper in the  Mid point  .
  // digitalPotWrite(potAddr, 0x80);
  // delay(1000);

  // adjust Lowest Resistance .
  // digitalPotWrite(potAddr, 0xFF);
  // delay(1000);

}

// void wireReqEvent()
// {
//   Serial.println("reqest received");
//   byte msg[4] = {'a','b','c','d'};
//   Wire.write(msg, 4);
// }

// void loop() 
// {

// }


void wireReqEvent()
{
  // calculate average
  for (int i = 0; i < analogValsLen; i++) {
    int sum = 0;
    for (int j = 0; j < buffLen; j++) {
      sum += analogBuffers[i][j];
    }
    analogVals[i] = sum / buffLen;
  }

  // split each short numbers (2 bytes) in 2 parts and store in wireVals
  wireMsg[0] = (analogVals[0] >> 8) & 0xFF;
  wireMsg[1] = analogVals[0] & 0xFF;

  wireMsg[2] = (analogVals[1] >> 8) & 0xFF;
  wireMsg[3] = analogVals[1] & 0xFF;

  wireMsg[4] = (analogVals[2] >> 8) & 0xFF;
  wireMsg[5] = analogVals[2] & 0xFF;

  wireMsg[6] = (analogVals[3] >> 8) & 0xFF;
  wireMsg[7] = analogVals[3] & 0xFF;
  
  Wire.write(wireMsg, wireMsgLen);
}

void loop()
{
  // analogRead -> int ranging from 0 to 1023 (0V-5V)
  for(int i = 0; i < analogValsLen; i++) {
    for (int j = 0; j < buffLen; j++) {
      analogBuffers[i][j] = (short)analogRead(analogPins[i]);
    }
  }

  delay(10);
}




// void wireRecEvent(int howMany)
// {
//   for (int i = 0; i < wireMsgLen; i++)
//   {
//     char c = Wire.read();
//     wireMsgRec[i] = c;
//   }

//   unpack_5c4n(wireMsgRec, wireValRec);
//   const byte potResVal = (byte)wireValRec[0];

//   Serial.println("wireRecEvent Triggered");
//   char info1[] = " cb-dev8-wireMsgRec[i]:";
//   for (byte i = 0; i< 5; i++) {
//     info1[20] = i + '0';
//     Serial.print(info1);
//     char * binptr = bin(wireMsgRec[i], 8);
//     Serial.print(binptr);
//     free(binptr);
//   }
//   Serial.println();

//   char info2[] = " cb-dev8-wireValRec[i]:";
//   for (byte i = 0; i< 1; i++) {
//     info2[20] = i + '0';
//     Serial.print(info2);
//     Serial.print(wireValRec[i], DEC);
//   }
//   Serial.println();

//   digitalPotWrite(potAddr, potResVal);
//   delay(10);
// }


/************ library ***********************/

void digitalPotWrite(byte potAddr, int value)
{
  // take the SS pin low to select the chip:
  digitalWrite(spi_CS_pin10, LOW);
  delay(100);
  //  send in the potAddr and value via SPI:
  SPI.transfer(potAddr);
  SPI.transfer(value);
  delay(100);
  // take the SS pin high to de-select the chip:
  digitalWrite(spi_CS_pin10, HIGH);
}


/************ library END *******************/