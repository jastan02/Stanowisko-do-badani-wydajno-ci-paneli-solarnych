#include <Wire.h>
#include <INA3221.h>
#include "DHT.h"
#include <BH1750.h>
#include <RTClib.h> 
#include <DallasTemperature.h>
#include <OneWire.h>

#define INA3221_ADDRESS INA3221_ADDR40_GND 

#define DHTPIN 23 
#define DHTTYPE DHT11

INA3221 ina3221(INA3221_ADDRESS);
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter(0x23);
RTC_DS3231 rtc; 
const int oneWireBus1 = 14;
const int oneWireBus2 = 27;
const int oneWireBus3 = 26;
OneWire oneWire1(oneWireBus1);
OneWire oneWire2(oneWireBus2);
OneWire oneWire3(oneWireBus3);
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);

#define CHANNEL_1 INA3221_CH1 
#define CHANNEL_2 INA3221_CH2 
#define CHANNEL_3 INA3221_CH3 

void setup() {
  Serial.begin(115200);

  Wire.begin();
  ina3221.begin(&Wire);
  dht.begin(); 
  lightMeter.begin();
  sensor1.begin();
  sensor2.begin();
  sensor3.begin();
  
  // Inicjalizacja RTC
  if (!rtc.begin()) {
    Serial.println("RTC nie jest podłączony!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC stracił zasilanie, ustawiamy czas...");
    // Ustawienie czasu i daty na przykład na kompilację (zakomentuj, jeśli chcesz ręcznie ustawić czas)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // Odczytanie wartości napięcia i prądu z kanałów INA3221
  float busVoltage1 = ina3221.getVoltage(CHANNEL_1);
  int32_t shuntVoltage1 = ina3221.getShuntVoltage(CHANNEL_1);
  float current1_mA = ina3221.getCurrent(CHANNEL_1);

  float busVoltage2 = ina3221.getVoltage(CHANNEL_2);
  int32_t shuntVoltage2 = ina3221.getShuntVoltage(CHANNEL_2);
  float current2_mA = ina3221.getCurrent(CHANNEL_2);

  float busVoltage3 = ina3221.getVoltage(CHANNEL_3);
  int32_t shuntVoltage3 = ina3221.getShuntVoltage(CHANNEL_3);
  float current3_mA = ina3221.getCurrent(CHANNEL_3);

  //--------------------------------------------------------
  // Wyświetlanie wyników dla każdego kanału
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

  //--------------------------------------------------------
  // Wyświetlanie danych z DHT11
  Serial.print("Wilgotność: ");
  Serial.print((float)dht.readHumidity());
  Serial.println("%");
  Serial.print("Temperatura: ");
  Serial.print((float)dht.readTemperature());
  Serial.println("°C");

  // Wyświetlanie danych z czujnika natężenia światła
  Serial.print("Natezenie swiatla: ");
  Serial.print(lightMeter.readLightLevel());
  Serial.println("Lux");

  //--------------------------------------------------------
  // Odczytanie i wyświetlenie aktualnej daty i godziny z RTC
  DateTime now = rtc.now();
  
  Serial.print("Data: ");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" Godzina: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);


  //--------------------------------------------------------
  //temperatura paneli solarnych

  sensor1.requestTemperatures();
  float temperatureC1 = sensor1.getTempCByIndex(0);
  Serial.print("Temp: ");
  Serial.print(temperatureC1);
  Serial.println("°C");

  sensor2.requestTemperatures();
  float temperatureC2 = sensor2.getTempCByIndex(0);
  Serial.print("Temp: ");
  Serial.print(temperatureC2);
  Serial.println("°C");

  sensor3.requestTemperatures();
  float temperatureC3 = sensor3.getTempCByIndex(0);
  Serial.print("Temp: ");
  Serial.print(temperatureC3);
  Serial.println("°C");

  

  // Krótkie opóźnienie przed ponownym odczytem
  delay(1000);
}
