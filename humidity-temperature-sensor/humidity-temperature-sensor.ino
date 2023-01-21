#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SEPARATOR ';'
#define TERMINATOR '/'

const char* SSID = "POCO X3 Pro";
const char* PASSWORD = "gik4yku9m55vpwm";

const char* MQTT_SERVER = "3.144.108.163";
const int PORT = 1883;

const String HUMIDITY_TOPIC = "modulos/modulo3/salones/salon5/humedad";
const String TEMPERATURE_TOPIC = "modulos/modulo3/salones/salon5/temperatura";
const String NAMES_SEPARATOR = "-";

const String DEVICE_NAME = HUMIDITY_TOPIC + NAMES_SEPARATOR + TEMPERATURE_TOPIC;

const int INTERVAL = 100;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(DEVICE_NAME.c_str(), NULL, NULL, NULL, 0, false, NULL, false)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, PORT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > INTERVAL) {
    lastMsg = now;

    if (Serial.available()) {
      String humidity = Serial.readStringUntil(SEPARATOR);
      String temperature = Serial.readStringUntil(TERMINATOR);

      client.publish(HUMIDITY_TOPIC.c_str(), humidity.c_str());
      client.publish(TEMPERATURE_TOPIC.c_str(), temperature.c_str());
    }
  }
}