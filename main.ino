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
#define DHTPIN 4
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

void logDataToSD(float voltage1, float current1, float voltage2, float current2, float voltage3, float current3, float temperature, float humidity, float lightLevel, DateTime now) {
  // Otwieramy plik do zapisu
  File dataFile = SD.open("/dane.csv", FILE_APPEND);

  if (dataFile) {
    // Zapisz dane w formacie CSV: Voltage1, Current1, Voltage2, Current2, Voltage3, Current3, Temperature, Humidity, LightLevel, Date, Time
    dataFile.print(voltage1);
    dataFile.print(",");
    dataFile.print(current1);
    dataFile.print(",");
    dataFile.print(voltage2);
    dataFile.print(",");
    dataFile.print(current2);
    dataFile.print(",");
    dataFile.print(voltage3);
    dataFile.print(",");
    dataFile.print(current3);
    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.print(humidity);
    dataFile.print(",");
    dataFile.print(lightLevel);
    dataFile.print(",");
    dataFile.print(now.day(), DEC);
    dataFile.print("/");
    dataFile.print(now.month(), DEC);
    dataFile.print("/");
    dataFile.print(now.year(), DEC);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(":");
    dataFile.println(now.second(), DEC);

    // Zamykamy plik po zapisie
    dataFile.close();
    Serial.println("Dane zapisane na kartę SD.");
  } else {
    Serial.println("Błąd otwarcia pliku na karcie SD.");
  }
}


void loop() {
  // Odczyt napięcia i prądu z kanałów INA3221
  float busVoltage1 = ina3221.getVoltage(CHANNEL_1);
  float current1_mA = ina3221.getCurrent(CHANNEL_1);

  float busVoltage2 = ina3221.getVoltage(CHANNEL_2);
  float current2_mA = ina3221.getCurrent(CHANNEL_2);

  float busVoltage3 = ina3221.getVoltage(CHANNEL_3);
  float current3_mA = ina3221.getCurrent(CHANNEL_3);

  // Odczyt temperatury i wilgotności z DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Odczyt natężenia światła z BH1750
  float lightLevel = lightMeter.readLightLevel();

  // Odczyt aktualnej daty i godziny z RTC
  DateTime now = rtc.now();

  // Wyświetlanie danych w Serial Monitor
  Serial.print("Channel 1 Voltage: ");
  Serial.print(busVoltage1);
  Serial.println(" V");

  Serial.print("Channel 1 Current: ");
  Serial.print(current1_mA);
  Serial.println(" mA");

  Serial.print("Channel 2 Voltage: ");
  Serial.print(busVoltage2);
  Serial.println(" V");

  Serial.print("Channel 2 Current: ");
  Serial.print(current2_mA);
  Serial.println(" mA");

  Serial.print("Channel 3 Voltage: ");
  Serial.print(busVoltage3);
  Serial.println(" V");

  Serial.print("Channel 3 Current: ");
  Serial.print(current3_mA);
  Serial.println(" mA");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Light Level: ");
  Serial.print(lightLevel);
  Serial.println(" lx");

  Serial.print("Date: ");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  // Zapisz dane na kartę SD
  logDataToSD(busVoltage1, current1_mA, busVoltage2, current2_mA, busVoltage3, current3_mA, temperature, humidity, lightLevel, now);

  // Krótkie opóźnienie przed ponownym odczytem
  delay(1000);
}
