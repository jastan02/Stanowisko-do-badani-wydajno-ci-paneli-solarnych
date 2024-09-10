#include <Wire.h>
#include <INA3221.h>
#include "DHT.h"
#include <BH1750.h>


#define INA3221_ADDRESS INA3221_ADDR40_GND 

#define DHTPIN 23 
#define DHTTYPE DHT11


INA3221 ina3221(INA3221_ADDRESS);
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter(0x23);


#define CHANNEL_1 INA3221_CH1 
#define CHANNEL_2 INA3221_CH2 
#define CHANNEL_3 INA3221_CH3 

void setup() {

  Serial.begin(115200);


  Wire.begin();
  ina3221.begin(&Wire);
  dht.begin(); 
  lightMeter.begin();

  
}

void loop() {
  float busVoltage1 = ina3221.getVoltage(CHANNEL_1);
  int32_t shuntVoltage1 = ina3221.getShuntVoltage(CHANNEL_1);
  float current1_mA = ina3221.getCurrent(CHANNEL_1);

  float busVoltage2 = ina3221.getVoltage(CHANNEL_2);
  int32_t shuntVoltage2 = ina3221.getShuntVoltage(CHANNEL_2);
  float current2_mA = ina3221.getCurrent(CHANNEL_2);

  float busVoltage3 = ina3221.getVoltage(CHANNEL_3);
  int32_t shuntVoltage3 = ina3221.getShuntVoltage(CHANNEL_3);
  float current3_mA = ina3221.getCurrent(CHANNEL_3);


  Serial.print("Channel 1 Bus Voltage: ");
  Serial.print(busVoltage1);
  Serial.println(" V");

  Serial.print("Channel 1 Shunt Voltage: ");
  Serial.print(shuntVoltage1);
  Serial.println(" mV");

  Serial.print("Channel 1 Current: ");
  Serial.print(current1_mA);
  Serial.println(" mA");

  Serial.println("------------------------------------"); 

  Serial.print("Channel 2 Bus Voltage: ");
  Serial.print(busVoltage2);
  Serial.println(" V");

  Serial.print("Channel 2 Shunt Voltage: ");
  Serial.print(shuntVoltage2);
  Serial.println(" mV");

  Serial.print("Channel 2 Current: ");
  Serial.print(current2_mA);
  Serial.println(" mA");

  Serial.println("------------------------------------");

  Serial.print("Channel 3 Bus Voltage: ");
  Serial.print(busVoltage3);
  Serial.println(" V");

  Serial.print("Channel 3 Shunt Voltage: ");
  Serial.print(shuntVoltage3);
  Serial.println(" mV");

  Serial.print("Channel 3 Current: ");
  Serial.print(current3_mA);
  Serial.println(" mA");

  Serial.println("------------------------------------");

  Serial.println((float)dht.readHumidity());
  Serial.println((float)dht.readTemperature());
  Serial.println(lightMeter.readLightLevel());

  delay(1000);
}
