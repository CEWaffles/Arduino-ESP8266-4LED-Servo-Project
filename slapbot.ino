/*******************************************************************
	Connect to Twtich Chat with a Bot
	Created with code from TheOtherLoneStar (https://www.twitch.tv/theotherlonestar)
	Hackaday IO: https://hackaday.io/otherlonestar
	By Brian Lough (https://www.twitch.tv/brianlough)
	YouTube: https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
	Created with code from noycebru www.twitch.tv/noycebru
  Modified by BuckNaked78 for use with 4 LED with Servo Twitch Project  
    - Added Usage of secrets.h file, to pull SSID info and TWITCHBOT_OAUTH_TOKEN
    - To get TWITCHBOT_OAUTH_TOKEN go to https://twitchapps.com/tmi/ as stated below
    - Added Additional Functions for LED Colors, Servo and etc...
 *******************************************************************/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <IRCClient.h>
#include <Servo.h>
#include "secrets.h"
Servo servo; 

//define your default values here, if there are different values in config.json, they are overwritten.
//#define secret_ssid "SECRET_SSID" 
#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
 
//------- Replace the following! ------
char ssid[] = SECRET_SSID;        // your network SSID (name)
char password[] = SECRET_PASS;    // your network key
 
//The name of the channel that you want the bot to join
const String twitchChannelName = "bucknaked78";
 
//The name that you want the bot to have
#define TWITCH_BOT_NAME "buckiebot"
 
//OAuth Key for your twitch bot
// https://twitchapps.com/tmi/
#define TWITCH_OAUTH_TOKEN TWITCHBOT_OAUTH_TOKEN
 
//------------------------------

/* DEFINE LEDs */
int led = 2;    // Green LED
int RED_PIN = 14;       // GPIO 14 = D5
int YELLOW_PIN = 16;    // GPIO 16 = D0
int BLUE_PIN = 4;       // GPIO  4 = D2
int GREEN_PIN = 2;      // GPIO  2 = D4

int Slap = 12;  // GPIO  12 = D6 || Servo
String ircChannel = "";

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

long randNum;       // Random Number used with Servo
int readVal = 0;    // Varible to store read values
 
// put your setup code here, to run once:
void setup() {
  pinMode(led, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(Slap, OUTPUT);
  
  servo.attach(12);
  servo.write(0);
  delay(2000);
  servo.write(45);
  delay(750);
  servo.write(95);
  delay(750);
  servo.write(180);
  delay(750);
  servo.write(0);
  
  Serial.begin(115200);
  Serial.println();
 
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
 
  // Attempt to connect to Wifi network:
  digitalWrite(YELLOW_PIN, HIGH);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(RED_PIN, HIGH);
    Serial.print(".");
    delay(500);
    digitalWrite(RED_PIN, LOW);
  }
  
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
 
  ircChannel = "#" + twitchChannelName;

  //Serial.print("OAUTH: ");
  //Serial.println(TWITCH_OAUTH_TOKEN);
 
  client.setCallback(callback);
  delay(1200);
  digitalWrite(GREEN_PIN, LOW);
  
}   // ===== END SETUP =====
 
void loop() {
 
  // Try to connect to chat. If it loses connection try again
  if (!client.connected()) {
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    delay(75);
    digitalWrite(GREEN_PIN, HIGH);
    Serial.println("Attempting to connect to " + ircChannel );
    // Attempt to connect
    // Second param is not needed by Twtich
    if (client.connect(TWITCH_BOT_NAME, "", TWITCH_OAUTH_TOKEN)) {
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      client.sendRaw("JOIN " + ircChannel);
      Serial.println("  Connected and ready to rock");
      sendTwitchMessage("buckna10DerpBat buckna10DerpBat buckna10DerpBat buckna10DerpBat");
      sendTwitchMessage("LED SERVER is Online!");         // Sends Message to Twitch Chat
      sendTwitchMessage("buckna10DerpBat buckna10DerpBat buckna10DerpBat buckna10DerpBat");
      
    } else {
      Serial.println("Failed... try again in 5 seconds");
      digitalWrite(RED_PIN, HIGH);
      // Wait 5 seconds before retrying
      delay(5000);
      digitalWrite(RED_PIN, LOW);
      
    }
    return;
  }
  client.loop();
  
}   // ===== END LOOP =====

// ########## SEND TWITCH MESSAGE ##########
void sendTwitchMessage(String message) {
  Serial.print("  Trying to Send Message to Twitch Chat...");
  client.sendMessage(ircChannel, message);
  
}

// ########## CALLBACK FUNCTION ##########
void callback(IRCMessage ircMessage) {
  //Serial.print("CallBack Msg: ");        // Comment Out
  //Serial.print(ircMessage.command);
  //Serial.print(" - ");
  //Serial.println(ircMessage.text);
  
  // ##############################
  // #####    !led Command    #####
  // ##############################
  // Grab ircMessage.text, should be !led and then grab trailing text up to 7 characters for options/colors available

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!led-status") {
    Serial.println("!ledstatus command");
    sendTwitchMessage("Yes, It is on");
  
  }   // End !ledoff Chat Command
  
  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!ledoff") {
      Serial.println("!ledoff command");
      led_off();
  
  }   // End !ledoff Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!ledstate") {
      Serial.println("!ledoff command");
      getState();
  
  }   // End !ledoff Chat Command

  
  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!rainbow") {
      Serial.println("!led rainbow command");
      led_rainbow();      
  
  }   // End !rainbow Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!rrainbow") {
      Serial.println("!led rainbow command");
      led_reverse_rainbow();      
  
  }   // End !rainbow Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!blue") {
      Serial.println("!led blue command");
      led_blue();      
  
  }   // End !blue Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!yellow") {
      Serial.println("!led yellow command");
      led_yellow();      
  
  }   // End !yellow Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!red") {
      Serial.println("!led red command");
      led_red();      
  
  }   // End !red Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!green") {
      Serial.println("!led green command");
      led_green();      
  
  }   // End !green Chat Command
  

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!servo") {
      Serial.println("!servo command");
      servo_only();
  
  }   // End !servo Chat Command

  if (ircMessage.command == "PRIVMSG" && ircMessage.text == "!led01") {
      Serial.println("!led01 command");
      led01();
  
  }   // End !servo Chat Command
  
  // ##########################################################################################
  // ########################### END FUNCTIONS ADDED BY BUCKNAKED78 ###########################
  // ##########################################################################################


  // ##############################
  // #### Stream Element Calls ####
  // ##############################
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
   
    ircMessage.nick.toUpperCase();
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    //prints chat to serial
    //Serial.println(message);

    // #### Subscribed ####
    if (ircMessage.text.indexOf("subscribed") > -1 && ircMessage.nick == "STREAMELEMENTS") {
      led_reverse_rainbow();
   
    }

    if (ircMessage.text.indexOf("following") > -1 && ircMessage.nick == "STREAMELEMENTS") {
      for (int i = 0; i <= 0; i++) {
        Serial.print("i: [");
        Serial.print(i);
        Serial.println("] Following Message Found pt 1");
        digitalWrite(GREEN_PIN, HIGH);
        delay(75);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
        delay(75);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(BLUE_PIN, HIGH);
        delay(75);
        digitalWrite(BLUE_PIN, LOW);
        digitalWrite(RED_PIN, HIGH);
        delay(75);
        digitalWrite(RED_PIN, LOW);
        servo.write(180);
        delay(200);
      }
      
      for (int i = 1; i <= 3; i++) {
        Serial.println("Following Message Found pt 2");
        servo.write(180);
        delay(500);
        servo.write(100);
        delay(200);
      } 
   
      for (int i = 3; i <= 3; i++) {
        Serial.println("Following Message Found pt 3");
        servo.write(0);
        delay(500);
      } 
  } 
 
    return;
  }
  
}
// ########## ########## ########## ########## 
// ########## END CALLBACK FUNCTION ##########
// ########## ########## ########## ########## 


// ########## ########## ########## ########## 
void getState() {   // Get Current State of LEDs and Servo
  const char* txtmessage;
  
  readVal = digitalRead(YELLOW_PIN);   // Read Current State of PIN
  Serial.print("Yellow: ");
  Serial.println(readVal);
  
  readVal = digitalRead(RED_PIN);   // Read Current State of PIN
  Serial.print("Red: ");
  Serial.println(readVal);
  
  readVal = digitalRead(BLUE_PIN);   // Read Current State of PIN
  Serial.print("Blue: ");
  Serial.println(readVal);
    
  readVal = digitalRead(GREEN_PIN);   // Read Current State of PIN
  Serial.print("Green: ");
  Serial.println(readVal);
  
  readVal = servo.read();   // Read Current State of PIN
  Serial.print("Servo: ");
  Serial.println(readVal);

  txtmessage = "Servo: ";
  txtmessage = txtmessage + readVal;
  sendTwitchMessage(txtmessage);
  
}
// ########## ########## ########## ########## 


// ######################################################################
// ######################################################################

// ##### TURN LEDS OFF #####
void led_off() {    // Called with !led rainbow
  Serial.println("  Turned Off LED(s)");  
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led
  servo.write(0);                   // Zero Servo
  
}

void servo_only() {
  servo.write(0);
  Serial.println("  Servo Only Command");
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led

  for (int i = 0; i <= 180; i++) {
    randNum = random(0, 180);
    servo.write(randNum);
    delay(125);
        
  }
  
  delay(200);
  Serial.println("  Completed Servo Only Command");
  servo.write(0);
  
}

// ##### RAINBOW #####
void led_rainbow() {    // Called with !led rainbow
  Serial.println(" > Inside Rainbow Function <");  
  
  for (int i = 0; i <= 25; i++) {
    
    digitalWrite(YELLOW_PIN, HIGH);
    servo.write(90);
    delay(25);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    delay(25);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    servo.write(45);
    delay(25);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    delay(25);
    digitalWrite(GREEN_PIN, LOW);
    servo.write(180);
    delay(200);
    servo.write(0);

  }

  digitalWrite(YELLOW_PIN, HIGH);    // Turn On Led
  digitalWrite(BLUE_PIN, HIGH);      // Turn On Led
  digitalWrite(RED_PIN, HIGH);       // Turn On Led
  digitalWrite(GREEN_PIN, HIGH);     // Turn On Led
  delay(150);
  servo.write(0);

}


void led_blue() {    // Called with !led rainbow
  Serial.println("  BLUE LED GO!!!");  
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led
  servo.write(0);                   // Zero Servo
  delay(75);

  randNum = random(0, 180);
  
  for (int i = 0; i <= 25; i++) {
    digitalWrite(BLUE_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(BLUE_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);

    digitalWrite(BLUE_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(BLUE_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(BLUE_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    digitalWrite(BLUE_PIN, LOW);
    delay(150);
    servo.write(random(0, 180));
    
  }

  digitalWrite(BLUE_PIN, HIGH);
  delay(150);
  servo.write(0);
  Serial.println("Compelted !blue");
  
}

void led_red() {    // Called with !led rainbow
  Serial.println("  RED LED GO!!!!");  
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led
  servo.write(0);                   // Zero Servo
  delay(75);

  randNum = random(0, 180);
  
  for (int i = 0; i <= 25; i++) {
    digitalWrite(RED_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(RED_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);

    digitalWrite(RED_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(RED_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(RED_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    digitalWrite(RED_PIN, LOW);
    delay(150);
    servo.write(random(0, 180));
    
  }

  digitalWrite(RED_PIN, HIGH);
  delay(150);
  servo.write(0);
  Serial.println("Compelted !blue");
  
}

void led_yellow() {    // Called with !led rainbow
  Serial.println("  RED LED GO!!!!");  
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led
  servo.write(0);                   // Zero Servo
  delay(75);

  randNum = random(0, 180);
  
  for (int i = 0; i <= 25; i++) {
    digitalWrite(YELLOW_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(YELLOW_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);

    digitalWrite(YELLOW_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(YELLOW_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(YELLOW_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    digitalWrite(YELLOW_PIN, LOW);
    delay(150);
    servo.write(random(0, 180));
    
  }

  digitalWrite(YELLOW_PIN, HIGH);
  delay(150);
  servo.write(0);
  Serial.println("Compelted !blue");
  
}

void led_green() {    // Called with !led rainbow
  Serial.println("  RED LED GO!!!!");  
  digitalWrite(YELLOW_PIN, LOW);    // Turn off Led
  digitalWrite(BLUE_PIN, LOW);      // Turn off Led
  digitalWrite(RED_PIN, LOW);       // Turn off Led
  digitalWrite(GREEN_PIN, LOW);     // Turn off Led
  servo.write(0);                   // Zero Servo
  delay(75);

  randNum = random(0, 180);
  
  for (int i = 0; i <= 25; i++) {
    digitalWrite(GREEN_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(GREEN_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);

    digitalWrite(GREEN_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(GREEN_PIN, LOW);
    servo.write(random(0, 180));
    delay(25);
    
    digitalWrite(GREEN_PIN, HIGH);
    servo.write(random(0, 180));
    delay(25);
    digitalWrite(GREEN_PIN, LOW);
    delay(150);
    servo.write(random(0, 180));
    
  }

  digitalWrite(GREEN_PIN, HIGH);
  delay(150);
  servo.write(0);
  Serial.println("Compelted !blue");
  
}

void led_reverse_rainbow() {    // Called with !led rainbow
  Serial.println(" > Inside Reverse Rainbow Function <");  
  
  for (int i = 0; i <= 25; i++) {
    digitalWrite(YELLOW_PIN, HIGH);
    servo.write(90);
    delay(25);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    delay(25);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    servo.write(45);
    delay(25);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    delay(25);
    digitalWrite(GREEN_PIN, LOW);
    servo.write(180);

    digitalWrite(GREEN_PIN, HIGH);
    servo.write(90);
    delay(25);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    delay(25);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    servo.write(45);
    delay(25);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    delay(25);
    digitalWrite(YELLOW_PIN, LOW);
    servo.write(180);

    delay(200);
    servo.write(0);

  }

  digitalWrite(YELLOW_PIN, HIGH);    // Turn On Led
  digitalWrite(BLUE_PIN, HIGH);      // Turn On Led
  digitalWrite(RED_PIN, HIGH);       // Turn On Led
  digitalWrite(GREEN_PIN, HIGH);     // Turn On Led
  delay(150);
  servo.write(0);

}

void led01() {
  // 
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  servo.write(0);
  delay(500);

  for (int i = 0; i <= 180; i++) {
    servo.write(i);
    if (i <= 45) {
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      
    }
    else if (i >= 90){
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
    }
    else if (i >= 135){
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
    }
    else {
      Serial.println(i);      // ##### FIX THIS and add that Badad237's Code for Reset cause you know... 
      digitalWrite(BLUE_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
    }

    delay(50);
    
  }

  servo.write(0);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  
}
