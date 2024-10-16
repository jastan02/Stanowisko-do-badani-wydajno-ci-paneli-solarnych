#include <Wire.h>
#include <INA3221.h>
#include "DHT.h"
#include <BH1750.h>
#include <RTClib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>

// Definicje pinów i adresów
#define INA3221_ADDRESS INA3221_ADDR40_GND
#define DHTPIN 23
#define DHTTYPE DHT11
#define SD_CS_PIN 5  // Pin CS do komunikacji z kartą SD

// Czujniki
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

// Kanały
#define CHANNEL_1 INA3221_CH1
#define CHANNEL_2 INA3221_CH2
#define CHANNEL_3 INA3221_CH3

void logDataToSD(float voltage1, float current1, float voltage2, float current2, float voltage3, float current3, DateTime now) {
  // Otwieramy plik do zapisu
  File dataFile = SD.open("/datalog.csv", FILE_WRITE);

  if (dataFile) {
    // Zapisz dane z kanałów 1, 2, 3 oraz aktualną datę i czas
    dataFile.print("Channel 1 Voltage: ");
    dataFile.print(voltage1);
    dataFile.print(" V, Current: ");
    dataFile.print(current1);
    dataFile.println(" mA");

    dataFile.print("Channel 2 Voltage: ");
    dataFile.print(voltage2);
    dataFile.print(" V, Current: ");
    dataFile.print(current2);
    dataFile.println(" mA");

    dataFile.print("Channel 3 Voltage: ");
    dataFile.print(voltage3);
    dataFile.print(" V, Current: ");
    dataFile.print(current3);
    dataFile.println(" mA");

    // Zapisz datę i czas
    dataFile.print("Date: ");
    dataFile.print(now.day(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.year(), DEC);
    dataFile.print(" Time: ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.println("------------------------------------");

    // Zamykamy plik po zapisie
    dataFile.close();
    Serial.println("Dane zapisane na kartę SD.");
  } else {
    Serial.println("Błąd otwarcia pliku na karcie SD.");
  }
}void logDataToSD(float voltage1, float current1, float voltage2, float current2, float voltage3, float current3, float temp_zew, float hum, float lux, float p1_temp, float p2_temp, float p3_temp ,DateTime now) {
  // Otwieramy plik do zapisu
  File dataFile = SD.open("/datalog.csv", FILE_WRITE);

  if (dataFile) {
    // Zapisz dane z kanałów 1, 2, 3 oraz aktualną datę i czas
    dataFile.print("Channel 1 Voltage: ");
    dataFile.print(voltage1);
    dataFile.print(" V, Current: ");
    dataFile.print(current1);
    dataFile.println(" mA");

    dataFile.print("Channel 2 Voltage: ");
    dataFile.print(voltage2);
    dataFile.print(" V, Current: ");
    dataFile.print(current2);
    dataFile.println(" mA");

    dataFile.print("Channel 3 Voltage: ");
    dataFile.print(voltage3);
    dataFile.print(" V, Current: ");
    dataFile.print(current3);
    dataFile.println(" mA");

    // Zapis danych z DHT11
    dataFile.print("Wilgotność: ");
    dataFile.print((float)dht.readHumidity());
    dataFile.println("%");
    dataFile.print("Temperatura: ");
    dataFile.print((float)dht.readTemperature());
    dataFile.println("°C");

    // Zapis danych z czujnika natężenia światła
    dataFile.print("Natezenie swiatla: ");
    dataFile.print(lightMeter.readLightLevel());
    dataFile.println("Lux");

    //Zapis temperatura paneli solarnych
    sensor1.requestTemperatures();
    float temperatureC1 = sensor1.getTempCByIndex(0);
    dataFile.print("Temp: ");
    dataFile.print(temperatureC1);
    dataFile.println("°C");

    sensor2.requestTemperatures();
    float temperatureC2 = sensor2.getTempCByIndex(0);
    dataFile.print("Temp: ");
    dataFile.print(temperatureC2);
    dataFile.println("°C");

    sensor3.requestTemperatures();
    float temperatureC3 = sensor3.getTempCByIndex(0);
    dataFile.print("Temp: ");
    dataFile.print(temperatureC3);
    dataFile.println("°C");

    // Zapisz datę i czas
    dataFile.print("Date: ");
    dataFile.print(now.day(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.year(), DEC);
    dataFile.print(" Time: ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.println("------------------------------------");

    // Zamykamy plik po zapisie
    dataFile.close();
    Serial.println("Dane zapisane na kartę SD.");
  } else {
    Serial.println("Błąd otwarcia pliku na karcie SD.");
  }
}

void setup() {
  // Rozpoczęcie komunikacji szeregowej
  Serial.begin(115200);

  // Inicjalizacja magistrali I2C
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
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Inicjalizacja karty SD
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Nie można zainicjować karty SD!");
    while (1);
  } else {
    Serial.println("Karta SD zainicjalizowana.");
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
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Wilgotność: ");
  Serial.print(hum);
  Serial.println("%");
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println("°C");

  // Wyświetlanie danych z czujnika natężenia światła
  float lux = lightMeter.readLightLevel();
  Serial.print("Natezenie swiatla: ");
  Serial.print(lux);
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
  
  // Zapisz dane na kartę SD
  logDataToSD(busVoltage1, current1_mA, busVoltage2, current2_mA, busVoltage3, current3_mA, temp, hum, lux, temperatureC1, temperatureC2, temperatureC3,now);

  // Krótkie opóźnienie przed ponownym odczytem
  delay(1000);
}
