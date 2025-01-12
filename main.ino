#include <Wire.h>
#include <INA3221.h>
#include <BH1750.h>
#include <RTClib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Definicje pinów i adresów
#define INA3221_ADDRESS INA3221_ADDR40_GND
#define DHTTYPE DHT11
#define SD_CS_PIN 15 
#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2

//Definicja czujników
INA3221 ina3221(INA3221_ADDRESS);
BH1750 lightMeter(0x23);
RTC_DS3231 rtc;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
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

//Definicja kanałów dla INA3221
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
  tft.fillScreen(ILI9341_BLACK);  // Wyczyść ekran
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  // Ustaw kolor tekstu
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

  //Inicjalizacja BME
  if (!bme.begin(0x76)) {
    Serial.println("Nie można połączyć się z BME280!");
    while (1);
  }

  //Inicjalizacja karty SD
  if (!SD.begin(SD_CS_PIN)) {
     Serial.println("Nie można zainicjować karty SD!");
     while (1);
   } else {
     Serial.println("Karta SD zainicjalizowana.");
   }
}

void logDataToSD(float voltage1, float current1, float power1, float tempp1, float voltage2, float current2, float power2, float tempp2, float voltage3, float current3, float power3, float tempp3, float temperature,
 float humidity, float pressure, float lightLevel, DateTime now) {
  
  // Sprawdzamy, czy plik już istnieje
  bool fileExists = SD.exists("/dane.csv");
  
  // Otwieramy plik w trybie dopisywania
  File dataFile = SD.open("/dane.csv", FILE_APPEND);

  // Jeśli plik nie istnieje, dodajemy nagłówek
  if (!fileExists && dataFile) {
    dataFile.println("Voltage1 (V);Current1 (A);Power1 (W);Tempeature P1 (C);Voltage2 (V);Current2 (A);Power2 (W);Tempeature P2 (C);Voltage3 (V);Current3 (A);Power3 (W);Tempeature P3 (C);Temperature (C);Humidity (%);Pressure (hPa);LightLevel (lux);Date;Time");
  }

  if (dataFile) {
      dataFile.print(voltage1);
      dataFile.print(";");
      dataFile.print(current1);
      dataFile.print(";");
      dataFile.print(power1);
      dataFile.print(";");
      dataFile.print(tempp1);
      dataFile.print(";");
      dataFile.print(voltage2);
      dataFile.print(";");
      dataFile.print(current2);
      dataFile.print(";");
      dataFile.print(power2);
      dataFile.print(";");
      dataFile.print(tempp2);
      dataFile.print(";");
      dataFile.print(voltage3);
      dataFile.print(";");
      dataFile.print(current3);
      dataFile.print(";");
      dataFile.print(power3);
      dataFile.print(";");
      dataFile.print(tempp3);
      dataFile.print(";");
      dataFile.print(temperature);
      dataFile.print(";");
      dataFile.print(humidity);
      dataFile.print(";");
      dataFile.print(pressure);
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
    // Odczyt napięcia i prądu z kanałów INA3221 oraz obliczenie mocy
    float busVoltage1 = ina3221.getVoltage(CHANNEL_1);
    float current1_mA = ina3221.getCurrent(CHANNEL_1);
    float power1 = busVoltage1 * current1_mA;

    float busVoltage2 = ina3221.getVoltage(CHANNEL_2);
    float current2_mA = ina3221.getCurrent(CHANNEL_2);
    float power2 = busVoltage2 * current2_mA;

    float busVoltage3 = ina3221.getVoltage(CHANNEL_3);
    float current3_mA = ina3221.getCurrent(CHANNEL_3);
    float power3 = busVoltage3 * current3_mA;

    //odczyt warunków atmosferycznych
    float temperature = bme.readTemperature();  
    float cisnienie = bme.readPressure() / 100.0F; 
    float humidity = bme.readHumidity();
    float lightLevel = lightMeter.readLightLevel();

    //odczyt temperatury czujników pod panelami
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

    //Wyświetlanie wartości na ekranie TFT 
    //warunki atmosferyczne
    tft.fillScreen(ILI9341_BLACK);  
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
    //panel1
    tft.setCursor(160, 0); 
    tft.println("Panel 1");

    tft.setCursor(160,20);
    tft.print(busVoltage1);
    tft.println(" V");

    tft.setCursor(160,40);
    tft.print(String(current1_mA,3));
    tft.println(" mA");

    tft.setCursor(160,60);
    tft.print(power1);
    tft.println(" W");

    tft.setCursor(160,80);
    tft.print(temperatureC1);
    tft.println(" C");

    ////////////////////////////
    //panel2
    tft.setCursor(0, 120); 
    tft.println("Panel 2");

    tft.setCursor(0,140);
    tft.print(busVoltage2);
    tft.println(" V");

    tft.setCursor(0,160);
    tft.print(String(current2_mA,3));
    tft.println(" mA");

    tft.setCursor(0,180);
    tft.print(power2);
    tft.println(" W");

    tft.setCursor(0,200);
    tft.print(temperatureC2);
    tft.println(" C");

    ////////////////////////////
    //panel3
    tft.setCursor(160, 120); 
    tft.println("Panel 3");

    tft.setCursor(160,140);
    tft.print(busVoltage3);
    tft.println(" V");

    tft.setCursor(160,160);
    tft.print(String(current3_mA,3));
    tft.println(" mA");

    tft.setCursor(160,180);
    tft.print(power3);
    tft.println(" W");

    tft.setCursor(160,200);
    tft.print(temperatureC3);
    tft.println(" C");

    // Odczyt aktualnej daty i godziny z RTC
    DateTime now = rtc.now();

    //Wyświetlanie danych w Serial Monitor
    Serial.print("Channel 1 Voltage: ");
    Serial.print(busVoltage1);
    Serial.println(" V");

    Serial.print("Channel 1 Current: ");
    Serial.print(current1_mA);
    Serial.println(" mA");

    Serial.print("Channel 1 Power: ");
    Serial.print(power1);
    Serial.println(" W");

    Serial.print("Channel 2 Voltage: ");
    Serial.print(busVoltage2);
    Serial.println(" V");

    Serial.print("Channel 2 Current: ");
    Serial.print(current2_mA);
    Serial.println(" mA");

    Serial.print("Channel 2 Power: ");
    Serial.print(power2);
    Serial.println(" W");

    Serial.print("Channel 3 Voltage: ");
    Serial.print(busVoltage3);
    Serial.println(" V");

    Serial.print("Channel 3 Current: ");
    Serial.print(current3_mA);
    Serial.println(" mA");

    Serial.print("Channel 3 Power: ");
    Serial.print(power3);
    Serial.println(" W");

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

    // // Zapisz dane na kartę SD
    logDataToSD(busVoltage1, current1_mA, power1, temperatureC1, busVoltage2, current2_mA, power2, temperatureC2, busVoltage3, current3_mA, power3, temperatureC3, temperature, humidity, cisnienie, lightLevel, now);

  // Krótkie opóźnienie przed ponownym odczytem
  delay(5000);
}

