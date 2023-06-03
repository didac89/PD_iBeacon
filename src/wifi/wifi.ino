
#include <WiFi.h>

std::string stURL="http://google.es";
SemaphoreHandle_t mutex_url;

void read_url_task(void * parameter);

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

//serial 2

  Serial2.begin(115200);
  xTaskCreate(read_url_task, "read_url_task",10000, NULL,1, NULL);

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

void read_url_task(void * parameter) {
  char dato;
  std::string stTMP;

  while(Serial2.available() > 0){

      dato=Serial2.read();

      if (dato==1) { //inicio mensaje
        stTMP="";
      }
      else if (dato==2) { //fin mensaje

        if (stTMP!=stURL) {

          Serial.print("New URL: ");
          Serial.println(stTMP.c_str());

          xSemaphoreTake(mutex_url, portMAX_DELAY);
          stURL=stTMP;
          xSemaphoreGive(mutex_url);
        }
      }
      else { //mensaje

        stTMP=stTMP+dato;
      }
  }
  delay(500);
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

      xSemaphoreTake(mutex_url, portMAX_DELAY);
      client.print(stURL.c_str());
      xSemaphoreGive(mutex_url);

      client.println(tail);

    }
    
    client.stop();
  }
}
