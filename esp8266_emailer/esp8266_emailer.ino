// Original code found at https://www.electronicshub.org/send-an-email-using-esp8266/
// Modifications made by Taylor Hokanson, 2019
// www.taylorhokanson.com

#include <ESP8266WiFi.h>
const char* ssid = "REPLACE_ME";      // Enter the SSID of your WiFi Network.
const char* password = "REPLACE_ME";  // Enter the Password of your WiFi Network.
char server[] = "mail.smtp2go.com";   // The SMTP Server

#define LED D7                        // output pin for optional LED
int sensorPin = A0;                   // select the input pin for the water sensor
boolean canTrigger = true; 


WiFiClient espClient;
void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }

void loop()
{
  
  int sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  
  if((sensorValue >= 350) && (canTrigger == true))
  {
    Serial.println("Sensor is wet!");
    byte ret = sendEmail();             // this is where the email is sent
    canTrigger = false;
    digitalWrite(LED, HIGH);  
    delay(1000);              
    digitalWrite(LED, LOW);    
  }
  if((sensorValue <= 300) && (canTrigger == false))
  {
    Serial.println("Sensor dried out, resuming...");
    canTrigger = true;
  }
  delay(500);
}

byte sendEmail()
{
  if (espClient.connect(server, 2525) == 1) 
  {
    Serial.println(F("connected"));
  } 
  else 
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp()) 
    return 0;
  //
  /*Serial.println(F("Sending TTLS"));
  espClient.println("STARTTLS");
  if (!emailResp()) 
  return 0;*/
  //  
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp()) 
    return 0;
  //  
  Serial.println(F("Sending User"));
  // Change this to your base64, ASCII encoded username
  /*
  For example, the email address test@gmail.com would be encoded as dGVzdEBnbWFpbC5jb20=
  */
  espClient.println("REPLACE_ME"); //base64, ASCII encoded Username
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  /*
  For example, if your password is "testpassword" (excluding the quotes),
  it would be encoded as dGVzdHBhc3N3b3Jk
  */
  espClient.println("REPLACE_ME");//base64, ASCII encoded Password
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending From"));
  // change to sender email address
  espClient.println(F("MAIL From: REPLACE_ME"));
  if (!emailResp()) 
    return 0;
  // change to recipient address
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: REPLACE_ME"));
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp()) 
    return 0;
  Serial.println(F("Sending email"));
  // change to recipient address
  espClient.println(F("To: REPLACE_ME"));
  // change to your address
  espClient.println(F("From: REPLACE_ME"));
  espClient.println(F("Subject: Leak in the basement!\r\n"));
  espClient.println(F("You've got some cleaning up to do :("));
  //
  espClient.println(F("."));    // this needs to be here
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp()) 
    return 0;
  //
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available()) 
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000) 
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}
