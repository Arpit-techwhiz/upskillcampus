#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ---------- WiFi Credentials ----------
const char* ssid = "Arpit";
const char* password = "arpit1921";

// ---------- MQTT Broker ----------
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// ---------- MQTT Topics --------------
const char* temp_topic = "iot/esp8266/temperature";
const char* hum_topic  = "iot/esp8266/humidity";

// ---------- DHT Configuration --------
#define DHTPIN D4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ---------- MQTT Client --------------
WiFiClient espClient;
PubSubClient client(espClient);

// ---------- WiFi Setup ---------------
void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

// ---------- MQTT Reconnect ------------
void reconnect_mqtt() {
  while (!client.connected()) {
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
    } else {
      delay(5000);
    }
  }
}

// ---------- Setup ---------------------
void setup() {
  Serial.begin(9600);
  delay(1000);

  dht.begin();
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

// ---------- Loop ----------------------
void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  char tempStr[10];
  char humStr[10];

  dtostrf(temp, 4, 2, tempStr);
  dtostrf(hum, 4, 2, humStr);

  client.publish(temp_topic, tempStr);
  client.publish(hum_topic, humStr);

  Serial.print("Temp: ");
  Serial.print(tempStr);
  Serial.print(" °C | Hum: ");
  Serial.print(humStr);
  Serial.println(" %");

  delay(2000);
}
