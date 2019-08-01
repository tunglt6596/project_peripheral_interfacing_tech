

/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "Connectify-Tung";
const char* password = "tung6596bkhn";

// Temperature and Humidity information
String tempe = "";
String humid = "";
    
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output2State = "off";
String output0State = "off";

// Assign output variables to GPIO pins
const int output2 = 2;
const int output0 = 0;

void setup() {
  Serial.begin(115200);
   
  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(output0, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(output0, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (Serial.available() > 0) {
      tempe = Serial.readStringUntil('\n');          // read temperature information
      humid = Serial.readStringUntil('\n');          // read humidity information
  }
  
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /2/on") >= 0) {
              output2State = "on";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              output2State = "off";
              digitalWrite(output2, LOW);
            } else if (header.indexOf("GET /0/on") >= 0) {
              output0State = "on";
              digitalWrite(output0, HIGH);
            } else if (header.indexOf("GET /0/off") >= 0) {
              output0State = "off";
              digitalWrite(output0, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1><font color=\"red\">3T-GROUP KTMT 59 Web Server</font></h1>");

            // Display temperature and humidity information
            String tempe_info = "<p><h1>" + tempe + "</h1></p>";
            client.println(tempe_info);
            String humid_info = "<p><h1>" + humid + "</h1></p>";
            client.println(humid_info);
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>GPIO 2 - State " + output2State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 0 
            client.println("<p>GPIO 0 - State " + output0State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output0State=="off") {
              client.println("<p><a href=\"/0/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/0/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}


