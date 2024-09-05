#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// mpu6050驱动文件
#include "Wire.h"
#include <MPU6050_light.h>

// 替换为你的WiFi网络的SSID和密码
const char* ssid = "Redmi Note 12";
const char* password = "1805300624";

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
  setup_mpu6050();
}

void loop() {

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
        
        delay(25);     
      }
      digitalWrite(LEDGpio, LOW);
    }

  }
}
