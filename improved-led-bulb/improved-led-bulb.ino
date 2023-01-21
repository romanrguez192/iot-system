#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* SSID = "POCO X3 Pro";
const char* PASSWORD = "gik4yku9m55vpwm";

const char* MQTT_SERVER = "3.144.108.163";
const int PORT = 1883;

const String DEVICE_NAME = "modulos/modulo3/salones/salon5/bombillos/bombillo1";
const String STATUS_NAME = "/estado";
const String CHANGE_NAME = "/cambiar";

const int INTERVAL = 2000;
const int PIN = D2;
const int MIN_STATUS = 0;
const int MAX_STATUS = 255;
const int INITIAL_STATUS = MAX_STATUS;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
int status = INITIAL_STATUS;

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");

  if (String(topic).endsWith(CHANGE_NAME)) {
    payload[length] = NULL;
    status = atoi((char*)payload);
    analogWrite(PIN, status);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(DEVICE_NAME.c_str(), NULL, NULL, NULL, 0, false, NULL, false)) {
      Serial.println("connected");

      const String changeTopic = DEVICE_NAME + CHANGE_NAME;
      client.subscribe(changeTopic.c_str(), 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  analogWrite(PIN, INITIAL_STATUS);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, PORT);
  client.setCallback(callback);
}

void publishStatus() {
  const String topic = DEVICE_NAME + STATUS_NAME;
  const String stringStatus = String(status);
  client.publish(topic.c_str(), stringStatus.c_str());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > INTERVAL) {
    lastMsg = now;
    publishStatus();
  }
}