#include <Arduino.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>

const char *ssid = "OpenDrop";
const char *password = "123456789";

//IPAddress local_IP(192, 168, 1, 1); // Set your server's fixed IP address here
//IPAddress gateway(192, 168, 1, 1);    // Set your network Gateway usually your Router base address
//IPAddress subnet(255, 255, 255, 0);   // Set your network sub-network mask here

AsyncWebServer server(80);

void setup() {
  //----------------------------------------------------Serial
  Serial.begin(115200);
  // while(!Serial){}
  Serial.println("\n");

  //----------------------------------------------------SD Handling
  if(!SD_MMC.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

//----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
//  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
//    Serial.println("WiFi STATION Failed to configure Correctly"); 
//  } 
  WiFi.softAP(ssid, password);
  Serial.print("Tentative de connexion...");
  
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------LOADING FILES OF SERVER + HOME PAGE ROUTE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.js", "text/javascript");
  });
  server.on("/spinner.gif", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/spinner.gif", "image/gif"); //
  });
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/logo.png", "image/gif"); //
  });
  server.on("/dropzone.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/dropzone.min.css", "text/css");
  });
  server.on("/dropzone.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/dropzone.min.js", "text/javascript");
  });

  //----------------------------------------------------SERVER REQUEST ROUTES

  server.on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
  {
      request->send(200, "text/plain", filesList(SD_MMC, "/", 0));
  });

  server.on("/",HTTP_POST,
            [](AsyncWebServerRequest * request){
                
                request->send(200); // Ready to receive file
             }, 
            [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
                
                saveFileToSD(SD_MMC, request, filename, index, data, len, final);
  });

  

  server.begin();
  Serial.println("Serveur actif!");
}

void loop() {
  // put your main code here, to run repeatedly:

}


String filesList(fs::FS &fs, const char * dirname, uint8_t levels) {
  String returnText = "";
  File root = fs.open(dirname);
  if(!root){
      Serial.println("Failed to open directory");
  }
  if(!root.isDirectory()){
      Serial.println("Not a directory");
  }
  File file = root.openNextFile();
  while(file){
        if(!file.isDirectory()){
           returnText += "<a href='"+String(file.name())+"'>"+String(file.name())+"</a>";
        }
        file = root.openNextFile();
  }
//  file.close();
//  root.close();
  return returnText;
}

void saveFileToSD(fs::FS &fs, AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if (!index) {
      String file = filename;
      if (!filename.startsWith("/")) file = "/" + filename;
      request->_tempFile = fs.open(file, FILE_WRITE);
      if (!request->_tempFile) Serial.println("Error creating file for upload...");
    }

    if (request->_tempFile) {
      if (len) {
        request->_tempFile.write(data, len); // Chunked data
        Serial.println("Transferred : " + String(len) + " Bytes");
      }
      if (final) {
        Serial.println("Upload size ="+String(request->_tempFile.size()));
        request->_tempFile.close();
      }
    }
}

String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
