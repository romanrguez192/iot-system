#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* SSID = "POCO X3 Pro";
const char* PASSWORD = "gik4yku9m55vpwm";

const char* MQTT_SERVER = "3.144.108.163";
const int PORT = 1883;

const String DEVICE_NAME = "modulos/modulo3/salones/salon3/bombillos/bombillo1";
const String STATUS_NAME = "/estado";
const String TURN_ON_NAME = "/encender";
const String TURN_OFF_NAME = "/apagar";
const String TOGGLE_NAME = "/alternar";

const int INTERVAL = 2000;
const int PIN = LED_BUILTIN;
const int INITIAL_STATUS = 0;
const char* TURNED_ON = "on";
const char* TURNED_OFF = "off";

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

  if (String(topic).endsWith(TURN_ON_NAME)) {
    status = 1;
    digitalWrite(PIN, LOW);
  } else if (String(topic).endsWith(TURN_OFF_NAME)) {
    status = 0;
    digitalWrite(PIN, HIGH);
  } else if (String(topic).endsWith(TOGGLE_NAME)) {
    status = status == 0 ? 1 : 0;
    digitalWrite(PIN, status == 0 ? HIGH : LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(DEVICE_NAME.c_str(), NULL, NULL, NULL, 0, false, NULL, false)) {
      Serial.println("connected");

      const String turnOnTopic = DEVICE_NAME + TURN_ON_NAME;
      const String turnOffTopic = DEVICE_NAME + TURN_OFF_NAME;
      const String toggleTopic = DEVICE_NAME + TOGGLE_NAME;

      client.subscribe(turnOnTopic.c_str(), 1);
      client.subscribe(turnOffTopic.c_str(), 1);
      client.subscribe(toggleTopic.c_str(), 1);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, INITIAL_STATUS == 0 ? HIGH : LOW);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > INTERVAL) {
    lastMsg = now;

    const String topic = DEVICE_NAME + STATUS_NAME;

    if (status == 0) {
      client.publish(topic.c_str(), TURNED_OFF);
    } else {
      client.publish(topic.c_str(), TURNED_ON);
    }
  }
}