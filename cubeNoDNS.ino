
#include <ESP8266WiFi.h> //WiFi library adapted for ESP8266
#include <ESP8266WebServer.h> //WebServer library adapted for ESP8266
//#include <pgmspace.h> //<avr/pgmspace.h> library adapted for ESP8266 that allows for variables to be stored into flash memory

#include "html.h"

//ssid = HOME-9752
//pass = 10CA73445024EA82


//const bool SETUP_COMPLETE PROGMEM = false; 

//server on port 80
ESP8266WebServer server(80);

//IP address to be used for the server
IPAddress apIP(10, 0, 0, 1);

String serialNumber, ssid, password;
uint8_t verified;
uint16_t verificationCode;

bool setupComplete;
int setupPhase = 0;

char* stringToCharPointer(String str) {
  if (str.length() > 0)
    return const_cast<char*>(str.c_str());
  else
    return "";
}

//something in here is causing the exception
void connectToNetwork() {
  Serial.println("preparing to connect to network");

  // *** connect to actual wifi network - often hits an exception around here
  
  char* ssidPointer = (char*) malloc( (ssid.length()+1) * sizeof(char) );
  for (int i = 0; i < ssid.length(); i++)
    ssidPointer[i] = ssid.charAt(i);
  ssidPointer[ssid.length()] = '\0';
  const char* constSSIDPointer = ssidPointer;

  char* passwordPointer = (char*) malloc( (ssid.length()+1) * sizeof(char) );
  for (int i = 0; i < password.length(); i++)
    passwordPointer[i] = password.charAt(i);
  passwordPointer[password.length()] = '\0';
  const char* constPasswordPointer = passwordPointer;

  Serial.println(constSSIDPointer);
  Serial.println(constPasswordPointer);
  
  //set wifi mode to station
  WiFi.mode(WIFI_STA);
  WiFi.begin(constSSIDPointer, constPasswordPointer);
  //WiFi.begin("HOME-9752", "10CA73445024EA82");

  free(ssidPointer);
  free(passwordPointer);

  long startConnectionTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis()-startConnectionTime < 15000) {
    // *** Alternate between yellow and orange LED lighting
    delay(250);
    //Serial.println("working...");
    Serial.println(millis()-startConnectionTime);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failure");
    // *** turn LED red
    delay(3000);
    ESP.restart();
    return;
  }

  Serial.println("Connected to Wi-Fi.");
}

int numNetworksFound;
String* networksPointer;

void setup() {

  //setupComplete = pgm_read_word_near(SETUP_COMPLETE);

  //if setup has not been completed
  if (true) {

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
  
      //store the name of each network
      networks[i] = WiFi.SSID(i);
      
    }

    networksPointer = &networks[0];
  
    //set wifi as an access point
    WiFi.mode(WIFI_AP);
    //configure the access point
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    //name the network
    WiFi.softAP("Cube-Setup");

    server.on("/", []() {
      if (server.method() == HTTP_POST) {
        String html;
        const String sNum = "serialnumber";
        const String ssID = "ssid";
        const String pass = "password";
        const String ver = "verified";
        if (server.argName(0) == sNum) {
          serialNumber = server.arg(0);
          //html = SSID_HTML;
          html = SSID_MENU_HTML_1;
          for (int i = 0; i < numNetworksFound; i++)
            //html += String("<a href=\"/\">"+networksPointer[i]+"</a>");
            // *** fix this html stuff
            html += String("<button name=\"ssid\" type=\"submit\" value=" + networksPointer[i] + ">other network</button>");
          html += String(SSID_MENU_HTML_2);
        } else if (server.argName(0) == ssID) { 
          ssid = server.arg(0);
          html = PASSWORD_HTML; // *** change this to be a list of available networks and create an option for "other"
        } else if (server.argName(0) == pass) {
          //Serial.println(server.arg(0));
          password = server.arg(0);
          // *** should probably hide the password
          html = String("<b>verify the following information</b><br>serial number: " + serialNumber + "; ssid: " + ssid + "; password: " + password + ";");
          html += VERIFY_HTML;
        } else if (server.argName(0) == ver) {
          verified = server.arg(0) == "true" ? 1 : 0;
          verificationCode = random(1000000);
          html = String(verificationCode);
          while (html.length() < 6)
            html = String("0" + html);
          // *** for some reason this is not being sent
        }
        server.send(200, "text/html", html);
      } else {
        //Serial.println(server.method());
        setupPhase = 0;
        //normal html response
        server.send(200, "text/html", DEFAULT_HTML);
      }
    });

    //when the /ssid url is accessed and POST is used to transfer data
    //when the /password url is accessed and GET (normal url) is used
    //when the /password url is accessed and POST is used to transfer data
    //when the /verify url is accessed and POST is used to transfer data
    // *** after verification, display a 6 digit code which will be used to verify the device from the website

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

    //let any remaining data get sent
    delay(100);
    //disconnect
    WiFi.disconnect();

    connectToNetwork();

    // *** connect to server to send serial number


    

  } else {

    connectToNetwork();
    
  }

  
}

void loop() {

    WiFiClient client;
    if (!client.connect("wifitest.adafruit.com", 80)) {
      Serial.println("Connection failed!");
    }

    Serial.println("getting html");
    client.print(String("GET /testwifi/index.html HTTP/1.1\r\n")+String("Host: wifitest.adafruit.com\r\n")+String("Connection: close\r\n\r\n"));

    Serial.println("reading response");
    /*while (client.available()) {
      Serial.print(client.readStringUntil('\r'));
    }*/
    while (client.connected()) {
      if (client.available()) {
        Serial.print(client.readStringUntil('\r'));
      }
    }
    Serial.println();
    Serial.println("done reading response");
    delay(2000);

  // *** check server to see if output needs to change
  
}
