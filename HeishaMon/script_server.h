/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef __SCRIPT_SERVER_H_
#define __SCRIPT_SERVER_H_

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>

// Simple HTTP server for script editing
// Provides basic file upload/download via HTTP for script management

class ScriptServer {
public:
    ScriptServer(int port = 8080);
    ~ScriptServer();
    
    // Initialize and start script server
    bool begin();
    
    // Handle client requests
    void handleClient();
    
    // Stop the server
    void stop();
    
    // Check if server is running
    bool isRunning() const { return running; }
    
private:
    WiFiServer* server;
    int serverPort;
    bool running;
    
    // HTTP method handlers
    void handleRequest(WiFiClient& client);
    void sendFileList(WiFiClient& client, const String& path);
    void sendFile(WiFiClient& client, const String& filename);
    void receiveFile(WiFiClient& client, const String& filename, int contentLength);
    
    // Helper methods
    void sendHttpHeader(WiFiClient& client, int code, const String& contentType, int contentLength = -1);
    String urlDecode(const String& str);
    String getContentType(const String& filename);
};

// Global script server instance
extern ScriptServer scriptServer;

#endif // __SCRIPT_SERVER_H_