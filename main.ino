#include <Wire.h>
#include <INA3221.h>
#include <BH1750.h>
#include <RTClib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Definicje pinów i adresów
#define INA3221_ADDRESS INA3221_ADDR40_GND
#define DHTTYPE DHT11
#define SD_CS_PIN 5  // Pin CS do komunikacji z kartą SD


// Czujniki
INA3221 ina3221(INA3221_ADDRESS);
BH1750 lightMeter(0x23);
RTC_DS3231 rtc;
TFT_eSPI tft = TFT_eSPI();
Adafruit_BME280 bme;
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
  lightMeter.begin();
  sensor1.begin();
  sensor2.begin();
  sensor3.begin();

  // Inicjalizacja ekranu TFT
  tft.begin();
  tft.setRotation(1);  // Ustawienie orientacji wyświetlacza
  tft.fillScreen(TFT_BLACK);  // Wyczyść ekran
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Ustaw kolor tekstu
  tft.setTextSize(2);  // Ustaw rozmiar tekstu

  //Inicjalizacja RTC
  if (!rtc.begin()) {
     Serial.println("RTC nie jest podłączony!");
     while (1);
   }

  if (rtc.lostPower()) {
     Serial.println("RTC stracił zasilanie, ustawiamy czas...");
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }

   if (!bme.begin(0x76)) {
    Serial.println("Nie można połączyć się z BME280!");
    while (1);
  }

   Inicjalizacja karty SD
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

  dataFile.println("Voltage1;Current1;Voltage2;Current2;Voltage3;Current3;Temperature;Humidity;LightLevel;Date;Time");

  if (dataFile) {
  // Zapisz dane w formacie CSV: Voltage1, Current1, Voltage2, Current2, Voltage3, Current3, Temperature, Humidity, LightLevel, Date, Time
    dataFile.print(voltage1);
    dataFile.print(";");
    dataFile.print(current1);
    dataFile.print(";");
    dataFile.print(voltage2);
    dataFile.print(";");
    dataFile.print(current2);
    dataFile.print(";");  
    dataFile.print(voltage3);
    dataFile.print(";");
    dataFile.print(current3);
    dataFile.print(";");
    dataFile.print(temperature);
    dataFile.print(";");
    dataFile.print(humidity);
    dataFile.print(";");
    dataFile.print(lightLevel);
    dataFile.print(";");
    dataFile.print(now.day(), DEC);
    dataFile.print("/");
    dataFile.print(now.month(), DEC);
    dataFile.print("/");
    dataFile.print(now.year(), DEC);
    dataFile.print(";");
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

    float temperature = bme.readTemperature();  // Odczyt temperatury w °C
    float cisnienie = bme.readPressure() / 100.0F;  // Odczyt ciśnienia w hPa
    float humidity = bme.readHumidity();

    // Odczyt natężenia światła z BH1750
    float lightLevel = lightMeter.readLightLevel();

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

    // Wyświetlanie wartości na ekranie TFT
    tft.fillScreen(TFT_BLACK);  // Wyczyść ekran, aby uniknąć nakładania się tekstu
    tft.setCursor(0, 0); 
    tft.println("Warunki zew");

    tft.setCursor(0,20);
    tft.print(lightLevel);
    tft.println(" lx");

    tft.setCursor(0,40);
    tft.print(temperature);
    tft.println(" C");

    tft.setCursor(0,60);
    tft.print(humidity);
    tft.println(" %");

    tft.setCursor(0,80);
    tft.print(cisnienie);
    tft.println(" hPa");

  ////////////////////////////
    tft.setCursor(160, 0); 
    tft.println("Panel 1");

    tft.setCursor(160,20);
    tft.print(busVoltage1);
    tft.println(" V");

    tft.setCursor(160,40);
    tft.print(current1_mA);
    tft.println(" mA");
    ////////////////////////////
    tft.setCursor(0, 120); 
    tft.println("Panel 2");

    tft.setCursor(0,140);
    tft.print(busVoltage2);
    tft.println(" V");

    tft.setCursor(0,160);
    tft.print(current2_mA);
    tft.println(" mA");
  ////////////////////////////
    tft.setCursor(160, 120); 
    tft.println("Panel 3");

    tft.setCursor(160,140);
    tft.print(busVoltage3);
    tft.println(" V");

    tft.setCursor(160,160);
    tft.print(current3_mA);
    tft.println(" mA");

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

    Serial.print("Cisnieniee: ");
    Serial.print(cisnienie);
    Serial.println(" hPa");

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
   logDataToSD(busVoltage1, current1_mA,busVoltage2, current2_mA,busVoltage3, current3_mA,temperature, humidity, lightLevel, now);

  // Krótkie opóźnienie przed ponownym odczytem
  delay(1000);
}
