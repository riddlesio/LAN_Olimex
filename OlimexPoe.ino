/*
  General Purpose Webserver adjusted for Olimex ESP32-PoE board.
  It can control Olimex module MOD-BME280.
  This sketch requires Arduino for ESP32 - how to install it read here: https://github.com/espressif/arduino-esp32
  This project is based on the one created by David Bird. The original can be found and downloaded here: https://github.com/G6EJD/ESP32-General-Purpose-Webserver
  The MIT License (MIT) Copyright (c) 2017 by David Bird.
  ### The formulation and calculation method of an IAQ - Internal Air Quality index ###
  ### The provision of a general purpose webserver ###
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
  publish, distribute, but not to use it commercially for profit making or to sub-license and/or to sell copies of the Software or to
  permit persons to whom the Software is furnished to do so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  See more at http://dsbird.org.uk
*/


//################# LIBRARIES ################
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <ESP32WebServer.h>  //https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include "Adafruit_BME280.h" // search in library packages of Arduino for: "Adafruit BME280 Library" and install it

//################ VARIABLES ################
const char* ssid      = "﻿Si_Friends";     // WiFi SSID
const char* password  = "﻿!aur3v01r!"; // WiFi Password

//const char* ssid      = "******";     // WiFi SSID
//const char* password  = "﻿******"; // WiFi Password

String siteheading    = "ESP32-PoE with MOD-BME280";               // Site's Main Title
String sitetitle      = "ESP32-PoE Webserver";       // Appears on the tabe of a Web Browser
String yourfootnote   = "Olimex demo for ESP32-PoE"; // A foot note e.g. "My Web Site"
String siteversion    = "v1.0";  // Version of your Website

#define sitewidth  1024  // Adjust site page width in pixels as required
#define SEALEVELPRESSURE_HPA (1013.25)  // required for MOD-BME280

Adafruit_BME280 bme; // I2C

String webpage = ""; // General purpose variable to hold HTML code

ESP32WebServer server(80); // Start server on port 80 (default for a web-browser, change to your requirements, e.g. 8080 perhaps, if your Router uses port 80
// To access server from outside of a WiFi (LAN) network e.g. on port 8080 add a rule on your Router that forwards a connection request
// to http://your_network_WAN_address:8080 to http://your_network_LAN_address:8080 and then you can view your ESP server from anywhere.
// Example http://yourhome.ip:8080 and your ESP Server is at 192.168.0.40, then the request will be directed to http://192.168.0.40:8080


#define BUTTON_PRESSED()  (!digitalRead (34))

#define INDICATOR(X)\
{\
    if (X)\
      webpage += "<svg height='10' width='10'> <circle cx='5' cy='5' r='4' stroke='black' stroke-width='1' fill='LawnGreen' /> </svg>";\
    else\
      webpage += "<svg height='10' width='10'> <circle cx='5' cy='5' r='4' stroke='black' stroke-width='1' fill='Red' /> </svg>";\
}

void setup()
{
  Serial.begin(115200); // initialize serial communications
  Wire.begin (13, 16);  // SPP
    
  Serial.println("Connect your I2C sensors to the default SDA, SCL pins for your board shown here:");
  Serial.println("I2C SDA pin = " + String(SDA));
  Serial.println("I2C SCL pin = " + String(SCL)); // Connect I2C sensors to the default SDA and SCL pins! Check Serial port for details
  StartWiFi(ssid, password);
  StartTime();
  //---------------------------------------------------------------------- 
  Serial.println("Use this URL to connect: http://" + WiFi.localIP().toString() + "/"); // Print the IP address
  server.on("/",          PoE_Demo);   // If the user types at their browser http://192.168.0.100/ control is passed here and then to user_input, you get values for your program...
  server.on("/PoE_Demo",  PoE_Demo);  // // SPP

  server.onNotFound(handleNotFound);   // If the user types something that is not supported, say so
  server.begin(); Serial.println(F("Webserver started...")); // Start the webserver
  pinMode (34, INPUT);
}

void loop ()
{
  server.handleClient();
}

void handleNotFound()
{
  String message = "The request entered could not be found, please try again with a different option\n";
  server.send(404, "text/plain", message);
}

void PoE_Demo ()
{
  webpage = ""; // don't delete this command, it ensures the server works reliably!
  append_HTML_header();
  String IPaddress = WiFi.localIP().toString();
  webpage += "<form action=\"http://" + IPaddress + "/PoE_Demo\" method=\"POST\">";
  webpage += "<table style='font-family:arial,sans-serif;font-size:16px;border-collapse:collapse;text-align:center;width:90%;margin-left:auto;margin-right:auto;'>";
  webpage += "<tr>";
  webpage += "</tr>";
  webpage += "</table><br><br>";
  // And so-on

  // MOD-BME280
  webpage += "<br>";
  INDICATOR (bme.begin(0x76));

  webpage += "MOD-BME Data: ";
  webpage += "<label for='advanced'>Temperature:</label> ";
  webpage +=  "<textarea id='advanced' name='MOD_BME_Temperature' rows='1' cols='5' maxlength='5' wrap='hard' readonly>";
  webpage +=  bme.readTemperature();
  webpage += "</textarea> ";

  webpage += "<label for='advanced'>Pressure:</label> ";
  webpage +=  "<textarea id='advanced' name='MOD_BME_Pressure' rows='1' cols='5' maxlength='5' wrap='hard' readonly>";
  webpage +=  (bme.readPressure() / 100.0F);
  webpage += "</textarea> ";
  
  webpage += "<label for='advanced'>Altitude:</label> ";
  webpage +=   "<textarea id='advanced' name='MOD_BME_Altitude' rows='1' cols='5' maxlength='5' wrap='hard' readonly>";
  webpage +=   bme.readAltitude(SEALEVELPRESSURE_HPA);
  webpage += "</textarea> ";

  webpage += "<label for='advanced'>Humidity:</label> ";
  webpage +=  "<textarea id='advanced' name='MOD_BME_Humidity' rows='1' cols='5' maxlength='5' wrap='hard' readonly>";
  webpage +=  bme.readHumidity();
  webpage += "</textarea><br>";

  // And so-on
  webpage += "</form></body>";
  append_HTML_footer();

  server.send(200, "text/html", webpage); // Send a response to the client to enter their inputs, if needed, Enter=defaults
  if (server.args() > 0 )
  { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ )
    {
      String Argument_Name   = server.argName(i);
      String client_response = server.arg(i);
    }
  }
}

void StartWiFi(const char* ssid, const char* password)
{
  int connAttempts = 0;
  Serial.print(F("\r\nConnecting to: ")); Serial.println(String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED )
  {
    delay(500); Serial.print(".");
    if (connAttempts > 20) {
      Serial.println("Failed to connect to WiFi");
    }
    connAttempts++;
  }
  Serial.print(F("WiFi connected at: "));
  Serial.println(WiFi.localIP());
}

void StartTime()
{
  configTime(0, 0, "0.uk.pool.ntp.org", "time.nist.gov");
  setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1); // Set for your locale
  delay(200);
  GetTime();
}

String GetTime()
{
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    StartTime();
  }
  //See http://www.cplusplus.com/reference/ctime/strftime/
  //Serial.println(&timeinfo, "%a %b %d %Y   %H:%M:%S"); // Displays: Saturday, June 24 2017 14:05:49
  char output[50];
  strftime(output, 50, "%d/%m/%y %H:%M:%S", &timeinfo); // Format needed for Google Charts is "11/12/17 22:01:00"; //dd/mm/yy hh:hh:ss
  return output;
}

void append_HTML_header()
{
  webpage  = "";
  webpage += "<!DOCTYPE html><html><head>";
  webpage += "<meta http-equiv='refresh' content='2'>"; // 5-sec refresh time, test needed to prevent auto updates repeating some commands
  webpage += "<style>";
  webpage += "body {width:" + String(sitewidth) + "px;margin:0 auto;font-family:arial;font-size:14px;text-align:center;color:blue;background-color:#F7F2Fd;}";
  webpage += "h1 {background-color:#ffc66e;margin:16px 30px;}"; // Orange background
  webpage += "h3 {color:#9370DB;font-size:24px;width:auto;}";
  webpage += ".navbar{overflow:hidden;background-color:#558ED5;position:fixed;top:0;width:" + String(sitewidth) + "px;margin-left:30px;}";
  webpage += ".navbar a {float:left;display:block;color:yellow;text-align:center;padding:10px 12px;text-decoration: none;font-size:17px;}";
  webpage += ".main{padding:0px;margin:16px 30px;height:1000px;width:" + String(sitewidth) + "px;}";
  webpage += ".style1{text-align:center;font-size:16px;background-color:#FFE4B5;}";
  webpage += ".style2{text-align:left;font-size:16px;background-color:#F7F2Fd;width:auto;margin:0 auto;}";
  // Note: You cannot include (table, tr, td, or th) styles if you want Google Charts to work!
  webpage += "</style>";
  webpage += "</head><body>";
  webpage += "<div class='navbar'>";
  // For each new page you add or remove, make sure there is a menu item to call it or remove it when not used
  //webpage += " <a href='/homepage'>Home</a>";
  //webpage += " <a href='/PoE_Demo'>PoE Demo</a>";// SPP
  webpage += "</div>";
  webpage += "<br><title>" + sitetitle + "</title><br>";
  webpage += "<div class='main'><h1>" + siteheading + " " + siteversion + "</h1>";
}

void append_HTML_footer()
{
  webpage += "<footer><p>" + yourfootnote + "<br>";
  webpage += "&copy; Original project made by David Bird. You can find it here:<br>https://github.com/G6EJD/ESP32-General-Purpose-Webserver</p></footer>";
  webpage += "</div></body></html>";
}
