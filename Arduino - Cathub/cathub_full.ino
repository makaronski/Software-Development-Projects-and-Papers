// ArduinoJson - Version: 5.13.4
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>


#include <WiFi101.h>
#include <SPI.h>
#include <TelegramBot.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include "pitches.h"
#include "html.h"

const char* ssid = "arduino_test";    //  your network SSID (name) 
const char* password = "88888888";  // your network password 
const char BotToken[] = "820970992:AAGMisPtTsAAdDw6WakEEYce1M31T7QuqR0"; 
const int address = TSL2561_ADDR_FLOAT;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(address, 12345);

WiFiSSLClient telegram_client;
TelegramBot bot (BotToken, telegram_client);
TelegramKeyboard keyboard_one;

int melodyOne[] = {
NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int thisNote = 0;
int noteDuration = 0;
int pauseBetweenNotes = 0;
int melodyTwo[] = {
NOTE_B5, NOTE_G5, NOTE_B5, NOTE_G5, NOTE_B5, 0, NOTE_G5, NOTE_B5};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsTwo[] = {
12,12,12,12,12,16,12,4};


int noteDurationsOne[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
//int melodyTwo[] = {
//  NOTE_C2, NOTE_G2, NOTE_G4, NOTE_A1, NOTE_G2, 0, NOTE_B3, NOTE_C4
//};
//int noteDurationsTwo[] = {
//  4, 8, 8, 4, 4, 4, 4, 4
//};

int PortionAmount = 1; // Set default amount of food to 1 portion

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

int buzzerPin = 7; // Pin attached to the buzzer
bool startDetecting = false;
bool checkLighting = false;
unsigned long timer = 0;
String OldChatId = "";

WiFiServer server(80);
bool web_server_mode=false;
bool food=false;
bool cat_detected=false;


void setup() {

  Serial.begin(115200);
  delay(3000);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  pinMode(2, INPUT);

  //LIGHT SENSOR
  if (!tsl.begin())
  {
    Serial.println("No TSL2561 detected");
    while (1);
  }
   tsl.enableAutoRange(true);
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
  
  Serial.println("");
  Serial.println("WiFi connected");

  // choose the emoji you like using UNICODE
  // here's the list https://unicode.org/emoji/charts/full-emoji-list.html

  const char*  MusicAndFood = "\U0001F3B6   +   \U0001F36A"; // Note + Cookie
  const char*  MusicNoFood = "\U0001F3B6   NO   \U0001F36A"; // Note NO Cookie
  const char*  OnePortion = "\U0001F408"; // CAT
  const char*  TwoPortion = "\U0001F408 \U0001F408"; // 2 CATS
  const char*  ThreePortion = "\U0001F408 \U0001F408 \U0001F408"; // 3 CATS

  // define your row's
  const char* row_one[] = {MusicAndFood, MusicNoFood};
  const char* row_two[] = {OnePortion, TwoPortion, ThreePortion};
  
    pinMode(3, OUTPUT);
    
  keyboard_one.addRow(row_one, 2); // assing a row to one or more keyboards
  keyboard_one.addRow(row_two, 3); // second argument is the length of the row
  bot.begin();
  myservo.attach(6);  // attaches the servo on pin 6 to the servo object
    
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  sensors_event_t event;    
    tsl.getEvent(&event);   
          
    Serial.println(event.light);
if(!web_server_mode){
  message m = bot.getUpdates(); // Read new messages
  if ( m.chat_id != 0 ) { // Checks if there are some updates
    OldChatId = m.chat_id;
    Serial.println(m.text);

    if (m.text == "ud83cudfb6   +   ud83cudf6a") { // if Melody AND food
      bot.sendMessage(m.chat_id, "Dispensing "+String(PortionAmount)+" portion of food right now", keyboard_one);  // Reply to the same chat with a text and a custom keyboard
      playMelody(melodyOne, noteDurationsOne, 8);
    playMelody(melodyOne, noteDurationsOne, 8);
      moveServo();
      startDetecting = true;
    checkLighting = true;
      food=true;
      timer = millis();
    }
    else if (m.text == "ud83cudfb6   NO   ud83cudf6a") { // if Melody and NO food
      bot.sendMessage(m.chat_id, "At your command", keyboard_one);
       AddTableCell(food, "No", "/");
      playMelody(melodyTwo, noteDurationsTwo, 8);
    playMelody(melodyTwo, noteDurationsTwo, 8);
    playMelody(melodyTwo, noteDurationsTwo, 8);
      startDetecting = true;
      food=false;
      timer = millis();
    }
    else if (m.text == "ud83dudc08") {
      PortionAmount = 1;
      bot.sendMessage(m.chat_id, "Food portion changed to 1", keyboard_one);
    }
    else if (m.text == "ud83dudc08 ud83dudc08") {
      PortionAmount = 2;
      bot.sendMessage(m.chat_id, "Food portion changed to 2", keyboard_one);
    }
    else if (m.text == "ud83dudc08 ud83dudc08 ud83dudc08") {
      PortionAmount = 3;
      bot.sendMessage(m.chat_id, "Food portion changed to 3", keyboard_one);
    }
    else if(m.text == "Server"){
      IPAddress ip = WiFi.localIP();
      web_server_mode=true;
      Serial.println(ip);
      String message = "To see the webpage go to http://"+IpToString(ip);
      Serial.println(message);
      bot.sendMessage(m.chat_id, message , keyboard_one);
      telegram_client.stop();
      delay(1000);
      server.begin();                           // start the web server on port 80
    }
    else bot.sendMessage(m.chat_id, "Hello !", keyboard_one);
  }

  if (startDetecting) {
    int value = digitalRead(2);
    if (value == HIGH) {
      String TimeValue = String((millis() - timer) / 1000);
      bot.sendMessage(OldChatId, "Cat detected! \nTime to reach the feeder: " + TimeValue + " seconds", keyboard_one);
      startDetecting = false;
      AddTableCell(food, "Yes", TimeValue);
      
    }
    else if (millis() - timer > 120000) {
      bot.sendMessage(OldChatId, "No cat detected in the past two minutes", keyboard_one);
      startDetecting = false;
      AddTableCell(food, "No", "/");
    }
  }
   if (checkLighting) {     
    sensors_event_t event;    
    tsl.getEvent(&event);   
          
    Serial.println(event.light);    
    if (event.light < 5) {    
         digitalWrite(3, HIGH);   
    }   
    if (millis() - timer > 120000) {    
        digitalWrite(3, LOW);   
        checkLighting = false;    
      }     
    if (event.light > 5) {    
         digitalWrite(3, LOW);    
    }   
  }   
//  if (is_dark == false && time_passed == true) {    
//      digitalWrite(3,LOW);    
//    }   
}

  
  if(web_server_mode){
     WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(html);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client pressed the button
        if (currentLine.endsWith("GET /BACKBUTTON")) {
          Serial.println("");
          Serial.println("back to telegram mode");
          Serial.println("");
          web_server_mode=false;
          client.stop();
          delay(1000);
          bot.begin();
          bot.sendMessage(OldChatId, "Back online !", keyboard_one);
        }
      }
    }
    // close the connection:
    client.stop();
  }
  }


}

void moveServo() {
  Serial.println("moving servo");
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 90 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                     // waits 15ms for the servo to reach the position

  }
  delay(PortionAmount * 300);  // keep the box open for a time interval based on the amount of food you want to deliver
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 90 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                     // waits 15ms for the servo to reach the position
  }
}

void playMelody(int melody[], int noteDurations[], int numberOfNotes ) {
  Serial.println("Playing melody");

  for (int thisNote = 0; thisNote < numberOfNotes; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(7, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(7);
  }

}

String IpToString(IPAddress& _address)
{
    String str = String(_address[0]);
    str += ".";
    str += String(_address[1]);
    str += ".";
    str += String(_address[2]);
    str += ".";
    str += String(_address[3]);
    return str;
}
