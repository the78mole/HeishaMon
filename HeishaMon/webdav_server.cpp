/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "webdav_server.h"
#include "src/common/log.h"

// Global WebDAV server instance
WebDAVServer webdavServer;

WebDAVServer::WebDAVServer(int port) : serverPort(port), running(false) {
#ifdef ESP8266
    server = new ESP8266WebServer(port);
#elif defined(ESP32)
    server = new WebServer(port);
#endif
}

WebDAVServer::~WebDAVServer() {
    stop();
    delete server;
}

bool WebDAVServer::begin() {
    if (running) {
        return true;
    }
    
    if (!LittleFS.begin()) {
        logprintf_P(F("Failed to initialize LittleFS for WebDAV"));
        return false;
    }
    
    // Ensure scripts directory exists
    if (!LittleFS.exists("/scripts")) {
        LittleFS.mkdir("/scripts");
    }
    
    // Set up WebDAV handlers
    server->on("*", HTTP_OPTIONS, [this]() { handleOPTIONS(); });
    server->on("*", HTTP_GET, [this]() { handleGET(); });
    server->on("*", HTTP_PUT, [this]() { handlePUT(); });
    server->on("*", HTTP_DELETE, [this]() { handleDELETE(); });
    
    server->onNotFound([this]() { handleNotFound(); });
    
    server->begin();
    running = true;
    
    logprintf_P(F("WebDAV server started on port %d"), serverPort);
    return true;
}

void WebDAVServer::handleClient() {
    if (running && server) {
        server->handleClient();
    }
}

void WebDAVServer::stop() {
    if (running && server) {
        server->stop();
        running = false;
        logprintf_P(F("WebDAV server stopped"));
    }
}

void WebDAVServer::sendDAVHeaders() {
    server->sendHeader("DAV", "1, 2");
    server->sendHeader("Allow", "OPTIONS, GET, HEAD, POST, PUT, DELETE, PROPFIND, PROPPATCH, MKCOL, COPY, MOVE, LOCK, UNLOCK");
    server->sendHeader("Access-Control-Allow-Origin", "*");
    server->sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PROPFIND, MKCOL");
    server->sendHeader("Access-Control-Allow-Headers", "Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, If-Modified-Since, X-File-Name, Cache-Control");
}

void WebDAVServer::handleOPTIONS() {
    sendDAVHeaders();
    server->send(200, "text/plain", "");
}

void WebDAVServer::handlePROPFIND() {
    sendDAVHeaders();
    
    String path = urlDecode(server->uri());
    if (path.startsWith("/scripts")) {
        path = path.substring(8); // Remove /scripts prefix
    }
    if (path.length() == 0) {
        path = "/";
    }
    
    sendMultiStatus(path);
}

void WebDAVServer::handleGET() {
    String path = urlDecode(server->uri());
    
    if (path.startsWith("/scripts")) {
        path = path.substring(8); // Remove /scripts prefix  
    }
    
    String fullPath = "/scripts" + path;
    
    if (LittleFS.exists(fullPath)) {
        File file = LittleFS.open(fullPath, "r");
        if (file && !file.isDirectory()) {
            String contentType = getContentType(fullPath);
            server->streamFile(file, contentType);
            file.close();
            return;
        }
        file.close();
    }
    
    server->send(404, "text/plain", "File not found");
}

void WebDAVServer::handlePUT() {
    String path = urlDecode(server->uri());
    
    if (path.startsWith("/scripts")) {
        path = path.substring(8);
    }
    
    String fullPath = "/scripts" + path;
    
    // Ensure directory exists
    int lastSlash = fullPath.lastIndexOf('/');
    if (lastSlash > 0) {
        String dir = fullPath.substring(0, lastSlash);
        if (!LittleFS.exists(dir)) {
            LittleFS.mkdir(dir);
        }
    }
    
    File file = LittleFS.open(fullPath, "w");
    if (file) {
        String body = server->arg("plain");
        file.print(body);
        file.close();
        
        sendDAVHeaders();
        server->send(201, "text/plain", "Created");
        
        logprintf_P(F("WebDAV: Created/updated file %s"), fullPath.c_str());
    } else {
        server->send(500, "text/plain", "Failed to create file");
    }
}

void WebDAVServer::handleDELETE() {
    String path = urlDecode(server->uri());
    
    if (path.startsWith("/scripts")) {
        path = path.substring(8);
    }
    
    String fullPath = "/scripts" + path;
    
    if (LittleFS.exists(fullPath)) {
        if (LittleFS.remove(fullPath)) {
            sendDAVHeaders();
            server->send(204, "text/plain", "");
            logprintf_P(F("WebDAV: Deleted file %s"), fullPath.c_str());
        } else {
            server->send(500, "text/plain", "Failed to delete file");
        }
    } else {
        server->send(404, "text/plain", "File not found");
    }
}

void WebDAVServer::handleMKCOL() {
    String path = urlDecode(server->uri());
    
    if (path.startsWith("/scripts")) {
        path = path.substring(8);
    }
    
    String fullPath = "/scripts" + path;
    
    if (LittleFS.mkdir(fullPath)) {
        sendDAVHeaders();
        server->send(201, "text/plain", "Directory created");
        logprintf_P(F("WebDAV: Created directory %s"), fullPath.c_str());
    } else {
        server->send(500, "text/plain", "Failed to create directory");
    }
}

void WebDAVServer::handleMOVE() {
    // Simplified MOVE implementation
    server->send(501, "text/plain", "MOVE not implemented");
}

void WebDAVServer::handleCOPY() {
    // Simplified COPY implementation  
    server->send(501, "text/plain", "COPY not implemented");
}

void WebDAVServer::handleNotFound() {
    server->send(404, "text/plain", "Not found");
}

void WebDAVServer::sendMultiStatus(const String& path) {
    String response = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    response += "<D:multistatus xmlns:D=\"DAV:\">\n";
    
    // Add the requested path
    response += sendPropResponse(path, false);
    
    response += "</D:multistatus>";
    
    server->send(207, "application/xml", response);
}

String WebDAVServer::sendPropResponse(const String& path, bool isDir) {
    String fullPath = "/scripts" + path;
    String response = "<D:response>\n";
    response += "<D:href>/scripts" + path + "</D:href>\n";
    response += "<D:propstat>\n<D:prop>\n";
    
    if (isDir) {
        response += "<D:resourcetype><D:collection/></D:resourcetype>\n";
    } else {
        response += "<D:resourcetype/>\n";
        if (LittleFS.exists(fullPath)) {
            File file = LittleFS.open(fullPath, "r");
            if (file) {
                response += "<D:getcontentlength>" + String(file.size()) + "</D:getcontentlength>\n";
                response += "<D:getcontenttype>" + getContentType(fullPath) + "</D:getcontenttype>\n";
                file.close();
            }
        }
    }
    
    response += "</D:prop>\n<D:status>HTTP/1.1 200 OK</D:status>\n</D:propstat>\n";
    response += "</D:response>\n";
    
    return response;
}

String WebDAVServer::urlDecode(const String& str) {
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

String WebDAVServer::getContentType(const String& filename) {
    if (filename.endsWith(".lua")) return "text/x-lua";
    if (filename.endsWith(".txt")) return "text/plain";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    return "application/octet-stream";
}