String btData = "";      //Definition des Strings für die Messwerte

#include <SoftwareSerial.h>
 
#define rxPin 0
#define txPin 1
 
SoftwareSerial btSerial(rxPin, txPin);

 
void setup()
{
  btSerial.begin(9600);
    while (!Serial)
     {
     ; // Warte auf serielle Kommunikation
     }
  btSerial.println("bluetooth available");
}
 
void loop()
{
  if (btSerial.available())
  {
   //btDATA so aufgebaut: xz-Wert + yz-Wert
   btData=32
   //Serial.println(btData.toFloat());
   btSerial.println(btData);
   // clear the string for new input:
   btData = "";
   digitalWrite(ledPin, HIGH);
   delay(1000);
  }
}
