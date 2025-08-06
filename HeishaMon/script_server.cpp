/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "script_server.h"
#include "src/common/log.h"

// Global script server instance
ScriptServer scriptServer;

ScriptServer::ScriptServer(int port) : serverPort(port), running(false) {
    server = new WiFiServer(port);
}

ScriptServer::~ScriptServer() {
    stop();
    delete server;
}

bool ScriptServer::begin() {
    if (running) {
        return true;
    }
    
    if (!LittleFS.begin()) {
        logprintf_P(F("Failed to initialize LittleFS for script server"));
        return false;
    }
    
    // Ensure scripts directory exists
    if (!LittleFS.exists("/scripts")) {
        LittleFS.mkdir("/scripts");
    }
    
    server->begin();
    running = true;
    
    logprintf_P(F("Script server started on port %d"), serverPort);
    return true;
}

void ScriptServer::handleClient() {
    if (!running || !server) {
        return;
    }
    
    WiFiClient client = server->available();
    if (client) {
        logprintf_P(F("Script server: new client connected"));
        handleRequest(client);
        client.stop();
    }
}

void ScriptServer::stop() {
    if (running && server) {
        server->stop();
        running = false;
        logprintf_P(F("Script server stopped"));
    }
}

void ScriptServer::handleRequest(WiFiClient& client) {
    String currentLine = "";
    String requestLine = "";
    String method = "";
    String path = "";
    int contentLength = 0;
    
    // Read the request headers
    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            if (c == '\n') {
                if (currentLine.length() == 0) {
                    // End of headers
                    break;
                } else {
                    // Process header line
                    if (requestLine.length() == 0) {
                        requestLine = currentLine;
                        int firstSpace = currentLine.indexOf(' ');
                        int secondSpace = currentLine.indexOf(' ', firstSpace + 1);
                        if (firstSpace > 0 && secondSpace > firstSpace) {
                            method = currentLine.substring(0, firstSpace);
                            path = currentLine.substring(firstSpace + 1, secondSpace);
                        }
                    } else if (currentLine.startsWith("Content-Length:")) {
                        contentLength = currentLine.substring(15).toInt();
                    }
                    currentLine = "";
                }
            } else if (c != '\r') {
                currentLine += c;
            }
        }
    }
    
    // Process the request
    if (method == "GET") {
        if (path == "/" || path == "/scripts" || path == "/scripts/") {
            sendFileList(client, "/scripts");
        } else if (path.startsWith("/scripts/")) {
            sendFile(client, path);
        } else {
            sendHttpHeader(client, 404, "text/plain");
            client.println("File not found");
        }
    } else if (method == "POST" && path.startsWith("/scripts/")) {
        receiveFile(client, path, contentLength);
    } else {
        sendHttpHeader(client, 405, "text/plain");
        client.println("Method not allowed");
    }
}

void ScriptServer::sendFileList(WiFiClient& client, const String& path) {
    sendHttpHeader(client, 200, "text/html");
    
    client.println("<!DOCTYPE html><html><head><title>HeishaMon Scripts</title>");
    client.println("<style>body{font-family:Arial,sans-serif;margin:20px;} .file{padding:5px;} a{color:#0066cc;text-decoration:none;}</style>");
    client.println("</head><body>");
    client.println("<h1>HeishaMon Script Files</h1>");
    client.println("<p>Upload scripts by sending POST requests to /scripts/filename.lua</p>");
    client.println("<ul>");
    
    File dir = LittleFS.open("/scripts", "r");
    if (dir && dir.isDirectory()) {
        File file = dir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                client.printf("<li class='file'><a href='/scripts/%s'>%s</a> (%d bytes)</li>\n", 
                             file.name(), file.name(), file.size());
            }
            file = dir.openNextFile();
        }
    }
    dir.close();
    
    client.println("</ul>");
    client.println("<hr>");
    client.println("<h2>Sample Script Upload (curl)</h2>");
    client.println("<pre>curl -X POST http://your-ip:8080/scripts/test.lua --data 'print(\"Hello from Lua!\")'</pre>");
    client.println("</body></html>");
}

void ScriptServer::sendFile(WiFiClient& client, const String& filename) {
    File file = LittleFS.open(filename, "r");
    if (!file) {
        sendHttpHeader(client, 404, "text/plain");
        client.println("File not found");
        return;
    }
    
    sendHttpHeader(client, 200, getContentType(filename), file.size());
    
    while (file.available()) {
        client.write(file.read());
    }
    file.close();
}

void ScriptServer::receiveFile(WiFiClient& client, const String& filename, int contentLength) {
    if (contentLength <= 0) {
        sendHttpHeader(client, 400, "text/plain");
        client.println("Bad request - no content");
        return;
    }
    
    File file = LittleFS.open(filename, "w");
    if (!file) {
        sendHttpHeader(client, 500, "text/plain");
        client.println("Failed to create file");
        return;
    }
    
    // Read the content
    int bytesRead = 0;
    while (bytesRead < contentLength && client.connected()) {
        if (client.available()) {
            char c = client.read();
            file.write(c);
            bytesRead++;
        }
    }
    
    file.close();
    
    sendHttpHeader(client, 201, "text/plain");
    client.printf("File %s created successfully (%d bytes)\n", filename.c_str(), bytesRead);
    
    logprintf_P(F("Script server: Created file %s (%d bytes)"), filename.c_str(), bytesRead);
}

void ScriptServer::sendHttpHeader(WiFiClient& client, int code, const String& contentType, int contentLength) {
    client.printf("HTTP/1.1 %d %s\r\n", code, 
                  code == 200 ? "OK" : 
                  code == 201 ? "Created" : 
                  code == 404 ? "Not Found" : 
                  code == 400 ? "Bad Request" :
                  code == 405 ? "Method Not Allowed" :
                  code == 500 ? "Internal Server Error" : "Unknown");
    
    client.printf("Content-Type: %s\r\n", contentType.c_str());
    client.println("Access-Control-Allow-Origin: *");
    client.println("Access-Control-Allow-Methods: GET, POST, OPTIONS");
    client.println("Access-Control-Allow-Headers: Content-Type");
    
    if (contentLength >= 0) {
        client.printf("Content-Length: %d\r\n", contentLength);
    }
    
    client.println("Connection: close");
    client.println();
}

String ScriptServer::urlDecode(const String& str) {
    String decoded = "";
    char temp[] = "00";
    int len = str.length();
    
    for (int i = 0; i < len; i++) {
        char c = str.charAt(i);
        if (c == '+') {
            decoded += ' ';
        } else if (c == '%' && i + 2 < len) {
            temp[0] = str.charAt(i + 1);
            temp[1] = str.charAt(i + 2);
            decoded += char(strtol(temp, NULL, 16));
            i += 2;
        } else {
            decoded += c;
        }
    }
    
    return decoded;
}

String ScriptServer::getContentType(const String& filename) {
    if (filename.endsWith(".lua")) return "text/x-lua";
    if (filename.endsWith(".txt")) return "text/plain";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".html")) return "text/html";
    return "application/octet-stream";
}