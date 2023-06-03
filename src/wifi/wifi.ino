
#include <WiFi.h>
#include <WebServer.h>

std::string stURL="http://google.es";

void read_url_task(void * parameter);

const char* ssid     = "yourssid";
const char* password = "yourpasswd";

WebServer server(80);
void handle_root();

String head="<!DOCTYPE html> \
<html> \
<head> \
<meta http-equiv=\"refresh\" content=\"2; url='";
    
String tail="'\" /> \
</head> \
<body> \
<p>You will be redirected soon! .. </p> \
</body> \
</html>";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();

//serial 2

  Serial2.begin(9600);
  xTaskCreate(read_url_task, "read_url_task",1000, NULL,1, NULL);

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
  server.on("/", handle_root);

}

void read_url_task(void * parameter) {
  char dato;
  std::string stTMP="";

  while (1) {

    while (Serial2.available() > 0){

        dato=Serial2.read();

        if (dato==0) { //fin mensaje

          if (stTMP!=stURL) {

            Serial.print("New URL: ");
            Serial.println(stTMP.c_str());

            stURL=stTMP;
            
          }

          stTMP="";
        }
        else { //mensaje

          stTMP=stTMP+dato;
        }
    }

    delay(500);

  }
}

void handle_root() {
  String html;

  html=head+stURL.c_str()+tail;

  server.send(200, "text/html", html);

}

void loop() {
}
