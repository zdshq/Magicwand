#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// mpu6050驱动文件
#include "Wire.h"
#include <MPU6050_light.h>
#include <ESP32_FTPClient.h>
#include "octocat.h"
#include <stdio.h>
// 替换为你的WiFi网络的SSID和密码
const char* ssid = "CMCC-6G94";
const char* password = "88888888";

char ftp_server[] = "192.168.1.10";
char ftp_user[]   = "user";
char ftp_pass[]   = "12345";


#define KeyGpio  15
#define LEDGpio  13
#define MpuSCL    6
#define MpuSDA    5
// 创建 mpu6050对象
MPU6050 mpu(Wire);

// 创建WiFi和PubSubClient对象
WiFiClient espClient;
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass);

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
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     // 创建客户端ID
//     String clientId = "ESP32Client-";
//     clientId += String(random(0xffff), HEX);

//     // 尝试连接
//     if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
//       Serial.println("connected");
//       // 一旦连接，发布一条消息
//       client.publish("test/topic", "Hello from ESP32");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.print(" try again in 5 seconds. Reason: ");
//       Serial.println(client.state());
//       // 等待5秒后重试
//       delay(5000);
//     }
//   }
// }

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
    ftp.OpenConnection();
    //Change directory
    ftp.ChangeWorkDir("/a");
    delay(10);
    if(digitalRead(KeyGpio))
    {
      digitalWrite(LEDGpio, HIGH);
      while(digitalRead(KeyGpio))
        delay(10);
      ftp.InitFile("Type A");
      ftp.NewFile("test.txt");
      char buffer[512];
      for(int i = 0; i < 200; i++)
      {
        mpu.update();
        sprintf(buffer,"ACCELERO  X: %.2f\tY: %.2f\tZ: %.2f\n",  mpu.getAccX(), mpu.getAccY(), mpu.getAccZ());
        ftp.Write(buffer);
        sprintf(buffer,"GYRO  X: %.2f\tY: %.2f\tZ: %.2f\n",  mpu.getGyroX(), mpu.getGyroY(), mpu.getGyroZ());
        ftp.Write(buffer);
        sprintf(buffer,"ACC ANGLE X: %.2f\tY: %.2f\n",  mpu.getAccAngleX(), mpu.getAccAngleY());
        ftp.Write(buffer);
        delay(25);     
      }
      ftp.CloseFile();
      digitalWrite(LEDGpio, LOW);
    }
      ftp.CloseConnection();
  }

  counter++;
}
