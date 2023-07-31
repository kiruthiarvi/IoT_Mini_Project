
//------------------------------Initialisation---------------------------------------------
#define BLYNK_PRINT Serial
#include <WiFi.h>
#define apiKey "P7TOC6IJ6P3WJC8T"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_TEMPLATE_ID "TMPL3QKWHXNxw"
#define BLYNK_TEMPLATE_NAME "kiruthika"
#define BLYNK_AUTH_TOKEN "uilnjdV5A9x_MFRzhYPIgd7y0HZ05OSw"
#define ONE_WIRE_BUS 5

#define SensorPin 36         // the pH meter Analog output is connected with the Arduino’s Analog
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buff[10];

int temp;


//----------------------------------------------------------------------------------------
//----------------------------------network-----------------------------------------------

// Replace with your network credentials (STATION)
const char* ssid = "Rohith Vignesh";
const char* password = "56987591";
const char *server = "api.thingspeak.com";

//------------------------------------------------------------------------------



void setup() 
{
  Serial.begin(9600); //Baud rate: 9600
   sensors.begin();
  pinMode(5,OUTPUT); 
  Serial.println("Ready"); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
   {
    delay(1000);
    Serial.println("Connecting to WiFi...");
   }
  Serial.println("Connected to WiFi");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop()
{

    Serial.println("----------------------------------------------------------------------------------------");
  Blynk.run();
//----------------------------------------pHValue------------------------------------------------
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buff[i]=analogRead(SensorPin);//pH
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buff[i]>buff[j])
      {
        temp=buff[i];
        buff[i]=buff[j];
        buff[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buff[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  //phValue=*phValue;
  phValue = 7 - (phValue/ 57.14);
  Serial.print("pHValue is=");                   //convert the millivolt into pH value
  Serial.print("");  
  Serial.print(phValue,2);
  Serial.println(" ");
  digitalWrite(5, HIGH);       
  delay(800);
  digitalWrite(5, LOW); 


//---------------------pHvalue----------------------------------------------------------

//---------------------Turbidity--------------------------------------------------------
  int sensorValue = analogRead(39);
  // Turbidity Value
  float voltage = sensorValue * (5.0 / 1024.0);
  Serial.print("Turbidity Value is = ");
  Serial.println(voltage);

//-------------------------Turbidity---------------------------------------------------


//----------------------------Temperature-----------------------------------------------
sensors.requestTemperatures(); 
  float temperature = sensors.getTempCByIndex(0);
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensors.getTempFByIndex(0));
  delay(1000);

//----------------------------Temperature-----------------------------------------------

Blynk.virtualWrite(V0,temperature);
Blynk.virtualWrite(V1,voltage);
Blynk.virtualWrite(V4,phValue);
//----------------------------ThingSpeak------------------------------------------------


  String postStr = apiKey;
  postStr += "&field1=";
  postStr += String(temperature);        //Temperature Value
  postStr += "&field2=";
  postStr += String(voltage);        //Turbidity Value
  postStr += "&field3=";
  postStr += String(phValue);            //pHValue
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: ");
  client.print(apiKey);
  client.println();
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
  delay(5000);

  Serial.println("----------------------------------------------------------------------------------------");
  //----------------------------ThingSpeak------------------------------------------------
}

 