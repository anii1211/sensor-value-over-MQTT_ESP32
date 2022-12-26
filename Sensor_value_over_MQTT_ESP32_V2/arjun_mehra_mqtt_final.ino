#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>

#define LED_BUILTIN 2
const int SensorPin = 15;
int soilMoistureValue = 0;
int count = 0;
char Rx_data[11]={0}; 
 char recive_data[20];
 int a,Rx=0;
bool call_back = 0;
int rx_buff = 0;
// WiFi
const char *ssid = "INVENTINDIA_4G"; // Enter your WiFi name
const char *password = "9099942020";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/arjun";
const char *topic2 = "esp32/mehra";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
int delay_time;

WiFiClient espClient;
PubSubClient client(espClient);
void led_output(int delay_time){
  digitalWrite(LED_BUILTIN,HIGH);
  delay(delay_time);
  digitalWrite(LED_BUILTIN,LOW);
}

void setup() {
   // Set software serial baud to 115200;
 Serial.begin(115200);
  Serial2.begin(115200);
  //LED_OUTPUT
  pinMode(LED_BUILTIN,OUTPUT);

 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("payload:");
 for (int i = 0; i < length; i++) {
       Serial.print((char) payload[i]);
       Serial2.write((char)payload[i]); 
 }
 memset(payload,0,sizeof(payload));
}
void loop() {
   client.loop();
   soilMoistureValue = analogRead(SensorPin);
   delay(1000);
   Serial.println(soilMoistureValue);
   Serial.print("\n");
   if(Serial2.available())
    {
         led_output(500);
         client.publish(topic2,"DATA RECIVED IN STM32 RX BUFFER" );
         while (Serial2.available() > 0)     //Clear the input buffer
      {
        Serial2.read();
      }
    }

}
