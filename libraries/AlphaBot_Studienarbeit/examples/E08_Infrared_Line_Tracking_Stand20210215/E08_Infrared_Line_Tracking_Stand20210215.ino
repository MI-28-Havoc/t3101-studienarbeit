/***************************************
  Waveshare AlphaBot Car Infrared Line Tracking

  CN: www.waveshare.net/wiki/AlphaBot
  EN: www.waveshare.com/wiki/AlphaBot
****************************************/
#include "TRSensors.h"

#define NUM_SENSORS 5



// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
TRSensors trs = TRSensors();
unsigned int sensorValues[NUM_SENSORS];
unsigned int last_proportional = 0;
long integral = 0;

int ENA = 5;
int ENB = 6;
int IN1 = A1;
int IN2 = A0;
int IN3 = A2;
int IN4 = A3;


int w = 0; //-> Winkel zum Ziel
int x = 0; //-> x-Koordinate
int y = 0; //-> y-Koordinate
int d = 0; //-> Delay / Bestätigung
int r = 0; //Richtung

//Zielkoordinaten, werden später über Bluetooth empfangen
int xz = 0;
int yz = 0;
typedef struct {
  int bt_xz;
  int bt_yz;
} koord;

int i_0;
int i_1;
int i_2;
int i_3;
int i_4;

const int maximum = 100;
int power_difference;

void setup()
{
  Serial.begin(38400);
  Serial.println("TRSensor example");
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  for (int i = 0; i < 400; i++)                  // make the calibration take about 10 seconds
  {
    trs.calibrate();                             // reads all sensors 10 times
  }
  Serial.println("calibrate done");

  // print the calibration minimum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(trs.calibratedMin[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(trs.calibratedMax[i]);
    Serial.print(' ');
  }
  Serial.println();
  delay(1000);
}


void loop()
{
  if (Serial.available() > 0) { // Checks whether data is comming from the serial port
    //xz = Serial.read(); // Reads the data from the serial port
    //yz = Serial.read(); // Reads the data from the serial port
    koord = Serial.read();
  }
  xz = koord::bt_xz;
  yz = koord::bt_yz;


  unsigned int position = trs.readLine(sensorValues);

  int proportional = (int)position - 2000;
  power_difference = proportional / 15; //+derivative; //mit 30 probieren

  if (power_difference > maximum)
    power_difference = maximum;
  if (power_difference < -maximum)
    power_difference = -maximum;

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    switch (i)
    {
    case 0:
      i_0 = sensorValues[i];
    case 1:
      i_1 = sensorValues[i];
    case 2:
      i_2 = sensorValues[i];
    case 3:
      i_3 = sensorValues[i];
    case 4:
      i_4 = sensorValues[i];
    default:
      break;
    }
  }

  int ii = i_0 + i_1 + i_2 + i_3 + i_4;
  if (ii >= 4000)
  {
    d++;
    if (d == 7)
    {
      Serial.println("Kreuzung erkannt");
      Serial.print(x);
      Serial.print(" / ");
      Serial.print(y);
      Serial.print("\n");

      drivestop();
      //HIER METHODE FÜR RICHTUNG
      //später soll hier w Wert von Apollo kommen
      //berechnung kann für Apollo adaptiert werden
      //++++++++++++++++++++++++++++++
      if (((yz - y) > 0) && (r % 2 == 0))
      {
        w = 1;
      }
      else if (((yz - y) > 0) && (r % 2 == 1))
      {
        w = 3;
      }
      else if ((xz - x) > 0 && (r % 2 == 0))
      {
        w = 2;
      }
      else if ((xz - x) > 0 && (r % 2 == 1))
      {
        w = 1;
      }
      else if (((xz - x) == 0) && ((yz - y) == 0))
      {
        w = 4; //Fahrt wird gestoppt

        Serial.println("Pause");
      }
      else {}

      //++++++++++++++++++++++++++++++
      switch (w)
      {
      case 1: middle(); break;
      case 2: driveright(); break;
      case 3: driveleft(); break;
      case 4: zielerreicht(); break;
      default: break;
      }
    }
  }
  else
  {
    d = 0;
    //Serial.println("StandardTracking");
    //links
    if (power_difference < 0)
    {
      analogWrite(ENB, maximum + power_difference);
      analogWrite(ENA, maximum);
    }
    //rechts
    else
    {
      analogWrite(ENB, maximum);
      analogWrite(ENA, maximum - power_difference);
    }
  }
}

void driveleft()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 1000mS
  {
    analogWrite(ENB, 0);// + power_difference);
    analogWrite(ENA, 2000);//maximum);
    endtime = millis();
  }
  analogWrite(ENA, 0);
  if (r % 2 == 0)
  {
    x--;
  }
  else if (r % 2 == 1)
  {
    y++;
  }
  else {}
  r++;
  //->sende Koordinaten x, y, r mit Bluetooth zurück
}

void driveright()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 1000mS
  {
    analogWrite(ENB, 200);
    analogWrite(ENA, 0);
    endtime = millis();
  }
  analogWrite(ENB, 0);
  if (r % 2 == 0)
  {
    x++;
  }
  else if (r % 2 == 1)
  {
    y--;
  }
  else {}
  r++;
  //->sende Koordinaten x, y, r mit Bluetooth zurück
}

void middle()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 1000mS
  {
    analogWrite(ENB, maximum);
    analogWrite(ENA, maximum);
    endtime = millis();
  }
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
  if (r % 2 == 0)
  {
    y++;
  }
  else if (r % 2 == 1)
  {
    x++;
  }
  else {}
  //  ->sende Koordinaten x, y mit Bluetooth zurück
}


void drivestop()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 1000) // do this loop for up to 1000mS
  {
    analogWrite(ENB, 0);
    analogWrite(ENA, 0);
    endtime = millis();
  }
}

void zielerreicht()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 10000) // do this loop for up to 1000mS
  {
    analogWrite(ENB, 0);
    analogWrite(ENA, 0);
    endtime = millis();
  }
}

void turnaround()
{
  int aa = 0;
  while (aa <= 2)
  {
    int a = 0;
    while (a <= 20000)
    {
      analogWrite(ENA, 1000);
      analogWrite(ENB, 0);
      a++;
    }
    aa++;
  }
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
  if (r % 2 == 0)
  {
    y++;
  }
  else if (r % 2 == 1)
  {
    x++;
  }
  else {}
}
