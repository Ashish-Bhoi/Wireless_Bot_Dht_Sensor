/***********************************************************************
**Project : Wireless Bot 
*
**Desc    : Wireless Bot based on Nodemcu(esp-12E) board.It also include 
*           DHT sensor Which will monitor surrounding Temperature and 
*           Humidity.
*           
**Author  : Ashish Bhoi
************************************************************************/

/**********************Include Files*****************************************/
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

/*Port Defined*/
WiFiClient client;
WiFiServer server(80);

/*LCD defined*/

LiquidCrystal_I2C lcd(0x27,16,2);

/*DHT11 defined*/
#define DHTTYPE DHT11

#define dhtpin  16       //D0 pin or gpio-16

DHT dht(dhtpin,DHTTYPE);

/**************Variables Decleared*******************************/ 

float t;                                   
float h;

/* data received from application */
String  data = "";

/*L293D motor control pins */
int leftMotorForward = 2;     /* GPIO2(D4) -> IN3   */
int rightMotorForward = 15;   /* GPIO15(D8) -> IN1  */
int leftMotorBackward = 0;    /* GPIO0(D3) -> IN4   */
int rightMotorBackward = 13;  /* GPIO13(D7) -> IN2  */


/* define L298N or L293D enable pins */
int rightMotorENB = 14; /* GPIO14(D5) -> Motor-A Enable */
int leftMotorENB = 12;  /* GPIO12(D6) -> Motor-B Enable */

void setup()
{
  /* initialize motor control pins as output */
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  /* initialize motor enable pins as output */
  pinMode(leftMotorENB, OUTPUT);
  pinMode(rightMotorENB, OUTPUT);
  /*************************************/

  dht.begin();
  /*LCD Connection*/
  lcd.begin(); 
 
  WiFiManager wifiManager;

  //reset saved settings

  //wifiManager.resetSettings();

  wifiManager.autoConnect("NODEMCU","password");

  //or use this for auto generated name ESP + ChipID

  //wifiManager.autoConnect();

  /*if you get here you have connected to the WiFi*/

  lcd.print("Waiting....");
  int connRes = WiFi.waitForConnectResult();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("WiFi Connected");
    delay(1000);
  }


  /* start server communication*/
  server.begin();
  delay(10);
  IPAddress ip;
  ip = WiFi.localIP();
  lcd.clear();
  lcd.print("IP Address : ");
  lcd.setCursor(0,1);
  lcd.print(ip);
 } 

void loop()
{
  /*  If the server available, run the "pingClient" function   */
  client = server.available();
  if (!client)
    return;
    
  data = pingClient();
  /************************ Run function according to incoming data from application *************************/

  /* If the incoming data is "forward", run the "Motor_forward" function */
  if (data == "FORWARD") Motor_forward();
  /* If the incoming data is "backward", run the "Motor_backward" function */
  else if (data == "REVERSE") Motor_backward();
  /* If the incoming data is "left", run the "Turn_left" function */
  else if (data == "LEFT") Turn_left();
  /* If the incoming data is "right", run the "Turn_right" function */
  else if (data == "RIGHT") Turn_right();
  /* If the incoming data is "stop", run the "Motor_stop" function */
  else if (data == "STOP") Motor_stop();
  /*If the incoming data is "N_E, "run the "Move_N_E" function*/
  else if (data == "NORTH_EAST") Move_N_E();
  /*If the incoming data is "N_W", run the Move_N_W funtion*/
  else if (data == "NORTH_WEST") Move_N_W();
  /*If the incoming data is "S_W", run the Move_S_E function*/
  else if (data == "SOUTH_WEST") Move_S_W();
  /***If the incoming data is "S_W", run the Move_S_W function****/
  else if (data == "SOUTH_EAST") Move_S_E();
  /**App inventor code goes here
   **Send data to app **/
  else if (data == "Get_Data")
  {
 /******************************Send data to lcd****************************************/  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp :");
  uint8_t t = dht.readTemperature(); 
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  delay(800);
  lcd.print("Humi :");
  uint8_t h = dht.readHumidity();
  lcd.print(h);
  lcd.print("%");
/************LOGIC TO GIVE SURROUNDING DATA TO APP**********************************/
 /************************ Return the response *************************************/
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("Temperature : ");
  client.print(t);
  client.print(" C");
  client.print("<br/>");
  client.print("Humidity : ");
  client.print(h);
  client.print("%");
  client.println("<br><br>");  
  client.println("</html>");
  }
}

/********************************************* FORWARD *****************************************************/
void Motor_forward(void)
{
  lcd.clear();
  lcd.print("Forward");
  analogWrite(leftMotorENB, 1020); //d7
  analogWrite(rightMotorENB, 1020);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

/********************************************* BACKWARD *****************************************************/
void Motor_backward(void)
{ 
  lcd.clear();
  lcd.print("Reverse");
  analogWrite(leftMotorENB, 1023);
  analogWrite(rightMotorENB, 1023);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

/********************************************* TURN LEFT *****************************************************/
void Turn_left(void)
{
  lcd.clear();
  lcd.print("Left");
  analogWrite(leftMotorENB, 1023);
  analogWrite(rightMotorENB, 1023);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
}

/********************************************* TURN RIGHT *****************************************************/
void Turn_right(void)
{
  lcd.clear();
  lcd.print("Right");
  analogWrite(leftMotorENB, 1023);
  analogWrite(rightMotorENB, 1023);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
  digitalWrite(leftMotorBackward, HIGH);
}

/********************************************* STOP *****************************************************/
void Motor_stop(void)
{
  lcd.clear();
  lcd.print("Stop");
  analogWrite(leftMotorENB, 0);
  analogWrite(rightMotorENB, 0);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}
/*****************************************MOTOR_NORTH_WEST*************************************************/

void Move_N_W(void)
{
  lcd.clear();
  lcd.print("North-West");  
  analogWrite(leftMotorENB, 1023);
  analogWrite(rightMotorENB, 850);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}
/**********************************************MOTOR_NORTH_EAST************************************/
void Move_N_E(void)
{
  lcd.clear();
  lcd.print("North-East");
  analogWrite(leftMotorENB, 850);
  analogWrite(rightMotorENB, 1023);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}
/*****************************************MOTOR_SOUTH_WEST*****************************************/
void Move_S_W(void)
{
  lcd.clear();
  lcd.print("South-West");  
  analogWrite(leftMotorENB, 1023);
  analogWrite(rightMotorENB, 850);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}
/*******************************************MOTOR_SOUTH_EAST***************************************/
void Move_S_E(void)
{
  lcd.clear();
  lcd.print("South-East");
  analogWrite(leftMotorENB, 850);
  analogWrite(rightMotorENB, 1023);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

/***********GET DATA FROM APP*****************************/
String pingClient (void)
{
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length() - 9, 9);
  return request;
}
