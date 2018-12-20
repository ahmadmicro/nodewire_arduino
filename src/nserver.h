#ifndef NSERVER_H
#define NSERVER_H

#ifdef ESP32
#include <SPIFFS.h>
#else
#include <FS.h>
#endif

/*
 * upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
 * or you can upload the contents of a folder if you CD in that folder and run the following command:
 * for file in `ls -A1`; do curl -F "file=@$PWD/$file" mygw.local/edit; done
 * 
 * 
 * Uploading html, css, javascript, etc.
 * Use curl to upload the files from the SPIFFS data directory.
 *  cd data/
 *  curl -X POST -F "data=@index.htm"     http://<ESP32 IP address>/edit >/dev/null
 *  curl -X POST -F "data=@graphs.js.gz"  http://<ESP32 IP address>/edit >/dev/null
 *  curl -X POST -F "data=@favicon.ico"   http://<ESP32 IP address>/edit >/dev/null
 *  curl -X POST -F "data=@edit.htm.gz"   http://<ESP32 IP address>/edit >/dev/null
 */

extern WebServer http_server;

//holds the current upload
File fsUploadFile;

String getContentType(String filename){
  if(http_server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
    Serial.println("handleFileRead: " + path);
    if(path.endsWith("/")) path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
        if(SPIFFS.exists(pathWithGz))
        path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = http_server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void handleFileUpload(){
  if(http_server.uri() != "/edit") return;
  HTTPUpload& upload = http_server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileDelete(){
  if(http_server.args() == 0) return http_server.send(500, "text/plain", "BAD ARGS");
  String path = http_server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if(path == "/")
    return http_server.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return http_server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  http_server.send(200, "text/plain", "");
  path = String();
}


void handleFileCreate(){
  if(http_server.args() == 0)
    return http_server.send(500, "text/plain", "BAD ARGS");
  String path = http_server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if(path == "/")
    return http_server.send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return http_server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return http_server.send(500, "text/plain", "CREATE FAILED");
  http_server.send(200, "text/plain", "");
  path = String();
}


#endif