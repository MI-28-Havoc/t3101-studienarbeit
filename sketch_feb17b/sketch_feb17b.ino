#include <SoftwareSerial.h>
#include <NewPing.h>
#define rxPin 0
#define txPin 1
#define TRIGGER_PIN_SR04 6
#define ECHO_PIN_SR04 7
#define MAX_DISTANCE 600


//+++++++++DEKLARATION++++++++++++++++

SoftwareSerial btSerial(rxPin, txPin);
NewPing sonarSR04(TRIGGER_PIN_SR04, ECHO_PIN_SR04, MAX_DISTANCE);
String btData_apollo; //Daten die von Apollo gesendet werden
String btData_rocky;  //Daten die von Rocky gesendet werden
//String testkoord;
int s0 = 8;
int s1 = 9;
int s2 = 10;
int s3 = 11;
int x_ziel = 0; //Zielvariable
int y_ziel = 0; //Zielvariable
int x_rocky = 0;  //Standort Rocky
int y_rocky = 0;  //Standort Rocky
int x1 = 0; //X-Wert1 von US SENSOR
int x2 = 0; //X-Wert2 von US SENSOR
int y1 = 0; //Y-Wert1 von US SENSOR
int y2 = 0; //Y-Wert2 von US SENSOR
int abstand = 130;

//+++++++++SETUP++++++++++++++++
void setup()
{
  Serial.begin(38400);
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT); 
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  digitalWrite(12, HIGH); 
  digitalWrite(13, HIGH); //Stromversorgung für BT-Modul
    while (!Serial)
     {
     ;
     }
   if(Serial.available() > 0)
   {
    receiveKoords();
   }
   delay(1000);
}


//+++++++++HAUPTPROGRAMM++++++++++++++++
void loop()
{
x_ziel = 0; 
y_ziel = 0; 
x_rocky = 0;  
y_rocky = 0; 

//MUX um Koordinaten zu ermitteln
//-> liefert normal 4 Werte, es sei denn Objekt steht im Schatten
//Vertikal und Horizontal jeweils maximal 2 Werte, minimal 1 Wert möglich



//Serial.println(x1);
//Serial.println(x2);
//Serial.println(y1);
//Serial.println(y2);






//BT-Kommunikation             
//if(Serial.available() > 0) //erhält hier Koordinaten von Rocky
//{
//  receiveKoords();
//}

muxkoords();


btData_apollo = evaluatekoords(x1, x2, y1, y2);


Serial.println(x_ziel);
Serial.println(y_ziel);

//sendkoords(btData_apollo);
}


//+++++++++FUNKTIONEN++++++++++++++++
void sendkoords(String koords) 
{ 
  for (int i=0; i<3; i++) //Wortlänge wird auf 3 festgelegt
  {
    Serial.print(koords[i]);
  }
  delay(100);
}


void receiveKoords() 
{
  String btData_rocky = "";
  if (Serial.available() > 0) 
  {
    char currentChar;
    do
    {
      currentChar = Serial.read();
      if(currentChar != '!')
      {
      btData_rocky += currentChar;
      //Serial.println("Absturz");
      }
    }while (currentChar != '!');
    char xz_char = btData_apollo[0] - '0';
    char yz_char = btData_apollo[1] - '0';
    
    if(int(xz_char) >= 0)
    {
    x_rocky = int(xz_char);
    }
    if(int(yz_char) >= 0)
    {
    y_rocky = int(yz_char);
    }
  }
}

String evaluatekoords(int x1_, int x2_, int y1_, int y2_)
{
if(x1_ == x_rocky)
{
  x_ziel = x2_;
}
else
{
  x_ziel = x1_;
}
if(y1_ == y_rocky)
{
  y_ziel = y2_;
}
else
{
  y_ziel = y1_;
}
String koords = "";
koords += x_ziel;
koords += y_ziel;
koords += "!"; 
return koords;
}

void writeMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
}

void muxkoords()
{
  bool erstenWertGefunden;
  for(int i = 0; i < 8; i++)
  {
    writeMux(i);
    delay(500);
    int cm = 0;
    cm = sonarSR04.ping_cm();
    Serial.print(i);
    Serial.print(": ");
    Serial.print(cm);
    Serial.println();
    if(cm != 0)
    {
      if(i < 4)
      {
        erstenWertGefunden = false;
        if(cm < abstand)
        {
          if(!erstenWertGefunden) 
          {
            x1 = i;
            erstenWertGefunden = true;
          }  
          else
          {
            x2 = i;     
          }
        }      
      }
      else if(i > 3)
      {
        erstenWertGefunden = false;
        if(cm < abstand)
        {
          if(!erstenWertGefunden) 
          {
            y1 = i-4;
            erstenWertGefunden = true;
          }  
          else
          {
            y2 = i-4;     
          }
        } 
      }
      delay(50);
    }
  }
}
