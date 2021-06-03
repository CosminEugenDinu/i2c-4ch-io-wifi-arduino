#include <Wire.h>
#include <SPI.h>

void numToStr(char *digits, int num, int len);
void digitalPotWrite(int potAddr, int value);
void unpack_5c4n(char c8b[5], short n10b[4]);
void pack_4n5c(short n10b[4], char c8b[5]);

#define wireMsgLen 5         // represent five 8bit numbers
#define wireValLen 4         // represent four 10bit numbers
const byte wireThisAddr = 8; // addr 0 - 7 are reserved; use 0 - 127
const byte analog_pin0 = 0;
const byte analog_pin1 = 1;
const byte analog_pin2 = 2;
const byte analog_pin3 = 3;
const byte spi_CS_pin10 = 10; // SPI ship select
const byte potAddr = 0x11;    // digital potentiometer address

//wire (I2C) message to send
char wireMsgSent[wireMsgLen];
// sequence of 4 10bit numbers to send over i2c
short wireValSent[wireValLen];

//wire (I2C) message received
char wireMsgRec[wireMsgLen] = {0};
// sequence of 4 10bit numbers received over i2c
short wireValRec[wireValLen] = {0};

void setup()
{
  // Serial.begin(9600);
  pinMode(spi_CS_pin10, OUTPUT);
  SPI.begin();
  Serial.begin(9600);

  // // adjust Highest Resistance .
  // digitalPotWrite(potAddr, 0x00);
  // delay(1000);

  // adjust  wiper in the  Mid point  .
  // digitalPotWrite(potAddr, 0x80);
  // delay(1000);

  // adjust Lowest Resistance .
  // digitalPotWrite(potAddr, 0xFF);
  // delay(1000);

  Wire.begin(wireThisAddr);     // join i2c bus with this device address
  Wire.onRequest(wireReqEvent); // register event
  Wire.onReceive(wireRecEvent);
}

void wireRecEvent(int howMany)
{
  for (int i = 0; i < wireMsgLen; i++)
  {
    char c = Wire.read();
    wireMsgRec[i] = c;
  }

  unpack_5c4n(wireMsgRec, wireValRec);
  const byte potResVal = (byte)wireValRec[0];
  Serial.println("wireRecEvent Triggered");

  char info1[] = " cb-dev8-wireMsgRec[i]:";
  for (byte i = 0; i< 5; i++) {
    info1[20] = i + '0';
    Serial.print(info1);
    char * binptr = bin(wireMsgRec[i], 8);
    Serial.print(binptr);
    free(binptr);
  }
  Serial.println();

  char info2[] = " cb-dev8-wireValRec[i]:";
  for (byte i = 0; i< 1; i++) {
    info2[20] = i + '0';
    Serial.print(info2);
    Serial.print(wireValRec[i], DEC);
  }
  Serial.println();

  digitalPotWrite(potAddr, potResVal);
  delay(10);
}

void wireReqEvent()
{
  /*
  function that executes whenever data is requested by master
  this function is registered as an event.
  */
  pack_4n5c(wireValSent, wireMsgSent);
  Wire.write(wireMsgSent);

  // analogRead -> int ranging from 0 to 1023 (0V-5V)
  wireValSent[0] = (short)analogRead(analog_pin0);
  delay(10);
  wireValSent[1] = (short)analogRead(analog_pin1);
  delay(10);
  wireValSent[2] = (short)analogRead(analog_pin2);
  delay(10);
  wireValSent[3] = (short)analogRead(analog_pin3);
  delay(10);
}

void loop()
{
  // char info2[] = " loop-dev8-wireValRec[i]:";
  // for (byte i = 0; i< 4; i++) {
  //   info2[22] = i + '0';
  //   Serial.print(info2);
  //   Serial.print(wireValRec[i], DEC);
  // }
  // Serial.println();
  delay(100);
}

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

char *bin(int num, int bits)
{
    char *str = (char *)malloc(bits + 1);

    int i;
    for (i = bits - 1; i >= 0; i--)
    {
        if (num & 1)
        {
            *(str + i) = '1';
        }
        else
        {
            *(str + i) = '0';
        }
        num = num >> 1;
    }
    *(str + bits + 1) = '\0';
    return str;
}

/************ library END *******************/