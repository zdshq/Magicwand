#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// mpu6050驱动文件
#include "Wire.h"
#include <MPU6050_light.h>

// 替换为你的WiFi网络的SSID和密码
const char* ssid = "CMCC-6G94";
const char* password = "88888888";

// 替换为你的MQTT代理的地址和端口（使用主机的IP地址）
const char* mqtt_server = "192.168.1.10";
const int mqtt_port = 1883;

// 替换为你的MQTT用户名和密码（如果有）
const char* mqtt_user = "";
const char* mqtt_password = "";

#define KeyGpio  15
#define LEDGpio  13
#define MpuSCL    6
#define MpuSDA    5
// 创建 mpu6050对象
MPU6050 mpu(Wire);

// 创建WiFi和PubSubClient对象
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long counter = 0;

// 连接WiFi的函数
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength (RSSI): ");
  Serial.println(WiFi.RSSI());
}

// 连接到MQTT代理的函数
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 创建客户端ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // 尝试连接
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // 一旦连接，发布一条消息
      client.publish("test/topic", "Hello from ESP32");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 5 seconds. Reason: ");
      Serial.println(client.state());
      // 等待5秒后重试
      delay(5000);
    }
  }
}

void setup_mpu6050(void){
  Wire.begin(MpuSDA, MpuSCL);
  byte status = mpu.begin();
  Serial.println(F("MPU6050 status:"));
  Serial.println(status);
  while(status != 0)
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void setup() {
  Serial.begin(115200);
  pinMode(KeyGpio, INPUT_PULLDOWN);
  pinMode(LEDGpio, OUTPUT);
  digitalWrite(LEDGpio, LOW);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  setup_mpu6050();
}

void loop() {
  // if (!client.connected()) {
  //   reconnect();
  // }
  if(counter > 100000000)
    counter = 0;
  // if(!(counter % 100000))
  // {
  //   client.loop();
  //   // 发布消息
  //   Serial.println("publish");
  //   client.publish("HA-ESP32-02/light/state", "on");
  // }

  if(digitalRead(KeyGpio))
  {
    delay(50);
    if(digitalRead(KeyGpio))
    {
      digitalWrite(LEDGpio, HIGH);
      while(digitalRead(KeyGpio))
        delay(10);
      for(int i = 0; i < 200; i++)
      {
        mpu.update();
        Serial.print(F("ACCELERO  X: "));Serial.print(mpu.getAccX());
        Serial.print("\tY: ");Serial.print(mpu.getAccY());
        Serial.print("\tZ: ");Serial.println(mpu.getAccZ());
      
        Serial.print(F("GYRO      X: "));Serial.print(mpu.getGyroX());
        Serial.print("\tY: ");Serial.print(mpu.getGyroY());
        Serial.print("\tZ: ");Serial.println(mpu.getGyroZ());
      
        Serial.print(F("ACC ANGLE X: "));Serial.print(mpu.getAccAngleX());
        Serial.print("\tY: ");Serial.println(mpu.getAccAngleY());


        Serial.print(F("ANGLE     X: "));Serial.print(mpu.getAngleX());
        Serial.print("\tY: ");Serial.print(mpu.getAngleY());
        Serial.print("\tZ: ");Serial.println(mpu.getAngleZ());
        delay(25);     
      }
      digitalWrite(LEDGpio, LOW);
    }

  }

  counter++;
}
