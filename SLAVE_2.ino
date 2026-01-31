#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Fakepng-IoT";
const char* password = "iotengineering";
const char* mqtt_server = "project.local";
const int mqtt_port = 1883;
const char* mqtt_Client = "LAB2";
const char* mqtt_username = "ham";
const char* mqtt_password = "ham";

WiFiClient espClient; 
PubSubClient client(espClient);

const int pingPin1 = 2;
const int inPin1 = 3;
const int pingPin2 = 4;
const int inPin2 = 5;
const int pingPin3 = 7;
const int inPin3 = 8;
const int pingPin4 = 10;
const int inPin4 = 11;

long lastMsg = 0;
int value = 0;
char msg[100];
String DataString;
int allTable = 4;
int cm1, cm2, cm3, cm4;
int status1, status2, status3, status4;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("@msg/operator");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pingPin1, OUTPUT);
  pinMode(inPin1, INPUT);
  pinMode(pingPin2, OUTPUT);
  pinMode(inPin2, INPUT);
  pinMode(pingPin3, OUTPUT);
  pinMode(inPin3, INPUT);
  pinMode(pingPin4, OUTPUT);
  pinMode(inPin4, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    update();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi with IP: ");
  Serial.println(WiFi.localIP());
}

void update() {
  detectAndUpdate(pingPin1, inPin1, status1, cm1);
  detectAndUpdate(pingPin2, inPin2, status2, cm2);
  detectAndUpdate(pingPin3, inPin3, status3, cm3);
  detectAndUpdate(pingPin4, inPin4, status4, cm4);

  Serial.println("Chair one     : " + (String)status1 + " people");
  Serial.println("Chair two     : " + (String)status2 + " people");
  Serial.println("Chair three   : " + (String)status3 + " people");
  Serial.println("Chair four    : " + (String)status4 + " people");
  Serial.println("---------------------------");
  Serial.println("Chair one     : " + (String)cm1 + "     cm");
  Serial.println("Chair two     : " + (String)cm2 + "     cm");
  Serial.println("Chair three   : " + (String)cm3 + "     cm");
  Serial.println("Chair four    : " + (String)cm4 + "     cm");

  int Numpeople2 = status1 + status2 + status3 + status4;

  int tableStatus2;

  if (Numpeople2 > 0) {
    tableStatus2 = 0;
    Serial.println("tableStatus2: " + String(tableStatus2));
    Serial.println("Tables are occupied");
  } else {
    tableStatus2 = 1;
    Serial.println("tableStatus2: " + String(tableStatus2));
    Serial.println("Tables are empty");
  }

  String statusMessage =
    "{\"status2\": " + String(tableStatus2) +
    ", \"Numpeople2\": " + String(Numpeople2) + "}";

  statusMessage.toCharArray(msg, 100);

  client.publish("data02/people02/table02", msg);

  Serial.println("Published: " + statusMessage);
  Serial.println("==========================");
}

void detectAndUpdate(int pingPin, int inPin, int &status, int &cm) {
  long duration;

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  if (cm < 17) {
    digitalWrite(LED_BUILTIN, 0);
    status = 1;
  } else {
    digitalWrite(LED_BUILTIN, 1);
    status = 0;
  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
