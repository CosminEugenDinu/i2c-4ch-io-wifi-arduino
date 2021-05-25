#include <Wire.h>
#include <SPI.h>

void numToStr(char *digits, int num, int len);
void digitalPotWrite(int potAddr, int value);
void unpackWireMsg(short wireVals[4], char wireMsg[5]);
void packWireMsg(char wireMsg[5], short wireVals[4]);


#define analog_pin0 0
#define analog_pin1 1
#define analog_pin2 2
#define analog_pin3 3
#define spi_CS_pin10 10 // SPI chip select
#define wireMsgLen 5
#define wireThisAddr 1
const byte potAddr = 0x11;


//wire (I2C) message received
char wireMsgRec[5];
//wire (I2C) message to send
char wireMsgSent[5];

// sequence of 4 10bit numbers received over i2c
short wireValRec[4];
// sequence of 4 10bit numbers to send over i2c 
short wireValSent[4];

void setup()
{
  // Serial.begin(9600);
  pinMode(spi_CS_pin10, OUTPUT);
  SPI.begin();

  // // adjust Highest Resistance .
  // digitalPotWrite(potAddr, 0x00);
  // delay(1000);

  // // adjust  wiper in the  Mid point  .
  // digitalPotWrite(potAddr, 0x80);
  // delay(1000);

  // // adjust Lowest Resistance .
  // digitalPotWrite(potAddr, 0xFF);
  // delay(1000);

  Wire.begin(wireThisAddr); // join i2c bus with this device address

  Wire.onRequest(requestEvent); // register event
  // Wire.onReceive(receiveEvent);
}

void receiveEvent(int howMany)
{
  // int i = 0;
  while (1 < Wire.available())
  {
    for (int i = 0; i < howMany; i++)
    {
      char c = Wire.read();
      wireMsgRec[i] = c;
    }
    break;
  }
}

void requestEvent()
{
  /*
  function that executes whenever data is requested by master
  this function is registered as an event.
  */
  // analogRead -> int ranging from 0 to 1023 (0V-5V)
  short a0 = (short)analogRead(analog_pin0);
  short a1 = (short)analogRead(analog_pin1);
  short a2 = (short)analogRead(analog_pin2);
  short a3 = (short)analogRead(analog_pin3);
  delay(10);
  wireValSent[0] = a0;
  wireValSent[1] = a1;
  wireValSent[2] = a2;
  wireValSent[3] = a3;
  packWireMsg(wireMsgSent, wireValSent);
  Wire.write(wireMsgSent);
  // delay(10);
}

void loop()
{
  delay(10);

  // for (int level = 0; level <= 255; level++) {
  //   digitalPotWrite(potAddr, level);
  //   delay(10);
  // }
  // delay(500);
  // for (int level = 255; level >= 0; level--) {
  //   digitalPotWrite(potAddr, level);
  //   delay(10);
  // }

  // go through the six channels of the digital pot:
  // for (int channel = 0; channel < 6; channel++) {
  //   // change the resistance on this channel from min to max:
  //   for (int level = 0; level < 255; level++) {
  //     digitalPotWrite(channel, level);
  //     delay(10);
  //   }
  //   // wait a second at the top:
  //   delay(100);
  //   // change the resistance on this channel from max to min:
  //   for (int level = 0; level < 255; level++) {
  //     digitalPotWrite(channel, 255 - level);
  //     delay(10);
  //   }
  // }
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

void unpackWireMsg(short wireVals[4], char wireMsg[5])
{
  wireVals[0] = (wireMsg[0] << 2) | (wireMsg[1] >> 6);
  wireVals[1] = ((wireMsg[1] & 0x3f) << 4) | (wireMsg[2] >> 4);
  wireVals[2] = ((wireMsg[2] & 0xf) << 6) | (wireMsg[3] >> 2);
  wireVals[3] = ((wireMsg[3] & 0x3) << 8) | wireMsg[4];
  
  // change from signed to unsigned numbers
  for (int i = 0; i < 4; i++)
  {
    wireVals[i] = (wireVals[i] < 0) ? 1024 + wireVals[i] : wireVals[i];
  }
}

void packWireMsg(char wireMsg[5], short wireVals[4])
{
  wireMsg[0] = (wireVals[0] >> 2)&0xff;
  wireMsg[1] = 0;
  wireMsg[1] = ((wireVals[0] & 0x3) << 6) | ((wireVals[1] >> 4) & 0x3f);
  wireMsg[2] = ((wireVals[1] & 0xf) << 4) | ((wireVals[2] >> 6) & 0xf);
  wireMsg[3] = ((wireVals[2] & 0x3f) << 2) | (wireVals[3] >> 8 );
  wireMsg[4] = wireVals[3] & 0xff;

  // change from signed to unsigned numbers
  for (int i = 0; i < 4; i++)
  {
    wireMsg[i] = (wireMsg[i] < 0) ? 256 + wireMsg[i] : wireMsg[i];
  //   // wireMsg[i] = wireMsg[i] & 0xff;
  }

}

/************ library END *******************/