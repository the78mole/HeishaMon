/*
  Copyright (C) HeishaMon Contributors

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef __SCRIPT_ENGINE_H_
#define __SCRIPT_ENGINE_H_

#include <Arduino.h>
#include <LittleFS.h>

class ScriptEngine {
public:
    ScriptEngine();
    ~ScriptEngine();
    
    // Initialize the script engine
    bool begin();
    
    // Execute a script by name
    bool executeScript(const char* scriptName);
    
    // Execute a script from string
    bool executeString(const char* script);
    
    // Handle events (replacement for rules_event_cb)
    void handleEvent(const char* prefix, const char* name);
    
    // Handle timer events (replacement for rules_timer_cb)
    void handleTimer(int nr);
    
    // Handle boot event (replacement for rules_boot)
    void handleBoot();
    
    // Parse and load scripts from filesystem
    int parseScripts();
    
    // Cleanup and deinitialize
    void deinitialize();
    
    // Check if script engine is initialized
    bool isInitialized() const { return initialized; }
    
private:
    bool initialized;
    
    // Helper methods
    bool loadScript(const char* filename, String& content);
    void registerGlobalFunctions();
    void setVariable(const char* name, const char* value);
    void setVariable(const char* name, float value);
    void setVariable(const char* name, int value);
};

// Global script engine instance
extern ScriptEngine scriptEngine;

#endif // __SCRIPT_ENGINE_H_