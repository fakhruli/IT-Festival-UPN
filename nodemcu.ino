//#include <ESP8266wifi.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char *ssid = "SERVER NAME"; 
const char *password = "SERVER PASSWORD"; 
const char *mqtt_server = "broker.hivemq.com"; 
const char *device_id = "esp8266";

WiFiClient espClient;
PubSubClient client(espClient);

const byte ledPin5 = 16;
char message_buff[100];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  if ((char)payload[0] == '1')
  {
    Serial.print("on");
    digitalWrite(ledPin5, LOW); // PIN HIGH will switch OFF the relay
  }
  if (payload[0] == '0')
  {
    Serial.print("off);
    digitalWrite(ledPin5, HIGH); // PIN LOW will switch ON the relay
  }
}

void reconnect()
{
  while (!client.connected())
  {
    String clientName;
    clientName += "esp8266";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName += "-";
    clientName += String(micros() & 0xff, 16);

    Serial.print("Connecting to ");
    Serial.print(server);
    Serial.print(" as ");
    Serial.println(clientName);
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect((char*) clientName.c_str()))
    { 
    Serial.println("connected");
    client.publish("itfestupndemo/output","konek");
    client.subscribe("itfestupndemo/input"); // write your unique ID here
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // change port number as mentioned in your cloudmqtt console
  client.setCallback(callback);

  pinMode(ledPin5, OUTPUT);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}