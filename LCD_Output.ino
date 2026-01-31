#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6UmnO19yV"
#define BLYNK_TEMPLATE_NAME "IoTCircuit"
#define BLYNK_AUTH_TOKEN "6U6b7VYs_NCPAW3QHoeRcB1gaSEHYjad"

#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <BlynkSimpleWifi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long previousMillis = 0;
const long interval = 2000;
bool showVacantTable = true;

const char* ssid = "Fakepng-IoT";
const char* password = "iotengineering";

char auth[] = "6U6b7VYs_NCPAW3QHoeRcB1gaSEHYjad";

const char* mqtt_server = "project.local";
const char* topic = "out/LCD";
const int mqtt_port = 1883;
const char* mqtt_Client = "Lcd";
const char* mqtt_username = "ham";
const char* mqtt_password = "ham";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println("Message received: " + message);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.println("Failed to parse JSON");
    return;
  }

  int alltable = doc["alltable"];
  int allpeople = doc["allpeople"];
  int seatleft = 8 - allpeople;

  Blynk.virtualWrite(V5, seatleft);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    lcd.clear();
    if (showVacantTable) {
      lcd.setCursor(0, 0);
      lcd.print("Vacant table: ");
      lcd.print(alltable);
      lcd.setCursor(0, 1);
      lcd.print("People: ");
      lcd.print(allpeople);

      Serial.println("Vacant table: " + String(alltable));
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Vacant seat: ");
      lcd.print(seatleft);

      Serial.println("Vacant seat: " + String(seatleft));
      Serial.println("People: " + String(allpeople));
    }

    showVacantTable = !showVacantTable;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT");
      client.subscribe(topic);
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  setup_wifi();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  Blynk.run();
}
