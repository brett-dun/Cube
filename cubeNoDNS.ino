
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

// *** change these strings to use const char str[] PROGMEM = R"=====(<html></html>)====="; to save dynamic memory
/*const String DEFAULT_HTML = ""
"<!DOCTYPE html>"
"<html>"
  "<form action=\"/\" method=\"POST\">"
    "<input type=\"number\" name=\"serialnumber\" id=\"serialnumber\" maxlength=\"16\">"
    "<label for=\"serialnumber\">Serial Number</label>"
    "<br><input type=\"submit\" value=\"Submit\">"
  "</form>"
"</html>";*/
const char DEFAULT_HTML[] = R"=====(
<!DOCTYPE html>
<html>
  <form action="/" method="POST">
    <input type="number" name="serialnumber" id="serialnumber" maxlength="16">
    <label for="serialnumber">Serial Number</label>
    <br>
    <input type="submit" value="Submit">
  </form>
</html>
)=====";

const String SSID_HTML = ""
"<!DOCTYPE html>"
"<html>"
  "<form action=\"/\" method=\"POST\">"
    "<input type=\"text\" name=\"ssid\" id=\"ssid\" maxlength=\"32\">"
    "<label for=\"password\">Network Name (SSID)</label>"
    "<br><input type=\"submit\" value=\"Submit\">"
  "</form>"
"</html>";

const String PASSWORD_HTML = ""
"<!DOCTYPE html>"
"<html>"
  "<form action=\"/\" method=\"POST\">"
    "<input type=\"password\" name=\"password\" id=\"password\" maxlength=\"64\">"
    "<label for=\"password\">Password</label>"
    "<br><input type=\"submit\" value=\"Submit\">"
  "</form>"
"</html>";

const String VERIFY_HTML = ""
"<!DOCTYPE html>"
"<html>"
  "<form action=\"/\" method=\"POST\">"
    "<button type=\"submit\" name=\"verified\" value=\"true\">Looks Good!</button>"
  "</form>"
"</html>";

bool setupComplete;
int setupPhase = 0;

char* stringToCharPointer(String str) {
  if (str.length() > 0)
    return const_cast<char*>(str.c_str());
  else
    return "";
}

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
    int numNetworksFound = WiFi.scanNetworks();
    //array to hold the name of each network that was found
    String networks[numNetworksFound];
  
    for (int i = 0; i < WiFi.scanNetworks(); i++) {
  
      //store the name of each network
      networks[i] = WiFi.SSID(i);
      
    }
  
    //set wifi as an access point
    WiFi.mode(WIFI_AP);
    //configure the access point
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    //name the network
    WiFi.softAP("Cube-Setup");
  
    /*//when the default url is accessed
    server.on("/", HTTP_GET, []() {
      
    });*/

    server.on("/", []() {
      if (server.method() == HTTP_POST) {
        String html;
        const String sNum = "serialnumber";
        const String ssID = "ssid";
        const String pass = "password";
        const String ver = "verified";
        if (server.argName(0) == sNum) {
          serialNumber = server.arg(0);
          html = SSID_HTML;
        } else if (server.argName(0) == ssID) {
          ssid = server.arg(0);
          html = PASSWORD_HTML;
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

    // *** try keeping to use on one webpage to avoid the url changing (implement a state variable to determine what html to send and how to process the response)
    // *** combine the methods for each url into one method that checks to see whether or not POST was used
    //when the /ssid url is accessed and GET (normal url) is used
    /*server.on("/ssid", HTTP_GET, []() {
      Serial.println(server.method());
      server.send(200, "text/html", SSID_HTML);
    });*/
  
    //when the /ssid url is accessed and POST is used to transfer data
    /*server.on("/ssid", HTTP_POST, []() {
      Serial.println(server.method());
      //Serial.println( server.uri() );
      serialNumber = server.arg("serialnumber");
      Serial.println( String("serial number: "+serialNumber) );
      //normal html response
      server.send(200, "text/html", SSID_HTML);
    });*/

    //when the /password url is accessed and GET (normal url) is used
    /*server.on("/password", HTTP_GET, []() {
      Serial.println(server.method());
      server.send(200, "text/html", PASSWORD_HTML);
    });*/
  
    //when the /password url is accessed and POST is used to transfer data
    /*server.on("/password", HTTP_POST, []() {
      Serial.println(server.method());
      ssid = server.arg("ssid");
      Serial.println( String("ssid: "+ssid) );
      //normal html response
      server.send(200, "text/html", PASSWORD_HTML);
    });*/

    //when the /verify url is accessed and POST is used to transfer data
    /*server.on("/verify", HTTP_POST, []() {
      Serial.println(server.method());
      password = server.arg("password");
      Serial.println( String("password: "+password) );
      //normal html response
      server.send(200, "text/html", "verify the information before proceeding");
    });*/

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

  }

  Serial.println("preparing to connect to network");

  // *** connect to actual wifi network - often hits an exception around here
  
  //const char* ssidChar = stringToCharPointer(ssid);
  //const char* passChar = stringToCharPointer(password);
  //char* ssidChar;// = &ssid.toCharArray();
  //ssid.toCharArray(ssidChar, ssid.length()+1);
  //char* passChar;// = &password.toCharArray();
  //password.toCharArray(passChar, password.length()+1);
  /*char ssidArray[ssid.length()+1];
  for (int i = 0; i < ssid.length(); i++)
    ssidArray[i] = ssid.charAt(i);
  ssidArray[ssid.length()] = '\0';
  //char* ssidPointer = &ssidArray;*/
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

  //Serial.println(constSSIDPointer);
  //Serial.println(constPasswordPointer);
  
  //set wifi mode to station
  WiFi.mode(WIFI_STA);
  WiFi.begin(constSSIDPointer, constPasswordPointer);

  free(ssidPointer);
  free(passwordPointer);

  long startConnectionTime = millis();
  while (WiFi.status() != WL_CONNECTED /*&& millis()-startConnectionTime < 15000*/) {
    // *** Alternate between yellow and orange LED lighting
    delay(250);
    Serial.println("working...");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failure");
    // *** turn LED red
    return;
  }

  Serial.println("Connected to Wi-Fi.");
  
  // *** connect to server to send serial number
  

}

void loop() {}
