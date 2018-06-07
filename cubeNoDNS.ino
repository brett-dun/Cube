
#include <ESP8266WiFi.h> //WiFi library adapted for ESP8266
#include <ESP8266WebServer.h> //WebServer library adapted for ESP8266
#include <EEPROM.h> //Arduino library that allows data to be stored into flash memory and kept, even after power has been lost

#include "html.h"

//ssid = HOME-9752
//pass = 10CA73445024EA82



//Persistant Data
uint8_t setupComplete;
const uint16_t setupCompleteAddress = 0;

char serialNumber[16];
const uint16_t serialNumberAddress = setupCompleteAddress + sizeof(setupComplete);
 
char ssid[32];
const uint16_t ssidAddress = serialNumberAddress + sizeof(serialNumber);

char password[64];
const uint16_t passwordAddress = ssidAddress + sizeof(ssid);

uint16_t secretKey[4];
const uint16_t secretKeyAddress = passwordAddress + sizeof(password);



//Global Variables
//server on port 80
ESP8266WebServer server(80);

//IP address to be used for the server
IPAddress apIP(10, 0, 0, 1);

uint8_t verified;
uint32_t verificationCode;

uint8_t numNetworksFound;
// *** this is a really messy way to do things, clean it up
String* networksPointer;



void connectToNetwork() {
  
  Serial.println("preparing to connect to network");

  Serial.println(&serialNumber[0]);
  Serial.println(&ssid[0]);
  Serial.println(&password[0]);
  
  //set wifi mode to station
  WiFi.mode(WIFI_STA);
  WiFi.begin(&ssid[0], &password[0]);

  //store the current time in milliseconds
  long startConnectionTime = millis();

  //while the device is not connected to wifi and it has been trying to connect for less than 15 seconds
  while (WiFi.status() != WL_CONNECTED && millis()-startConnectionTime < 15000) {
    
    // *** Alternate between yellow and orange LED lighting

    Serial.println(millis()-startConnectionTime);

    //wait
    delay(250);
    
  }

  //if the device is not connected to a network
  if (WiFi.status() != WL_CONNECTED) {
    
    Serial.println("Failure");
    
    // *** turn LED red

    //wait 3 seconds
    delay(3000);

    //restart the device
    ESP.restart();
    
    return;
    
  }

  //the device is now connected to wifi
  Serial.println("Connected to Wi-Fi.");
  
}



void handleSetupResponse() {

  //if post is used, then this is a response from a webpage that has already been sent
  if (server.method() == HTTP_POST) {

    //string to hold the response html
    String html;

    //if the serial number has been sent
    if (server.argName(0) == "serialnumber") {
      
      for (int i = 0; i < server.arg(0).length(); i++)
        serialNumber[i] = server.arg(0).charAt(i);
        
      html = SSID_MENU_HTML_1;
      
      for (int i = 0; i < numNetworksFound; i++)
        // *** fix this html stuff
        html += String("<button name=\"ssid\" type=\"submit\" value=" + networksPointer[i] + ">" + networksPointer[i] + "</button>");
        
      html += String(SSID_MENU_HTML_2);

    //if the ssid has been sent and its value is not "other"
    } else if (server.argName(0) == "ssid" && server.argName(1) != "other") { 

      //for each character in the string that has been sent back
      for (int i = 0; i < server.arg(0).length(); i++)
        //put the character into the array
        ssid[i] = server.arg(0).charAt(i);
        
      html = PASSWORD_HTML; // *** change this to be a list of available networks and create an option for "other"
      
    } else if (server.argName(0) == "ssid") {
      
      html = SSID_HTML;
      
    } else if (server.argName(0) == "password") {
      
      for (int i = 0; i < server.arg(0).length(); i++)
        password[i] = server.arg(0).charAt(i);
        
      // *** should probably hide the password
      html = String("<b>verify the following information</b><br>serial number: " + String(&serialNumber[0]) + "; ssid: " + String(&ssid[0]) + "; password: " + String(&password[0]) + ";");
      //html for the button to say that the information has been verified
      html += VERIFY_HTML;
        
    } else if (server.argName(0) == "verified") {

      //check to see if the response is "true"
      verified = server.arg(0) == "true" ? 1 : 0;
      Serial.println(verified);

      //generate a verification code, store it, and send it back to the client
      html = String( (verificationCode=random(999999)) );
      
    }

    //send the html back to the client
    server.send(200, "text/html", html);
    
  } else {

    //this is a first time request to this page, so send back the default html
    server.send(200, "text/html", DEFAULT_HTML);
    
  }
  
}



void setup() {

  //read setupComplete from EEPROM
  //EEPROM.get(setupCompleteAddress, setupComplete);

  uint16_t buttonTimer = 0;
  uint32_t prevTime = millis();
  //while the setup button is being held down
  while ( true && buttonTimer <= 3000) {
    buttonTimer += millis() - prevTime;
    delay(1);
  }

  //if the button was held down for at least 3 seconds at startup
  if (buttonTimer >= 3000) {
    setupComplete = 0;
    //EEPROM.put(setupCompleteAddress, setupComplete);
  }

  //if setup has not been completed
  if (setupComplete != 1) {

    memset(serialNumber, '\0', sizeof(serialNumber));
    memset(ssid, '\0', sizeof(ssid));
    memset(password, '\0', sizeof(password));

    // *** turn LED yellow
    
    //open serial connection
    Serial.begin(115200);
  
    //set wifi mode to station
    WiFi.mode(WIFI_STA);
    //disconnect
    WiFi.disconnect();
  
    //wait
    delay(100);
  
    //number of nearby wifi networks
    numNetworksFound = WiFi.scanNetworks();
    //array to hold the name of each network that was found
    String networks[numNetworksFound];
  
    for (int i = 0; i < WiFi.scanNetworks(); i++) {

      // *** sanitize this data to make sure that there are no "" and no duplicates
  
      //store the name of each network
      networks[i] = WiFi.SSID(i);
      Serial.println(networks[i]);
      
    }

    // *** doing this is actually so bad
    networksPointer = &networks[0];
  
    //set wifi as an access point
    WiFi.mode(WIFI_AP);
    //configure the access point
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    //name the network
    WiFi.softAP("Cube-Setup");

    server.on("/", handleSetupResponse);

    //when the url is not found
    server.onNotFound([]() {
      server.send(404, "text/html", "page not found");
    });
  
    //start the server
    server.begin();
    Serial.println("Server has started");

    //wait until setup has been complete
    while (!verified) {

      //handle requests to the server
      server.handleClient();

      // *** Alternate between red and blue LED lighting
      
    }

    for (int i = 0; i < 4; i++) {
      secretKey[i] = random(pow(2,16));
      Serial.print(secretKey[0], HEX);
    }
    Serial.println();

    // *** there has to be a better way to do this, figue out what it is
    //let any remaining data get sent
    delay(200);
    //disconnect
    WiFi.disconnect();

    

    //store serialNumber, ssid, and password to EEPROM to be used again after the device is restarted
    //EEPROM.put(serialNumberAddress, serialNumber);
    //EEPROM.put(ssidAddress, ssid);
    //EEPROM.put(passwordAddress, password);
    //EEPROM.put(secretKeyAddress, secretKey);

    connectToNetwork();

    // *** connect to server to send serial number, 6 digit verification code, and secret key

    //setup is now complete
    setupComplete = 1;
    //EEPROM.put(setupCompleteAddress, setupComplete);
    
  } else {

    //read the remaining persistant data from EEPROM
    //EEPROM.get(serialNumberAddress, serialNumber);
    //EEPROM.get(ssidAddress, ssid);
    //EEPROM.get(passwordAddress, password);

    connectToNetwork();
    
  }

  while (true) {
    
    while (WiFi.status() == WL_CONNECTED) {
  
      WiFiClient client;

      // *** check server to see if output needs to change
      
      if (!client.connect("wifitest.adafruit.com", 80)) {
        
        Serial.println("Connection failed!");

        //wait before trying to connect again
        delay(100);
        
      } else {
  
        Serial.println("getting html");
        client.print(String("GET /testwifi/index.html HTTP/1.1\r\n")+String("Host: wifitest.adafruit.com\r\n")+String("Connection: close\r\n\r\n"));
    
        Serial.println("reading response");
        
        while (client.connected())
          if (client.available())
            Serial.print(client.readStringUntil('\r'));
        
        Serial.println("\ndone reading response\n");
        
      }

    }

    connectToNetwork();

  }
  
}



void loop() {}
