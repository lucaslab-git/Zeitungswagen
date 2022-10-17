#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

TinyGPSPlus gps;
LiquidCrystal_I2C lcd(0x27,16,4);

SoftwareSerial ss(3, 2);

#define BLA 6
#define BLM 5
#define BRA 7
#define BRM 8

int SBLA = 0;
int SBLM = 0;
int SBRA = 0;
int SBRM = 0;

int screenmode = 0; //0 = Normal,  1 = Debug GPS

double shortest = 100;
double now;

double vlng = 0;
double vlat = 0;
int i;

int LSON = A1;  
int LPRO = A2;  
int LSWM = A3;  

File myFile;
String Name = "";  
void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  
  pinMode(BLA, INPUT);
  pinMode(BLM, INPUT);
  pinMode(BRA, INPUT);
  pinMode(BRM, INPUT);

  pinMode(LSON, OUTPUT);
  pinMode(LPRO, OUTPUT);
  pinMode(LSWM, OUTPUT);

  analogWrite(LSON, 255);
  analogWrite(LPRO, 255);
  analogWrite(LSWM, 255);
  
  lcd.init();      
  lcd.backlight();

  lcd.print("Booting up...");
  lcd.setCursor(0,1);
  lcd.print("Please wait");

  delay(1500);

  analogWrite(LSON, 0);
  analogWrite(LPRO, 0);
  analogWrite(LSWM, 0);
  
  lcd.clear();
  lcd.print("SD Init");
  delay(1500);
  
  lcd.clear();
  
  lcd.print("SD done.");

  analogWrite(LSON, 255);
  analogWrite(LPRO, 255);
  analogWrite(LSWM, 255);

  delay(1000);
  SD.begin(4);

  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      vlng = gps.location.lng();
      vlat = gps.location.lat();
      break;
    }
  }

  analogWrite(LSON, 0);
  analogWrite(LPRO, 0);
  analogWrite(LSWM, 0);
  
  
}
void Read()
{
  shortest = 100;
  myFile = SD.open("Daten.txt");
  while (myFile.available())
  {

    String Str = myFile.readStringUntil('+');
    //Serial.println("Str:  " + Str);
    Name = myFile.readStringUntil('#');
    //Serial.println("Name:  " + Name);  
    String SWM = myFile.readStringUntil('!');
    //Serial.println("SWM:  " + SWM);
    String Pro = myFile.readStringUntil('$');
   // Serial.println("Pro:" + Pro);
    String Son = myFile.readStringUntil('=');
    //Serial.println("Son:  " + Son); 
    String latc = myFile.readStringUntil('%');
    //Serial.println("lat:  " + latc);
    String lngc = myFile.readStringUntil('-');
    //Serial.println("lng:  " + lngc);
    
    now = TinyGPSPlus::distanceBetween(latc.toDouble(), lngc.toDouble(), vlat, vlng);
    if(now < shortest){
      shortest = now;
      lcd.clear();
      lcd.print(Str);
      lcd.setCursor(0,1);
      lcd.print(Name);
      Serial.print(now);
      Serial.println(" to " + Name);
      
      
     if(Pro.indexOf("J") > 0){
        analogWrite(LPRO, 255);
        Serial.println("Pro:  " + Pro);
      }else{
        analogWrite(LPRO, 0);
      }

      if(SWM.indexOf("J") > 0){
        analogWrite(LSWM, 255);
      }else{
        analogWrite(LSWM, 0);
      }

      if(Son.indexOf("J") > 0){
        analogWrite(LSON, 255);
      }else{
        analogWrite(LSON, 0);
      }
      
    }
  }
  Serial.println("B");
  myFile.close();
  
}

void next()
{
  
  myFile = SD.open("Daten.txt");
  while (myFile.available())
  {
    
    const char* readuntil = Name.c_str();
    String Next = myFile.readStringUntil(readuntil);
    Serial.println(readuntil);
    String di = myFile.readStringUntil('-');
    String Str = myFile.readStringUntil('+');
    Serial.println("Str:  " + Str);
    Name = myFile.readStringUntil('#');
    Serial.println("Name:  " + Name);  
    String SWM = myFile.readStringUntil('!');
    //Serial.println("SWM:  " + SWM);
    String Pro = myFile.readStringUntil('$');
    //Serial.println("Pro:  " + Pro);
    String Son = myFile.readStringUntil('=');
    //Serial.println("Son:  " + Son); 
    String latc = myFile.readStringUntil('%');
    //Serial.println("lat:  " + latc);
    String lngc = myFile.readStringUntil('-');
    //Serial.println("lng:  " + lngc);
    
    
     lcd.clear();
     lcd.print(Str);
     lcd.setCursor(0,1);
     lcd.print(Name);
     break;
      
    
  }
  Serial.println("Next");
  myFile.close();
  delay(1000);
  
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {

      vlng = gps.location.lng();
      vlat = gps.location.lat();
    }
  }
  
  lcd.setCursor(0,0);
  SBLA = digitalRead(BLA);
  SBLM = digitalRead(BLM);
  SBRA = digitalRead(BRA);
  SBRM = digitalRead(BRM);
  
  if (SBLA == HIGH) {
     if(screenmode == 0){
        screenmode = 1;
     }else{
        screenmode = 0;
     }
     delay(500);
  }

  if (SBRA == HIGH) {
     next();
     delay(3000);
  }
  
  if(screenmode == 0){
    if(i == 100){
      i = 0;
      Read();
    }else{
      i++;
    }
 
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Lng:");
    lcd.print(vlng, 6);
    lcd.setCursor(0,1);
    lcd.print("Lat:");
    lcd.print(vlat, 6);

    Serial.print("Lng: ");
    Serial.println(vlng, 6);
    Serial.print("Lat: ");
    Serial.println(vlat, 6);
    
    
  }
 
  
}
