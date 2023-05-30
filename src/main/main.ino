
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEBeacon.h>

#define DEVICE_NAME            "ESP32"
#define SERVICE_UUID           "7A0247E7-8E88-409B-A959-AB5092DDB03E"
#define BEACON_UUID            "2D7A9F0C-E0E8-4CC9-A71B-A21DB2D034A1"
#define BEACON_UUID_REV        "A134D0B2-1DA2-1BA7-C94C-E8E00C9F7A2D"
#define CHARACTERISTIC_UUID    "82258BAA-DF72-47E8-99BC-B73D7ECD08A5"

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

BLEServer *pServer;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool mode_set_url = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("deviceConnected = true");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("deviceConnected = false");

      // Restart advertising to be visible and connectable again
      BLEAdvertising* pAdvertising;
      pAdvertising = pServer->getAdvertising();
      pAdvertising->start();
      Serial.println("iBeacon advertising restarted");
    }
};


//************************************************************
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {

      std::string tmp;

      tmp=pCharacteristic->getValue();

      if (mode_set_url) {
        mode_set_url=false;

        stURL=tmp;
        
        Serial.print("Saved URL: ");
        Serial.println(stURL.c_str());

      }
      else {
        if (tmp=="password") {
            mode_set_url=true;

            Serial.println("Waiting URL to save...");
        }
      }

    }

    void onRead(BLECharacteristic *pCharacteristic) {
      
      pCharacteristic->setValue(stURL);
      pCharacteristic->notify();

    }
};
//*******************************************************************


void init_service() {
  BLEAdvertising* pAdvertising;
  pAdvertising = pServer->getAdvertising();
  pAdvertising->stop();

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID));

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pAdvertising->addServiceUUID(BLEUUID(SERVICE_UUID));

  // Start the service
  pService->start();

  pAdvertising->start();
}

void init_beacon() {
  BLEAdvertising* pAdvertising;
  pAdvertising = pServer->getAdvertising();
  pAdvertising->stop();
  // iBeacon
  BLEBeacon myBeacon;
  myBeacon.setManufacturerId(0x4c00);
  myBeacon.setMajor(5);
  myBeacon.setMinor(88);
  myBeacon.setSignalPower(0xc5);
  myBeacon.setProximityUUID(BLEUUID(BEACON_UUID_REV));

  BLEAdvertisementData advertisementData;
  advertisementData.setFlags(0x1A);
  advertisementData.setManufacturerData(myBeacon.getData());
  pAdvertising->setAdvertisementData(advertisementData);

  pAdvertising->start();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();

//Bluetooth ...

  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  init_service();
  init_beacon();

  Serial.println("iBeacon + service defined and advertising!");

//Wifi ....

  /*Serial.println();
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
  
  server.begin();*/
}

void loop() {
  /*WiFiClient client = server.available(); 

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
  }*/
}
