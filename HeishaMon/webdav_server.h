/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef __WEBDAV_SERVER_H_
#define __WEBDAV_SERVER_H_

#include <Arduino.h>
#include <LittleFS.h>

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WebServer.h>
#include <WiFi.h>
#endif

class WebDAVServer {
public:
    WebDAVServer(int port = 8080);
    ~WebDAVServer();
    
    // Initialize and start WebDAV server
    bool begin();
    
    // Handle client requests
    void handleClient();
    
    // Stop the server
    void stop();
    
    // Check if server is running
    bool isRunning() const { return running; }
    
private:
#ifdef ESP8266
    ESP8266WebServer* server;
#elif defined(ESP32)
    WebServer* server;
#endif
    
    int serverPort;
    bool running;
    
    // WebDAV method handlers
    void handleOPTIONS();
    void handlePROPFIND();
    void handleGET();
    void handlePUT();
    void handleDELETE();
    void handleMKCOL();
    void handleMOVE();
    void handleCOPY();
    void handleNotFound();
    
    // Helper methods
    void sendDAVHeaders();
    void sendMultiStatus(const String& path);
    void sendPropResponse(const String& path, bool isDir = false);
    String urlDecode(const String& str);
    String getContentType(const String& filename);
    String formatFileTime(time_t time);
};

// Global WebDAV server instance
extern WebDAVServer webdavServer;

#endif // __WEBDAV_SERVER_H_