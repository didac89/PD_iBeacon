
#include <WiFi.h>

std::string stURL="http://google.es";

const char* ssid     = "yourssid";
const char* password = "yourpasswd";

WiFiServer server(80);

#define head "<!DOCTYPE html> \
<html> \
  <head> \
    <meta http-equiv=\"refresh\" content=\"2; url='"
    
#define tail "'\" /> \
  </head> \
  <body> \
    <p>You will be redirected soon! .. </p> \
  </body> \
</html>\r\n\r\n"




void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();


//Wifi ....

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); 

  if (client) {                        
    while (client.connected()) {  
      while (client.available()) { 
        client.read(); 
      }
      
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.print(head);
      client.print(stURL.c_str());
      client.println(tail);
      
    }
    
    client.stop();
  }
}
