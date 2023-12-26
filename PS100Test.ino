#include <OneWire.h> // 1-Wire protocol library https://github.com/PaulStoffregen/OneWire


//do not display the temperature and soil moisture values on the first loop, but give time to the DS18B20 to calculate the temperature needed to compensate the soil moisture value.
//When the consecutive communication errors are >=3 the values are not displayed. The sensorErr counter is set to zero at the first successfull retrieve of the values.
unsigned long sensorErr=3; 

float temperatureValue;
unsigned int soilMoistureValue;
byte sensorAddress[8];
OneWire oneWire(12); // initialize the OneWire protocol 
  
void setup() {
    Serial.begin(9600);
}

void loop() {
  getTemperatureAndSoilMoisture();
  if (sensorErr<3)
  {
    Serial.print("tmp=");
    Serial.print(temperatureValue);
    Serial.print("\thum=");
    Serial.print(soilMoistureValue);  
    Serial.print(" (");
    Serial.print(((unsigned long)soilMoistureValue * 100) / 65535);  
    Serial.println("%)");
  }
  else
  {
    Serial.println(".");
  }
  delay(2000);
}



void getTemperatureAndSoilMoisture() {
  int16_t r = 0;
  long i = 0;
  byte data[12];
  oneWire.target_search(0x28);
  if (oneWire.search(sensorAddress))
  {
    delay(1);
    if (oneWire.reset()) {
      oneWire.select(sensorAddress);
      oneWire.write(0xBE, 0);
      oneWire.read_bytes(data, 9);
      int c = oneWire.crc8(data, 8);
      if (c == data[8]) {
        r = (data[1] << 8) | data[0];
        byte tmpSign = (data[1] & 0b10000000) + (data[1] & 0b01000000) + (data[1] & 0b00100000) + (data[1] & 0b00010000) + (data[1] & 0b00001000) + (data[1] & 0b00000100);
        r = r * (tmpSign == 0 ? +1 : -1);
        temperatureValue = (float)r / 16;
        i = (long)data[2] * 256 + data[3];
        soilMoistureValue = i;
        sensorErr=0;
      }
      else
      {
        sensorErr++;
      }
    }
    else
    {
      sensorErr++;
    }
  }
  else
  {
    sensorErr++;
  }
}
