#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

#include <BlynkSimpleEsp8266.h>

char ssid[] = "SSID";
char pass[] = "PASSWORD";

Servo myServo;
int FeedStatus;

// int Full = 10;
// int Low = 20;

long duration;
float distanceCm;
float distanceInch;

const int trigPin = 12;
const int echoPin = 14;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

void setup() {
//jarak
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
//indikator
  pinMode(LED_BUILTIN, OUTPUT);
//Servo
  myServo.attach(D4);
  myServo.write(0);
  Serial.begin(115200);

//display oled
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);

//WiFi
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk Connected");

}

void Feed(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();
  display.println("wayah e mangan");
  display.display(); 
  for(int pos = 0; pos<=180;pos++)
  {
    myServo.write(pos);
    delay(10);
  }

  for (int pos = 180;pos>=0;pos--)
  {
    myServo.write(pos);
    delay(10);
  }
  
}
void Distance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY/2;

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();
  display.println("isi: "+String(distanceCm) + "%");
  display.display(); 

  // Serial.print("Distance (cm): ");
  // Serial.println(distanceCm);

  if (distanceCm < 10){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 50);
  display.println("Kondisipakan:Sedikit");
  display.display(); 
  Blynk.logEvent("pesansedikit", String("diisi pakan e bos:")+distanceCm);
  }
  
  Blynk.virtualWrite(V1, distanceCm);
  delay(500);
}

void loop() {
  Blynk.run();
  // Serial.println("FeedStatus:" +String(FeedStatus));
  Distance();
  if(FeedStatus == 1)
  {
    Feed();
    digitalWrite(LED_BUILTIN, HIGH);
    Blynk.virtualWrite(V0, 0);
    FeedStatus = 0;
    delay(2000);
    Serial.println("Feed Time");
  }
}

BLYNK_WRITE(V0)
{
  FeedStatus = param.asInt();
}
