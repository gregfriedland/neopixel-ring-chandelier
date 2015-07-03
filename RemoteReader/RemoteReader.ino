// Wait for input from an IR remote and send the signal on over I2C

#include <IRremote.h>
#include <Wire.h>

int RECV_PIN = 10;

IRrecv irrecv(RECV_PIN);

decode_results results;

// from http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
byte CRC8(const byte *data, byte len) {
  byte crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  Wire.begin(); // join i2c bus (address optional for master)  
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    
    // compress the key code into one byte
    byte data[4] = {results.value & 0xFF000000, results.value & 0xFF0000, results.value & 0xFF00, results.value & 0xFF};
    byte crc = CRC8(data, 4);
    
    Wire.beginTransmission(4); // transmit to device #4
    Serial.println(crc, HEX);
    Wire.write(crc);
    Wire.endTransmission();    // stop transmitting
    irrecv.resume(); // Receive the next value
  }
}
