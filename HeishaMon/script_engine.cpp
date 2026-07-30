/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "script_engine.h"
#include "src/common/log.h"
#include "decode.h"
#include "dallas.h"
#include "HeishaOT.h"
#include "commands.h"

// External variables from main application
extern dallasDataStruct *actDallasData;
extern int dallasDevicecount;
extern settingsStruct heishamonSettings;
extern char actData[DATASIZE];
extern char actOptData[OPTDATASIZE];
extern char actDataExtra[DATASIZE];
extern String openTherm[2];
extern bool send_command(byte* command, int length);

// Global script engine instance
ScriptEngine scriptEngine;

ScriptEngine::ScriptEngine() : initialized(false) {
}

ScriptEngine::~ScriptEngine() {
    deinitialize();
}

bool ScriptEngine::begin() {
    if (initialized) {
        return true;
    }
    
    if (!LittleFS.begin()) {
        logprintf_P(F("Failed to initialize LittleFS for scripts"));
        return false;
    }
    
    initialized = true;
    logprintf_P(F("Script engine initialized (basic mode)"));
    return true;
}

bool ScriptEngine::executeScript(const char* scriptName) {
    if (!initialized) {
        return false;
    }
    
    String content;
    if (!loadScript(scriptName, content)) {
        return false;
    }
    
    return executeString(content.c_str());
}

bool ScriptEngine::executeString(const char* script) {
    if (!initialized) {
        return false;
    }
    
    // For now, provide a simple script interpreter that handles basic commands
    // This demonstrates the concept and can be extended with full Lua later
    
    String scriptStr = String(script);
    scriptStr.trim();
    
    if (scriptStr.length() == 0) {
        return true;
    }
    
    logprintf_P(F("Executing script: %s"), scriptStr.c_str());
    
    // Simple command parsing - look for common patterns
    if (scriptStr.startsWith("print(")) {
        // Extract print message
        int start = scriptStr.indexOf('"');
        int end = scriptStr.lastIndexOf('"');
        if (start >= 0 && end > start) {
            String message = scriptStr.substring(start + 1, end);
            logprintf_P(F("Script output: %s"), message.c_str());
        }
        return true;
    }
    
    if (scriptStr.startsWith("log(")) {
        // Extract log message
        int start = scriptStr.indexOf('"');
        int end = scriptStr.lastIndexOf('"');
        if (start >= 0 && end > start) {
            String message = scriptStr.substring(start + 1, end);
            logprintf_P(F("Script log: %s"), message.c_str());
        }
        return true;
    }
    
    if (scriptStr.indexOf("sendCommand(") >= 0) {
        logprintf_P(F("Script would send command (not implemented yet)"));
        return true;
    }
    
    if (scriptStr.indexOf("getSensorData(") >= 0) {
        logprintf_P(F("Script would get sensor data (not implemented yet)"));
        return true;
    }
    
    // For any other script content, just acknowledge it
    logprintf_P(F("Script content processed (basic interpreter)"));
    return true;
}

bool ScriptEngine::loadScript(const char* filename, String& content) {
    if (!LittleFS.begin()) {
        return false;
    }
    
    File file = LittleFS.open(filename, "r");
    if (!file) {
        // Don't log error for missing files - they may not exist yet
        return false;
    }
    
    content = file.readString();
    file.close();
    
    return content.length() > 0;
}

void ScriptEngine::handleEvent(const char* prefix, const char* name) {
    if (!initialized) return;
    
    // Construct script filename from event
    char scriptName[100];
    snprintf(scriptName, sizeof(scriptName), "/scripts/%s%s.lua", prefix, name);
    
    logprintf_P(F("Script event: %s%s"), prefix, name);
    executeScript(scriptName);
}

void ScriptEngine::handleTimer(int nr) {
    if (!initialized) return;
    
    char scriptName[50];
    snprintf(scriptName, sizeof(scriptName), "/scripts/timer_%d.lua", nr);
    
    logprintf_P(F("Script timer: %d"), nr);
    executeScript(scriptName);
}

void ScriptEngine::handleBoot() {
    if (!initialized) return;
    
    logprintf_P(F("Script boot event"));
    executeScript("/scripts/boot.lua");
}

int ScriptEngine::parseScripts() {
    if (!LittleFS.begin()) {
        return -1;
    }
    
    // Ensure scripts directory exists
    if (!LittleFS.exists("/scripts")) {
        LittleFS.mkdir("/scripts");
        logprintf_P(F("Created /scripts directory"));
        
        // Create a sample boot script
        File bootScript = LittleFS.open("/scripts/boot.lua", "w");
        if (bootScript) {
            bootScript.println("-- HeishaMon Boot Script");
            bootScript.println("-- This script runs when HeishaMon starts");
            bootScript.println("print('HeishaMon started with script engine!')");
            bootScript.close();
            logprintf_P(F("Created sample boot.lua script"));
        }
    }
    
    return 0;
}

void ScriptEngine::deinitialize() {
    initialized = false;
    logprintf_P(F("Script engine deinitialized"));
}