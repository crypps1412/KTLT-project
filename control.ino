#include <ESP8266WiFi.h>
#include <Servo.h>

Servo sv1,sv2,sv3,sv4;
#define SV1  2
#define SV2  0
#define SV3  4
#define SV4  5
#define PWMA  14
#define DIRA  12
#define PWMB  13
#define DIRB  15

const char* ssid = "iPhon";
const char* password = "12345678";
int Speed;
 
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200); //Default Baud Rate for NodeMCU
  delay(10);
 
  sv1.attach(SV1);
  sv2.attach(SV2);
  sv3.attach(SV3);
  sv4.attach(SV4);
  pinMode(PWMA, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Connect to WiFi network
  Serial.println();
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
  
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  sv_build(1);
  sv_build(2);
  sv_build(3);
  sv_build(4);
}

void sv_build(int num)
{
  Servo sv;
  if (num == 1) sv = sv1;
  if (num == 2) sv = sv2;
  if (num == 3) sv = sv3;
  if (num == 4) sv = sv4;
  int pos;
  for (pos = 0; pos <= 180; pos += 1) {
    sv.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    sv.write(pos);
    delay(15);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  bool check = false;
  
  check = check | armHandle(req);

  check = check | carHandle(req);

  if (check == false)
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  s += req;
  s += "</html>";
  client.println(s);
  client.flush();
}

int getData(String req,int Max,int Min)
{
  int posGet;
  String posStr;
  for (posGet=Max;posGet>=Min;posGet--)
  {
    posStr = String(posGet);
    if (req.indexOf(posStr) != -1)
      {
        Serial.println(req.indexOf(posStr));
        return posGet;
      }
  }
  return -1;
}

int armHandle(String req)
{
  int c = getData(req,180,0);
  if (c ==-1) return 0;
  if (req.indexOf("Servo1") != -1)
  {
    sv1.write(c);
    return 1;
  }
  if (req.indexOf("Servo2") != -1)
  {
    sv2.write(c);
    return 1;
  }
  if (req.indexOf("Servo3") != -1)
  {
    sv3.write(c);
    return 1;
  }
  if (req.indexOf("Servo4") != -1)
  {
    sv4.write(c);
    return 1;
  }
}

int carHandle(String req)
{
  int Speed = getData(req,200,0);
  if (Speed ==-1) return 0;
  if (req.indexOf("goFt") != -1)
  {
    goT();
    return 1;
  }
  if (req.indexOf("goFL") != -1)
  {
    goTT();
    return 1;
  }
  if (req.indexOf("goFR") != -1)
  {
    goTP();
    return 1;
  }
  if (req.indexOf("stop") != -1)
  {
    Stop();
    return 1;
  }
  if (req.indexOf("goL") != -1)
  {
    goTr();
    return 1;
  }
  if (req.indexOf("goR") != -1)
  {
    goP();
    return 1;
  }
  if (req.indexOf("goBk") != -1)
  {
    goL();
    return 1;
  }
  if (req.indexOf("goBL") != -1)
  {
    goLT();
    return 1;
  }
  if (req.indexOf("goBR") != -1)
  {
    goLP();
    return 1;
  }
  return 0;
}

void goT() {
  Serial.println("Tiến " + String(Speed));
  analogWrite(PWMA, Speed);
  analogWrite(PWMB, Speed);
  digitalWrite(DIRB, HIGH);
  digitalWrite(DIRB, HIGH);
}
void goTP() {
  Serial.println("Tiến phải " + String(Speed));
  analogWrite(PWMA, Speed);
  analogWrite(PWMB, Speed / 2);
  digitalWrite(DIRA, HIGH);
  digitalWrite(DIRB, HIGH);
}
void goTT() {
  Serial.println("Tiến trái " + String(Speed));
  analogWrite(PWMA, Speed / 2);
  analogWrite(PWMB, Speed);
  digitalWrite(DIRA, HIGH);
  digitalWrite(DIRB, HIGH);
}
void goL() {
  Serial.println("Lùi " + String(Speed));
  analogWrite(PWMA, Speed);
  analogWrite(PWMB, Speed);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
void goLP() {
  Serial.println("Lùi phải " + String(Speed));
  analogWrite(PWMA, Speed);
  analogWrite(PWMB, Speed / 2);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
void goLT() {
  Serial.println("Lùi trái " + String(Speed));
  analogWrite(PWMA, Speed / 2);
  analogWrite(PWMB, Speed);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
void goTr() {
  Serial.println("Trái " + String(Speed));
  analogWrite(PWMA, Speed / 2);
  analogWrite(PWMB, Speed / 2);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, HIGH);
}
void goP() {
  Serial.println("Phải " + String(Speed));
  analogWrite(PWMA, Speed / 2);
  analogWrite(PWMB, Speed / 2);
  digitalWrite(DIRA, HIGH);
  digitalWrite(DIRB, LOW);
}
void Stop() {
  Serial.println("Stop " + String(Speed));
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}
