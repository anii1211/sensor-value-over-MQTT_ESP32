#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#define MDASH_APP_NAME "MinimalApp"
#include <mDash.h>                     //for OTA
#define WIFI_NETWORK "INVENTINDIA_4G"  //for OTA
#define WIFI_PASSWORD "9099942020"     //for OTA
#define DEVICE_PASSWORD "5VVM6M91Lp904smza3tUcNzQ"    //for OTA
#define LED_BUILTIN 2             // led pin on esp32
#define SensorPin 34              // moisture sensor pin
#define Water_Level_pin 19        //Capacitive Level Sensor Pin
int water_level=0;
int Pump_State = 0;               // flag to difine on/off of pump
int soilMoistureValue = 0;        // to store value of moisture
int STM32 = 0;                    // flag used to give command to turn on pump only when callback func is called
char Rx_data[11]={0}; 
char recive_data[20];
int a,Rx=0;
bool call_back = 0;
int rx_buff = 0;
const char turn_the_pump_on[] = {"PUMP ON"};
// WiFi
// const char *ssid = "INVENTINDIA_4G"; // Enter your WiFi name
// const char *password = "9099942020";  // Enter WiFi password

const char *ssid = "Airtel_2 "; // Enter your WiFi name
const char *password = "Invent@789";  // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "broker.mqttdashboard.com";
const char *topic = "MCM/Authenticate";
const char *topic2 = "MCM/Confirm";
const char *topic3 = "MCM/Sanitizer";
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

//Functions
void callback(char *topic, byte *payload, unsigned int length);
void PUMP_ON(void);


void setup() {
   // Set software serial baud to 115200;
 Serial.begin(115200);
  Serial2.begin(115200);

///////////////// OTA////////////////////////////
 WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected, IP address:");
  Serial.println(WiFi.localIP());
  mDashBegin(DEVICE_PASSWORD);
  ///////////////////////////////////////////////
  //LED_OUTPUT
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(Water_Level_pin,INPUT);

 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 client.publish(topic2,"ESP32 is connected with WIFI" );
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

void loop() {
   client.loop();
   soilMoistureValue = analogRead(SensorPin);
  water_level = digitalRead(Water_Level_pin);
  Serial.print("Sanitizer Bottle:");
  Serial.println(water_level);
  if(water_level==0)
  {
    client.publish(topic3,"Sanitizer Bottle is Empty" );
  }
   if(soilMoistureValue > 2900)
   {
    Pump_State = 1;
   }
  if((Pump_State == 1) && (STM32 == 1))
  {
    delay(2000);
   Serial.println("Pump On");
    for (int i = 0; i < 8; i++)
    {
       Serial2.write((char)turn_the_pump_on[i]);
    }
    STM32 = 0;
  }
  else
   {
   Serial.print("Moisture Value:");
   Serial.println(soilMoistureValue);
   delay(2000);
   }
   Pump_State = 0;
   //void PUMP_ON();
   if(Serial2.available()) 
    {
         led_output(500);
         client.publish(topic2,"DATA RECIVED IN STM32 RX BUFFER" );
         Serial.print("stm32:");
         while (Serial2.available() > 0)     //Clear the input buffer
      {
        Serial2.read();
      }
    }
  //  Serial.println(soilMoistureValue);
  //  delay(1000);

}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("payload:");
 for (int i = 0; i < length; i++) {
       Serial.print((char) payload[i]);
       Serial2.write((char)payload[i]); 
 }
 STM32 = 1;
 memset(payload,0,sizeof(payload));
}
