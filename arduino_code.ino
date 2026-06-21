#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <Wire.h> 
#include <RTClib.h> 
#include <Servo.h> 
#include <LiquidCrystal_I2C.h> 
#include <NTPClient.h> 
#include <WiFiUdp.h> 
// WiFi Credentials 
const char* ssid = "YourWiFiName"; 
const char* password = "YourWiFiPassword"; 
// RTC, LCD, and Servo objects 
RTC_DS3231 rtc; 
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo servoMotor; 
ESP8266WebServer server(80); 
// NTP Client setup 
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // 19800 = IST offset in seconds 
DEPT OF ECE, JCET HUBBALLI  
31  
2025-26 
INTELLIGENT MEDICINE BOX FOR MEDICATION MANAGEMENT USING IOT 
// I/O Pins 
#define BUZZER D5 
#define BUTTON D6 
#define SERVO_PIN D7 
// Medicine times (24-hour format) 
int medHour[3] = {8, 13, 20}; 
int medMinute[3] = {0, 0, 0}; 
bool medDispensed[3] = {false, false, false}; 
// Web page form for setting medicine times 
String htmlPage() { 
String page = "<html><body><h2>Smart Medicine Dispenser</h2>" 
"<form action='/setTimes' method='GET'>" 
"Medicine 1 Time (HH:MM): <input name='t1'><br>" 
"Medicine 2 Time (HH:MM): <input name='t2'><br>" 
"Medicine 3 Time (HH:MM): <input name='t3'><br><br>" 
"<input type='submit' value='Set Times'></form></body></html>"; 
return page; 
} 
// Convert time string (HH:MM) to hour and minute 
DEPT OF ECE, JCET HUBBALLI  
32  
2025-26 
INTELLIGENT MEDICINE BOX FOR MEDICATION MANAGEMENT USING IOT 
void parseTime(String timeStr, int &hour, int &minute) { 
int colonIndex = timeStr.indexOf(':'); 
if (colonIndex > 0) { 
hour = timeStr.substring(0, colonIndex).toInt(); 
minute = timeStr.substring(colonIndex + 1).toInt(); 
} 
} 
// Handle web form submission 
void handleSetTimes() { 
if (server.hasArg("t1")) parseTime(server.arg("t1"), medHour[0], medMinute[0]); 
if (server.hasArg("t2")) parseTime(server.arg("t2"), medHour[1], medMinute[1]); 
if (server.hasArg("t3")) parseTime(server.arg("t3"), medHour[2], medMinute[2]); 
server.send(200, "text/html", "<h3>Times updated successfully!</h3><a href='/'>Go 
Back</a>"); 
} 
void setup() { 
Serial.begin(115200); 
WiFi.begin(ssid, password); 
lcd.init(); 
lcd.backlight(); 
servoMotor.attach(SERVO_PIN); 
pinMode(BUZZER, OUTPUT); 
DEPT OF ECE, JCET HUBBALLI  
33  
2025-26 
INTELLIGENT MEDICINE BOX FOR MEDICATION MANAGEMENT USING IOT 
pinMode(BUTTON, INPUT_PULLUP); 
digitalWrite(BUZZER, LOW); 
lcd.setCursor(0, 0); 
lcd.print("Connecting WiFi..."); 
while (WiFi.status() != WL_CONNECTED) { 
delay(500); 
Serial.print("."); 
} 
lcd.clear(); 
lcd.print("WiFi Connected!"); 
delay(1000); 
timeClient.begin(); 
rtc.begin(); 
server.on("/", []() { server.send(200, "text/html", htmlPage()); }); 
server.on("/setTimes", handleSetTimes); 
server.begin(); 
lcd.clear(); 
lcd.print("System Ready"); 
delay(1000); 
} 
DEPT OF ECE, JCET HUBBALLI  
34  
2025-26 
INTELLIGENT MEDICINE BOX FOR MEDICATION MANAGEMENT USING IOT 
void loop() { 
server.handleClient(); 
timeClient.update(); 
DateTime now = rtc.now(); 
lcd.setCursor(0, 0); 
lcd.print("Time: "); 
lcd.print(now.hour()); 
lcd.print(":"); 
lcd.print(now.minute()); 
lcd.print("   "); 
for (int i = 0; i < 3; i++) { 
if (now.hour() == medHour[i] && now.minute() == medMinute[i] && !medDispensed[i]) { 
lcd.setCursor(0, 1); 
lcd.print("Dispensing Slot "); 
lcd.print(i + 1); 
servoMotor.write(90);  // Rotate to dispense 
digitalWrite(BUZZER, HIGH); 
// Wait for user confirmation 
while (digitalRead(BUTTON) == HIGH) { 
delay(100); 
DEPT OF ECE, JCET HUBBALLI  
35  
2025-26 
INTELLIGENT MEDICINE BOX FOR MEDICATION MANAGEMENT USING IOT 
} 
digitalWrite(BUZZER, LOW); 
servoMotor.write(0); 
medDispensed[i] = true; 
lcd.clear(); 
lcd.print("Slot "); 
lcd.print(i + 1); 
lcd.print(" done!"); 
delay(2000); 
} 
// Reset daily 
if (now.hour() == 0 && now.minute() == 0) { 
medDispensed[i] = false; 
} 
} 
}
