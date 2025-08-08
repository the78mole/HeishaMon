#ifndef ESP32_COMPAT_H
#define ESP32_COMPAT_H

#ifdef ESP32

#include <ETH.h>

// WiFi status compatibility
#ifndef WL_STOPPED
#define WL_STOPPED 6 // Define missing WL_STOPPED
#endif

// ETH PHY type compatibility
#ifndef ETH_PHY_W5500
#define ETH_PHY_W5500 ETH_PHY_IP101 // Use available PHY type as fallback
#endif

// Helper functions for ETH API compatibility
inline bool eth_hasIP() { return ETH.localIP() != IPAddress(0, 0, 0, 0); }

inline bool eth_connected() { return ETH.localIP() != IPAddress(0, 0, 0, 0); }

inline int eth_phyAddr() {
  return 1; // Return default PHY address since phyAddr() doesn't exist in
            // ESP32 3.x
}

#endif // ESP32

#endif // ESP32_COMPAT_H
