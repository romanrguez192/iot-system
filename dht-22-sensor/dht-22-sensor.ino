#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
#define SEPARATOR ';'
#define TERMINATOR '/'
#define DELAY_MS 2000

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print(humidity);
  Serial.print(SEPARATOR);
  Serial.print(temperature);
  Serial.print(TERMINATOR);

  delay(DELAY_MS);
}
