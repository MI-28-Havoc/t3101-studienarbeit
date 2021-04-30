/***************************************
  Waveshare AlphaBot Car Infrared Line Tracking

  CN: www.waveshare.net/wiki/AlphaBot
  EN: www.waveshare.com/wiki/AlphaBot
****************************************/

#include "TRSensors.h"
#define NUM_SENSORS 5

TRSensors trs = TRSensors();
unsigned int sensorValues[NUM_SENSORS];
unsigned int last_proportional = 0;
//long integral = 0;

int ENA = 5;
int ENB = 6;
int IN1 = A1;
int IN2 = A0;
int IN3 = A2;
int IN4 = A3;

int w = 0;  //-> Wegentscheidung bei nächste Kreuzung
int x = 0;  //-> x-Koordinate Rocky
int y = 0;  //-> y-Koordinate Rocky
int xz = 0; //-> Zielkoordinate von Apollo
int yz = 0; //-> Zielkoordinate von Apollo
int d = 0;  //-> Schleifenzähler für Kreuzungserkennung
int r = 1;  //-> Richtung in der Rocky steht (1 nach oben, 2 nach rechts, 3 nach links, 4 nach unten)
int t = 0;  //-> Counter für Turnarounds um Koordinaten und Wege korrekt zu berechnen

int i_0; //S1
int i_1; //S2
int i_2; //S3
int i_3; //S4
int i_4; //S5
int ii;  //Rechnet alle 5 Sensorwerte zusammen für Kreuzungserkennung

const int maximum = 100;
int power_difference;
int proportional;

void setup()
{
  Serial.begin(38400);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(2, OUTPUT); //LED leuchtet wenn Kreuzung erkannt wird
  pinMode(3, OUTPUT); //LED leuchtet wenn Koordinaten empfangen werden
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
  sendkoords(x, y);
  if (Serial.available() > 0)
  {
  setlights2on();
  receiveKoords(); 
  }   
  
  delay(1000);
  setlights2off();
}


void loop()
{
  //sendkoords(x, y);
  unsigned int position = trs.readLine(sensorValues);
  proportional = (int)position - 2000;
  power_difference = proportional / 15; 
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
  ii = i_0 + i_1 + i_2 + i_3 + i_4;

  //KREUZUNGSERKENNUNG
  if (ii >= 4000) //Bedingung erfüllt wenn alle Sensoren hohe Werte liefern
  {
    d++;          //um Messfehler auszuschließen muss eine Kreuzung 7 mal hintereinander erkannt werden
    if (d == 5)
    {
      setlights1on();
      drivestop(); //hier werden auch Koordinaten gesendet und empfangen
      w = wegberechnung(); //ermittelt seinen günstigsten Weg zum Ziel
      //if((t > 0) && (t < 4))
      //{
      //  t++;
       // w = 2;
      //  if(r == 1)
      //  {
      //    r = 2;
      //  }
       // else if(r == 2)
      //  {
      //    r = 4;
      //  }
      //  else if(r == 3)
      //  {
      //   r = 1;
      //  }
      //  else if(r == 4)
      //  {
     //     r = 3;
      //  }
      //}
     // else if(t == 4)
     // {
      //  t = 0;
      //}
      switch (w)
      {
      case 1: middle(); break;
      case 2: driveright(); break;
      case 3: driveleft(); break;
      //case 4: turnaround(); break;
      case 5: zielerreicht(); break;
      default: break;
      }
      d = 0;
      setlights1off();
    }
    else
    {
      //standardtracking(); 
    }
  }
  else
  {
    d = 0;
    standardtracking();   
  }
}


//+++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++
void driveleft()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 200mS
  {
    analogWrite(ENB, 0);
    analogWrite(ENA, 200);
    endtime = millis();
  }
  analogWrite(ENA, 0);
}

void driveright()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 200mS
  {
    analogWrite(ENB, 200);
    analogWrite(ENA, 0);
    endtime = millis();
  }
  analogWrite(ENB, 0);
}

void middle()
{
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= 200) // do this loop for up to 200mS
  {
    analogWrite(ENB, 200);
    analogWrite(ENA, 200);
    endtime = millis();
  }
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
}


void drivestop()
{
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
  unsigned long starttime;
  unsigned long endtime;
  starttime = millis();
  endtime = starttime;
  sendkoords(x, y);
  
  if (Serial.available() > 0)
   {
    receiveKoords(); 
   }   
   else{}
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
  setlights1on();
  setlights2on();
  while ((endtime - starttime) <= 1000) // do this loop for up to 1000mS
  { 
    analogWrite(ENB, 0);
    analogWrite(ENA, 0);
    endtime = millis();
  }
  setlights1off();
  setlights2off();
  //turnaround();
}

//void turnaround()
//{
//  t = 1;
//}


void sendkoords(int x_, int y_) 
{ 
  setlights2on();
  String btData_rocky = "";
  btData_rocky = x_;
  btData_rocky += y_;
  btData_rocky += "!";
  
  for (int i=0; i<3; i++) //Wortlänge wird auf 3 festgelegt
  {
    Serial.print(btData_rocky[i]);
  }
  delay(100);
  setlights2off();
}


void receiveKoords() 
{
  String btData_apollo = "";
  if (Serial.available() > 0) 
  {
    setlights2on();
    char currentChar;
    do
    {
      currentChar = Serial.read();
      if(currentChar != '!')
      {
      btData_apollo += currentChar;
      }
    }while (currentChar != '!');
    char xz_char = btData_apollo[0] - '0';
    char yz_char = btData_apollo[1] - '0';
    if(int(xz_char) >= 0)
    {
    xz = int(xz_char);
    }
    if(int(yz_char) >= 0)
    {
    yz = int(yz_char);
    }
  }
  setlights2off();
}

void standardtracking()
{
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


int wegberechnung()
{
  int w_;
      //r 1 -> upside
      //r 2 -> right
      //r 3 -> left
      //r 4 -> downside
      //w_ = 1; -> middle
      //w_ = 2; -> right
      //w_ = 3; -> left
      //w_ = 4; -> turnaround
      if(yz > y)
      {
        switch (r)
        {
        case 1: w_ = 1; break;
        case 2: w_ = 3; break;
        case 3: w_ = 2; break;
        case 4: w_ = 4; break;
        default: break;
        }
        r = 1;
        y++;
      }
      else if(yz < y)
      {
        switch (r)
        {
        case 1: w_ = 4; break;
        case 2: w_ = 2; break;
        case 3: w_ = 3; break;
        case 4: w_ = 1; break;
        default: break;
        }
        r = 4;
        y--;
      }
      else if(xz > x)
      {
        switch (r)
        {
        case 1: w_ = 2; break;
        case 2: w_ = 1; break;
        case 3: w_ = 4; break;
        case 4: w_ = 3; break;
        default: break;
        }
        r = 2;
        x++;
      }
      else if(xz < x)
      {
        switch (r)
        {
        case 1: w_ = 3; break;
        case 2: w_ = 4; break;
        case 3: w_ = 1; break;
        case 4: w_ = 2; break;
        default: break;
        }
        r = 3;
        x--;
      }
      else if((xz == x) && (yz == y))
      {
        w_ = 5;
      }
      else
      {
        //analogWrite(ENA, 100);
        //analogWrite(ENB, 0);
      }
   return w_;
}

void setlights1on()
{
  digitalWrite(2, HIGH);
}
void setlights1off()
{
  digitalWrite(2, LOW);
}
void setlights2on()
{
  digitalWrite(3, HIGH);
}
void setlights2off()
{
  digitalWrite(3, LOW);
}
