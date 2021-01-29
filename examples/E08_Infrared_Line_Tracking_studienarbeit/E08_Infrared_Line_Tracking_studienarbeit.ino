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

int i_0;
int i_1;
int i_2;
int i_3;
int i_4;

const int maximum = 100;
int power_difference;

void setup()
{
  Serial.begin(115200);
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
  Serial.print("\n");
  //->Winkel alpha über Bluetooth erhalten->Winkel sind relativ zum Ziel
  //  ->w = 1 wenn alpha < -45°
  //  ->w = 2 wenn alpha > 45°
  //  ->w = 3 wenn alpha <= 45° und >= -45°

  

  unsigned int position = trs.readLine(sensorValues);
  //  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  //  {
  //    Serial.print(sensorValues[i]);
  //    Serial.print('\t');  
  //  }

  // The "proportional" term should be 0 when we are on the line.
  int proportional = (int)position - 2000;
  power_difference = proportional / 15; //+derivative; //mit 30 probieren


  //power_difference wird bei maximum also 100 gedeckelt
  if (power_difference > maximum)
    power_difference = maximum;
  if (power_difference < -maximum)
    power_difference = -maximum;

  //Serial.println(position);

  for(int i = 0; i < NUM_SENSORS; i++)
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

    Serial.print(i_0);
    Serial.print("\t");
    Serial.print(i_1);
    Serial.print("\t");
    Serial.print(i_2);
    Serial.print("\t");
    Serial.print(i_3);
    Serial.print("\t");
    Serial.print(i_4);
    Serial.print("\n");

  //if (position == 2000)
  int ii = i_0 + i_1 + i_2 + i_3 + i_4;
  if (ii >= 4000)
  {
    d++;
    if (d == 6)
    {
       Serial.println("Kreuzung erkannt");
       Serial.println(w);
       w++;
      //w = 1; //TEST
      if (w == 1)
      {
        driveleft();
             
      }
      else if (w == 2)
      {
        driveright();
       
      }
      else if (w == 3)
      {
        middle();
        w = 0; //TEST
      }
    }
  }
  //if ((power_difference <= -30) || (power_difference >= 30)) //alt
  else
  {
    d = 0;
    Serial.println("StandardTracking");
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
  int a = 0;
  while (a <= 20000)
  {
    analogWrite(ENB, 0);// + power_difference);
    analogWrite(ENA, 200);//maximum);
    a++;
  }
  analogWrite(ENA, 0);
  if (r % 2 == 0)
  {
    x++;
  }
  else if (r % 2 == 1)
  {
    y++;
  }
  else {}
  r++;

  //->sende Koordinaten x, y mit Bluetooth zurück
}

void driveright()
{
  int a = 0;
  while (a <= 20000)
  {
    analogWrite(ENB, 200);
    analogWrite(ENA, 0);
    a++;
  }
  analogWrite(ENB, 0);
  if (r % 2 == 0)
  {
    x++;
  }
  else if (r % 2 == 1)
  {
    y++;
  }
  else {}
  r++;
  //->sende Koordinaten x, y mit Bluetooth zurück
}

void middle()
{
  int a = 0;
  while (a <= 10000)
  {
    analogWrite(ENB, maximum);
    analogWrite(ENA, maximum);
    a++;
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
  r++;
  //  ->sende Koordinaten x, y mit Bluetooth zurück
}
